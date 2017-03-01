/*
 *
 *  Copyright (c) 2012-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "deviceoffset.h"

#include "keydialog.h"
#include "ui_keydialog.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QDir>
#include <QTableWidget>
#include <QDebug>
#include <QFile>

#include "bin_path.h"
#include "../zuluCrypt-gui/dialogmsg.h"
#include "../zuluCrypt-gui/task.h"
#include "../zuluCrypt-cli/constants.h"
#include "plugin_path.h"
#include "../zuluCrypt-gui/utility.h"
#include "lxqt_wallet.h"
#include "mountoptions.h"
#include "../zuluCrypt-gui/tcrypt.h"
#include "zulumounttask.h"
#include "../zuluCrypt-gui/task.h"
#include "zulumounttask.h"
#include "siritask.h"
#include "veracrypt_support.h"
#include "truecrypt_support.h"
#include "veracryptpimdialog.h"

#define KWALLET         "kde wallet"
#define INTERNAL_WALLET "internal wallet"
#define GNOME_WALLET    "gnome wallet"

keyDialog::keyDialog( QWidget * parent,
		      QTableWidget * table,
		      secrets& s,
		      const volumeProperty& e,
		      std::function< void() > p,
		      std::function< void( const QString& ) > q ) :
	QDialog( parent ),
	m_ui( new Ui::keyDialog ),
	m_secrets( s ),
	m_cancel( std::move( p ) ),
	m_success( std::move( q ) )
{
	m_ui->setupUi( this ) ;
	m_ui->checkBoxShareMountPoint->setToolTip( utility::shareMountPointToolTip() ) ;
	m_table = table ;
	m_path = e.volumeName() ;
	m_working = false ;
	m_encryptedFolder = e.fileSystem() == "cryptfs" ;

	decltype( tr( "" ) ) msg ;

	if( e.fileSystem() == "crypto_LUKS" ){

		msg = tr( "Mount A LUKS volume in \"%1\"").arg( m_path ) ;
	}else{
		msg = tr( "Mount An Encrypted Volume In \"%1\"").arg( m_path ) ;
	}

	m_ui->checkBoxVisibleKey->setToolTip( tr( "Check This Box To Make Password Visible" ) ) ;

	this->setWindowTitle( msg ) ;

	m_ui->lineEditMountPoint->setText( m_path ) ;
	m_ui->pbOpenMountPoint->setIcon( QIcon( ":/folder.png" ) ) ;

	m_menu = new QMenu( this ) ;

	connect( m_menu,SIGNAL( triggered( QAction * ) ),this,SLOT( pbPluginEntryClicked( QAction * ) ) ) ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;

	m_ui->lineEditKey->setFocus() ;

	m_ui->checkBoxOpenReadOnly->setChecked( utility::getOpenVolumeReadOnlyOption( "zuluMount-gui" ) ) ;

	m_ui->pbkeyOption->setEnabled( false ) ;

	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;

	m_veraCryptVolume = utility::autoSetVolumeAsVeraCrypt( "zuluMount-gui" ) ;

	m_ui->checkBoxVeraCryptVolume->setChecked( m_veraCryptVolume ) ;

	m_ui->checkBoxVeraCryptVolume->setEnabled( !m_encryptedFolder ) ;

	connect( m_ui->checkBoxVeraCryptVolume,SIGNAL( stateChanged( int ) ),this,SLOT( cbVeraCryptVolume( int ) ) ) ;
	connect( m_ui->pbOptions,SIGNAL( clicked() ),this,SLOT( pbOptions() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbkeyOption,SIGNAL( clicked() ),this,SLOT( pbkeyOption() ) ) ;
	connect( m_ui->pbOpenMountPoint,SIGNAL( clicked() ),this,SLOT( pbMountPointPath() ) ) ;
	connect( m_ui->checkBoxOpenReadOnly,SIGNAL( stateChanged( int ) ),this,SLOT( cbMountReadOnlyStateChanged( int ) ) ) ;
	connect( m_ui->cbKeyType,SIGNAL( currentIndexChanged( int ) ),this,SLOT( cbActicated( int ) ) ) ;
	connect( m_ui->checkBoxVisibleKey,SIGNAL( stateChanged( int ) ),this,SLOT( cbVisibleKeyStateChanged( int ) ) ) ;

	m_ui->pbOpenMountPoint->setVisible( false ) ;

	const auto& m = e.mountPoint() ;

	if( m.isEmpty() || m == "Nil" ){

		m_point = utility::mountPathPostFix( m_path.split( "/" ).last() ) ;
	}else{
		m_point = utility::mountPathPostFix( m ) ;
	}

	m_ui->lineEditMountPoint->setText( m_point ) ;

	this->addAction( [ this ](){

		auto ac = new QAction( this ) ;
		ac->setShortcut( Qt::CTRL + Qt::Key_F ) ;
		connect( ac,SIGNAL( triggered() ),this,SLOT( showOffSetWindowOption() ) ) ;

		return ac ;
	}() ) ;

	m_menu_1 = new QMenu( this ) ;

	m_menu_1->setFont( this->font() ) ;

	m_ui->pbOptions->setEnabled( !m_encryptedFolder ) ;

	if( !m_encryptedFolder ){

		auto _add_action = [ & ]( const QString& e ){

			m_menu_1->addAction( e ) ;
		} ;

		_add_action( tr( "Set File System Options" ) ) ;
		_add_action( tr( "Set Volume Offset" ) ) ;
		_add_action( tr( "Set VeraCrypt PIM value" ) ) ;
	}

	m_ui->cbKeyType->addItem( tr( "Key" ) ) ;
	m_ui->cbKeyType->addItem( tr( "KeyFile" ) ) ;
	m_ui->cbKeyType->addItem( tr( "Key+KeyFile" ) ) ;
	m_ui->cbKeyType->addItem( tr( "Plugin" ) ) ;

	if( m_encryptedFolder ){

		m_ui->checkBoxShareMountPoint->setEnabled( false ) ;
	}else{
		m_ui->cbKeyType->addItem( tr( "TrueCrypt/VeraCrypt Keys" ) ) ;
	}

	connect( m_menu_1,SIGNAL( triggered( QAction * ) ),this,SLOT( doAction( QAction * ) ) ) ;

	m_veraCryptWarning.setWarningLabel( m_ui->veraCryptWarning ) ;

	m_ui->pbkeyOption->setVisible( false ) ;
	m_ui->checkBoxVisibleKey->setVisible( true ) ;
	this->installEventFilter( this ) ;
}

bool keyDialog::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->pbCancel() ; } ) ;
}

void keyDialog::pbOptions()
{
	m_menu_1->exec( QCursor::pos() ) ;
}

void keyDialog::showOffSetWindowOption()
{
	deviceOffset::instance( this,false,[ this ]( const QString& e,const QString& f ){

		Q_UNUSED( f ) ;
		m_deviceOffSet = QString( " -o %1" ).arg( e ) ;
	} ) ;
}

void keyDialog::showFileSystemOptionWindow()
{
	mountOptions::instance( &m_options,this ) ;
}

void keyDialog::doAction( QAction * ac )
{
	auto e = ac->text() ;

	e.remove( "&" ) ;

	if( e == tr( "Set File System Options" ) ){

		this->showFileSystemOptionWindow() ;

	}else if( e == tr( "Set Volume Offset" ) ){

		this->showOffSetWindowOption() ;

	}else if( e == tr( "Set Volume As VeraCrypt Volume" ) ){

		m_veraCryptVolume = true ;

	}else if( e == tr( "Set VeraCrypt PIM value" ) ){

		VeraCryptPIMDialog::instance( this,[ this ]( int e ){

			m_veraCryptPIMValue = e ;
			m_veraCryptVolume = true ;

			m_ui->checkBoxVeraCryptVolume->setChecked( true ) ;
		} ) ;
	}
}

void keyDialog::cbVeraCryptVolume( int state )
{
	m_veraCryptVolume = state != Qt::Unchecked ;

	utility::autoSetVolumeAsVeraCrypt( "zuluMount-gui",m_veraCryptVolume ) ;
}

void keyDialog::cbMountReadOnlyStateChanged( int state )
{
	m_ui->checkBoxOpenReadOnly->setEnabled( false ) ;
	m_ui->checkBoxOpenReadOnly->setChecked( utility::setOpenVolumeReadOnly( this,state == Qt::Checked,QString( "zuluMount-gui" ) ) ) ;
	m_ui->checkBoxOpenReadOnly->setEnabled( true ) ;

	if( m_ui->lineEditKey->text().isEmpty() ){

		m_ui->lineEditKey->setFocus() ;

	}else if( m_ui->lineEditMountPoint->text().isEmpty() ){

		m_ui->lineEditMountPoint->setFocus() ;
	}else{
		m_ui->pbOpen->setFocus() ;
	}
}

void keyDialog::pbMountPointPath()
{
	auto msg = tr( "Select A Folder To Create A Mount Point In" ) ;
	auto Z = QFileDialog::getExistingDirectory( this,msg,utility::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( !Z.isEmpty() ){

		while( true ){

			if( Z.endsWith( '/' ) ){

				Z.truncate( Z.length() - 1 ) ;
			}else{
				break ;
			}
		}

		Z = Z + "/" + m_ui->lineEditMountPoint->text().split( "/" ).last() ;
		m_ui->lineEditMountPoint->setText( Z ) ;
	}
}

void keyDialog::cbVisibleKeyStateChanged( int s )
{
	if( m_ui->cbKeyType->currentIndex() == keyDialog::Key ){

		if( s == Qt::Checked ){

			m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
		}else{
			m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;
		}

		m_ui->lineEditKey->setFocus() ;
	}
}

void keyDialog::enableAll()
{
	m_ui->checkBoxVisibleKey->setEnabled( m_ui->cbKeyType->currentIndex() == keyDialog::Key ) ;
	m_ui->checkBoxVeraCryptVolume->setEnabled( true ) ;
	m_ui->pbOptions->setEnabled( !m_encryptedFolder ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->lineEditMountPoint->setEnabled( true ) ;
	m_ui->pbOpenMountPoint->setEnabled( true ) ;
	m_ui->pbCancel->setEnabled( true ) ;
	m_ui->pbOpen->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->cbKeyType->setEnabled( true ) ;

	auto index = m_ui->cbKeyType->currentIndex() ;

	m_ui->lineEditKey->setEnabled( index == keyDialog::Key ) ;

	m_ui->pbkeyOption->setEnabled( index == keyDialog::Key || index == keyDialog::keyfile ) ;
	m_ui->checkBoxOpenReadOnly->setEnabled( true ) ;

	m_ui->checkBoxShareMountPoint->setEnabled( !m_encryptedFolder ) ;
}

void keyDialog::disableAll()
{
	m_ui->checkBoxVisibleKey->setEnabled( false ) ;
	m_ui->checkBoxVeraCryptVolume->setEnabled( false ) ;
	m_ui->cbKeyType->setEnabled( false ) ;
	m_ui->pbOptions->setEnabled( false ) ;
	m_ui->pbkeyOption->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->lineEditMountPoint->setEnabled( false ) ;
	m_ui->pbOpenMountPoint->setEnabled( false ) ;
	m_ui->lineEditKey->setEnabled( false ) ;
	m_ui->pbCancel->setEnabled( false ) ;
	m_ui->pbOpen->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->checkBoxOpenReadOnly->setEnabled( false ) ;
	m_ui->checkBoxShareMountPoint->setEnabled( false ) ;
}

void keyDialog::KeyFile()
{
	if( m_ui->cbKeyType->currentIndex() == keyDialog::keyfile ){

		auto msg = tr( "Select A File To Be Used As A Keyfile" ) ;
		auto Z = QFileDialog::getOpenFileName( this,msg,utility::homePath() ) ;

		if( !Z.isEmpty() ){

			m_ui->lineEditKey->setText( Z ) ;
		}
	}
}

void keyDialog::pbkeyOption()
{
	auto keyType = m_ui->cbKeyType->currentIndex() ;

	if( keyType == keyDialog::plugin ){

		this->Plugin() ;

	}else if( keyType == keyDialog::keyfile ){

		this->KeyFile() ;
	}
}

void keyDialog::Plugin()
{
	utility::createPlugInMenu( m_menu,tr( INTERNAL_WALLET ),
				   tr( GNOME_WALLET ),tr( KWALLET ),!m_encryptedFolder ) ;

	m_menu->setFont( this->font() ) ;

	m_menu->addSeparator() ;

	m_menu->addAction( tr( "Cancel" ) ) ;

	m_menu->exec( QCursor::pos() ) ;
}

void keyDialog::pbPluginEntryClicked( QAction * e )
{
	auto r = e->text() ;

	r.remove( "&" ) ;

	if( r != tr( "Cancel" ) ){

		m_ui->lineEditKey->setText( r ) ;
	}
}

void keyDialog::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}

void keyDialog::pbOpen()
{
	this->disableAll() ;

	if( m_ui->cbKeyType->currentIndex() == keyDialog::plugin ){

		utility::wallet w ;

		auto wallet = m_ui->lineEditKey->text() ;

		if( wallet == tr( KWALLET ) ){

			auto s = m_secrets.walletBk( LXQt::Wallet::BackEnd::kwallet ) ;

			w = utility::getKey( s.bk(),m_path ) ;

		}else if( wallet == tr( INTERNAL_WALLET ) ){

			auto s = m_secrets.walletBk( LXQt::Wallet::BackEnd::internal ) ;

			w = utility::getKey( s.bk(),m_path,"zuluMount" ) ;

			if( w.notConfigured ){

				DialogMsg msg( this ) ;
				msg.ShowUIOK( tr( "ERROR!" ),tr( "Internal wallet is not configured" ) ) ;
				return this->enableAll() ;
			}

		}else if( wallet == tr( GNOME_WALLET ) ){

			auto s = m_secrets.walletBk( LXQt::Wallet::BackEnd::libsecret ) ;

			w = utility::getKey( s.bk(),m_path ) ;
		}else{
			return this->openVolume() ;
		}

		if( w.opened ){

			if( w.key.isEmpty() ){

				DialogMsg msg( this ) ;

				msg.ShowUIOK( tr( "ERROR" ),tr( "The volume does not appear to have an entry in the wallet" ) ) ;

				this->enableAll() ;

				if( m_ui->cbKeyType->currentIndex() != keyDialog::Key ){

					m_ui->lineEditKey->setEnabled( false ) ;
				}
			}else{
				m_key = w.key.toLatin1() ;
				this->openVolume() ;
			}
		}else{
			this->enableAll() ;
		}
	}else{
		this->openVolume() ;
	}
}

void keyDialog::encryptedFolderMount()
{
	DialogMsg msg( this ) ;

	if( m_key.isEmpty() ){

		msg.ShowUIOK( tr( "ERROR" ),tr( "Atleast one required field is empty" ) ) ;

		return this->enableAll() ;
	}

	auto m = utility::mountPath( utility::mountPathPostFix( m_ui->lineEditMountPoint->text() ) ) ;

	auto ro = m_ui->checkBoxOpenReadOnly->isChecked() ;

	auto e = siritask::encryptedFolderMount( { m_path,m,m_key,QString(),QString(),
						   QString(),ro,m_success } ).await() ;

	switch( e.status() ){

	case siritask::status::success :

		return this->HideUI() ;

	case siritask::status::cryfs :

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to unlock a cryfs volume.\nWrong password entered" ) ) ;
		break;

	case siritask::status::encfs :

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to unlock an encfs volume.\nWrong password entered" ) ) ;
		break;

	case siritask::status::gocryptfs :

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to unlock a gocryptfs volume.\nWrong password entered" ) ) ;
		break;

	case siritask::status::ecryptfs :

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to unlock an ecryptfs volume.\nWrong password entered" ) ) ;
		break;

	case siritask::status::securefs :

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to unlock a securefs volume.\nWrong password entered" ) ) ;
		break;

	case siritask::status::cryfsNotFound :

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to unlock a cryfs volume.\ncryfs executable could not be found" ) ) ;
		break;

	case siritask::status::securefsNotFound :

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to unlock a securefs volume.\nsecurefs executable could not be found" ) ) ;
		break;

	case siritask::status::gocryptfsNotFound :

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to unlock a gocryptfs volume.\ngocryptfs executable could not be found" ) ) ;
		break;

	case siritask::status::encfsNotFound :

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to unlock an encfs volume.\nencfs executable could not be found" ) ) ;
		break;

	case siritask::status::ecryptfs_simpleNotFound :

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to unlock an ecryptfs volume.\necryptfs-simple executable could not be found" ) ) ;
		break;

	case siritask::status::failedToCreateMountPoint :

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to create mount point" ) ) ;
		break;

	case siritask::status::unknown :

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed to unlock the volume.\nNot supported volume encountered" ) ) ;
		break;

	case siritask::status::backendFail :
	default:

		msg.ShowUIOK( tr( "ERROR" ),e.msg() ) ;
		break;
	}

	if( m_ui->cbKeyType->currentIndex() == keyDialog::Key ){

		m_ui->lineEditKey->clear() ;
	}

	m_ui->lineEditKey->setFocus() ;

	this->enableAll() ;
}

bool keyDialog::errorNotFound( int r )
{
	DialogMsg msg( this ) ;

	switch ( r ){
		case 0 : break ;
		case 1 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to mount ntfs/exfat file system using ntfs-3g,is ntfs-3g/exfat package installed?" ) ) ; break ;
		case 2 : msg.ShowUIOK( tr( "ERROR!" ),tr( "There seem to be an open volume accociated with given address" ) ) ;				break ;
		case 3 : msg.ShowUIOK( tr( "ERROR!" ),tr( "No file or device exist on given path" ) ) ; 						break ;
		case 4 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Volume could not be opened with the presented key" ) ) ;					break ;
		case 5 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to mount the device with given options" ) ) ;				break ;
		case 6 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to open device in read write mode or device does not exist" ) ) ;	break ;
		case 7 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Only root user can perform this operation" ) ) ;						break ;
		case 8 : msg.ShowUIOK( tr( "ERROR!" ),tr( "-O and -m options can not be used together" ) ) ;						break ;
		case 9 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not create mount point, invalid path or path already taken" ) ) ;			break ;
		case 10: msg.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point path aleady taken" ) ) ;							break ;
		case 11: msg.ShowUIOK( tr( "ERROR!" ),tr( "There seem to be an opened mapper associated with the device" ) ) ;				break ;
		case 12: msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not get a passphrase from the module" ) ) ;						break ;
		case 13: msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not get passphrase in silent mode" ) ) ;						break ;
		case 14: msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient memory to hold passphrase" ) ) ;						break ;
		case 15: msg.ShowUIOK( tr( "ERROR!" ),tr( "One or more required argument(s) for this operation is missing" ) ) ;			break ;
		case 16: msg.ShowUIOK( tr( "ERROR!" ),tr( "Invalid path to key file" ) ) ;								break ;
		case 17: msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not get enought memory to hold the key file" ) ) ;					break ;
		case 18: msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to open key file for reading" ) ) ;					break ;
		case 19: msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not get a passphrase through a local socket" ) ) ;					break ;
		case 20: msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to mount a filesystem:invalid/unsupported mount option or unsupported file system encountered" ) ) ;	break ;
		case 21: msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not create a lock on /etc/mtab" ) ) ;							break ;
		case 22: msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to open a system volume.\n\nConsult menu->help->permission for more informaion\n" ) ) ;					break ;
		case 113:msg.ShowUIOK( tr( "ERROR!" ),tr( "A non supported device encountered,device is missing or permission denied\n\
Possible reasons for getting the error are:\n1.Device path is invalid.\n2.The device has LVM or MDRAID signature" ) ) ;					break ;
		default: return true ;
	}

	return false ;
}

void keyDialog::openVolume()
{
	auto keyType = m_ui->cbKeyType->currentIndex() ;

	if( m_encryptedFolder ){

		if( keyType == keyDialog::Key ){

			m_key = m_ui->lineEditKey->text().toLatin1() ;

		}else if( keyType == keyDialog::keyfile ){

			QFile f( m_ui->lineEditKey->text() ) ;

			f.open( QIODevice::ReadOnly ) ;

			m_key = f.readAll() ;

		}else if( keyType == keyDialog::keyKeyFile ){

			if( utility::pluginKey( m_secrets.parent(),&m_key,"hmac" ) ){

				return this->enableAll() ;
			}

		}else if( keyType == keyDialog::plugin ){

			/*
			 * m_key is already set
			 */
		}

		return this->encryptedFolderMount() ;
	}

	if( m_ui->lineEditKey->text().isEmpty() ){

		 if( keyType == keyDialog::plugin ){

			DialogMsg msg( this ) ;
			msg.ShowUIOK( tr( "ERROR" ),tr( "Plug in name field is empty" ) ) ;

			m_ui->lineEditKey->setFocus() ;

			return this->enableAll() ;

		}else if( keyType == keyDialog::keyfile ){

			DialogMsg msg( this ) ;
			msg.ShowUIOK( tr( "ERROR" ),tr( "Keyfile field is empty" ) ) ;

			m_ui->lineEditKey->setFocus() ;

			return this->enableAll() ;
		}
	}

	auto test_name = m_ui->lineEditMountPoint->text() ;

	if( test_name.contains( "/" ) ){

		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "\"/\" character is not allowed in the mount name field" ) ) ;

		m_ui->lineEditKey->setFocus() ;

		return this->enableAll() ;
	}

	QString m ;
	if( keyType == keyDialog::Key ){

		auto addr = utility::keyPath() ;
		m = QString( "-f %1" ).arg( addr ) ;

		utility::keySend( addr,m_ui->lineEditKey->text() ) ;

	}else if( keyType == keyDialog::keyKeyFile ){

		if( utility::pluginKey( m_secrets.parent(),&m_key,"hmac" ) ){

			return this->enableAll() ;
		}

		auto addr = utility::keyPath() ;
		m = QString( "-f %1" ).arg( addr ) ;

		utility::keySend( addr,m_key ) ;

	}else if( keyType == keyDialog::keyfile ){

		auto e = m_ui->lineEditKey->text().replace( "\"","\"\"\"" ) ;
		m = "-f \"" + utility::resolvePath( e ) + "\"" ;

	}else if( keyType == keyDialog::plugin ){

		auto r = m_ui->lineEditKey->text() ;

		if( r == "hmac" || r == "gpg" || r == "keykeyfile" ){

			if( utility::pluginKey( m_secrets.parent(),&m_key,r ) ){

				return this->enableAll() ;
			}
		}
		if( m_key.isEmpty() ){

			m = "-G " + m_ui->lineEditKey->text().replace( "\"","\"\"\"" ) ;
		}else{

			auto addr = utility::keyPath() ;
			m = QString( "-f %1" ).arg( addr ) ;

			utility::keySend( addr,m_key ) ;
		}

	}else if( keyType == keyDialog::tcryptKeys ){

		QEventLoop wait ;

		bool cancelled = false ;

		tcrypt::instance( this,false,[ this,&wait ]( const QString& key,
				  const QStringList& keyFiles ){

			m_key = key.toLatin1() ;
			m_keyFiles = keyFiles ;

			wait.exit() ;

		},[ this,&wait,&cancelled ](){

			cancelled = true ;
			m_key.clear() ;
			m_keyFiles.clear() ;

			wait.exit() ;
		} ) ;

		wait.exec() ;

		if( cancelled ){

			return this->enableAll() ;
		}

		auto addr = utility::keyPath() ;
		m = QString( "-f %1 " ).arg( addr ) ;

		utility::keySend( addr,m_key ) ;
	}else{
		qDebug() << "ERROR: Uncaught condition" ;
	}

	auto volume = m_path ;
	volume.replace( "\"","\"\"\"" ) ;

	QString exe = zuluMountPath ;

	if( m_ui->checkBoxShareMountPoint->isChecked() ){

		exe += " -M -m -d \"" + volume + "\"" ;
	}else{
		exe += " -m -d \"" + volume + "\"" ;
	}

	if( m_ui->checkBoxOpenReadOnly->isChecked() ){

		exe += " -e ro" ;
	}else{
		exe += "  e rw" ;
	}

	auto mountPoint = m_ui->lineEditMountPoint->text() ;
	mountPoint.replace( "\"","\"\"\"" ) ;

	exe += " -z \"" + mountPoint + "\"" ;

	if( !m_options.isEmpty() ){

		exe += " -Y " + m_options ;
	}

	if( !m_deviceOffSet.isEmpty() ){

		exe += m_deviceOffSet ;
	}

	if( !m_keyFiles.isEmpty() ){

		for( const auto& it : m_keyFiles ){

			auto e = it ;
			e.replace( "\"","\"\"\"" ) ;

			exe += " -F \"" + e + "\"" ;
		}
	}

	if( m_veraCryptVolume ){

		if( m_veraCryptPIMValue > 0 ){

			exe += " -t vcrypt." + QString::number( m_veraCryptPIMValue ) + " " + m ;
		}else{
			exe += " -t vcrypt " + m ;
		}
	}else{
		exe += " " + m ;
	}

	m_veraCryptWarning.show( m_veraCryptVolume ) ;

	m_working = true ;

	auto s = utility::Task::run( utility::appendUserUID( exe ) ).await() ;

	m_working = false ;

	m_veraCryptWarning.stopTimer() ;

	if( s.success() ){

		m_success( utility::mountPath( mountPoint ) ) ;

		this->HideUI() ;
	}else{
		m_veraCryptWarning.hide() ;

		auto keyType = m_ui->cbKeyType->currentIndex() ;

		int r = s.exitCode() ;

		if( r == 12 && keyType == keyDialog::plugin ){

			/*
			 * A user cancelled the plugin
			 */

			this->enableAll() ;
		}else{
			if( this->errorNotFound( r ) ){

				QString z = s.output() ;

				z.replace( tr( "ERROR: " ),"" ) ;

				DialogMsg msg( this ) ;

				msg.ShowUIOK( tr( "ERROR" ),z ) ;
			}

			if( m_ui->cbKeyType->currentIndex() == keyDialog::Key ){

				m_ui->lineEditKey->clear() ;
			}

			this->enableAll() ;

			m_ui->lineEditKey->setFocus() ;

			if( keyType == keyDialog::keyKeyFile ){

				m_ui->cbKeyType->setCurrentIndex( 0 ) ;

				this->key() ;
			}
		}
	}
}

void keyDialog::cbActicated( int e )
{
	m_ui->pbkeyOption->setVisible( e != keyDialog::Key ) ;
	m_ui->checkBoxVisibleKey->setVisible( e == keyDialog::Key ) ;

	switch( e ){

		case keyDialog::Key        : return this->key() ;
		case keyDialog::keyfile    : return this->keyFile() ;
		case keyDialog::keyKeyFile : return this->keyAndKeyFile() ;
		case keyDialog::plugin     : return this->plugIn() ;
		case keyDialog::tcryptKeys : return this->tcryptGui() ;
	}
}

void keyDialog::keyAndKeyFile()
{
	m_ui->pbkeyOption->setIcon( QIcon( ":/module.png" ) ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
	m_ui->label->setText( tr( "Plugin name" ) ) ;
	m_ui->pbkeyOption->setEnabled( false ) ;
	m_ui->lineEditKey->setEnabled( false ) ;
	m_ui->lineEditKey->setText( tr( "Key+KeyFile" ) ) ;
}

void keyDialog::plugIn()
{
	m_ui->pbkeyOption->setIcon( QIcon( ":/module.png" ) ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
	m_ui->label->setText( tr( "Plugin name" ) ) ;
	m_ui->pbkeyOption->setEnabled( true ) ;
	m_ui->lineEditKey->setEnabled( false ) ;
	m_ui->lineEditKey->setText( INTERNAL_WALLET ) ;
}

void keyDialog::key()
{
	m_ui->pbkeyOption->setIcon( QIcon( ":/passphrase.png" ) ) ;
	m_ui->pbkeyOption->setEnabled( false ) ;
	m_ui->label->setText( tr( "Key" ) ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;
	m_ui->lineEditKey->clear() ;
	m_ui->lineEditKey->setEnabled( true ) ;
}

void keyDialog::keyFile()
{
	m_ui->pbkeyOption->setIcon( QIcon( ":/keyfile.png" ) ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
	m_ui->label->setText( tr( "Keyfile path" ) ) ;
	m_ui->pbkeyOption->setEnabled( true ) ;
	m_ui->lineEditKey->clear() ;
	m_ui->lineEditKey->setEnabled( true ) ;
}

void keyDialog::tcryptGui()
{
	m_ui->pbkeyOption->setIcon( QIcon( ":/module.png" ) ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
	m_ui->label->setText( tr( "Plugin name" ) ) ;
	m_ui->pbkeyOption->setEnabled( false ) ;
	m_ui->lineEditKey->setEnabled( false ) ;
	m_ui->lineEditKey->setText( tr( "TrueCrypt/VeraCrypt Keys" ) ) ;
}

void keyDialog::pbCancel()
{
	this->HideUI() ;
	m_cancel() ;
}

void keyDialog::ShowUI()
{
	this->show() ;
}

void keyDialog::HideUI()
{
	if( !m_working ){

		this->hide() ;
		this->deleteLater() ;
	}
}

keyDialog::~keyDialog()
{
	m_menu->deleteLater() ;
	delete m_ui ;
}

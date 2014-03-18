/*
 *
 *  Copyright ( c ) 2011
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "password_dialog.h"
#include "zulucrypt.h"
#include "lxqt_wallet/frontend/lxqt_wallet.h"

#include <QMenu>
#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>

#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QStringList>
#include <QCloseEvent>
#include <QTableWidget>
#include <stdlib.h>

#include "ui_password.h"
#include "openvolume.h"
#include "task.h"
#include "utility.h"
#include "dialogmsg.h"
#include "plugin_path.h"
#include "tablewidget.h"
#include "../zuluCrypt-cli/constants.h"
#include "socketsendkey.h"

#include "utility.h"

#define KWALLET         "kde wallet"
#define INTERNAL_WALLET "internal wallet"
#define GNOME_WALLET    "gnome wallet"

/*
 * this ugly global variable is defined in zulucrypt.cpp to prevent multiple prompts when opening multiple volumes
 */
static QString _internalPassWord ;

passwordDialog::passwordDialog( QTableWidget * table,const QString& folderOpener,QWidget * parent ) : QDialog( parent )
{
	m_ui = new Ui::PasswordDialog() ;
	m_ui->setupUi( this ) ;

	m_parent = parent ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;
	this->setDefaultOpenMode() ;

	m_ui->PushButtonMountPointPath->setIcon( QIcon( QString( ":/folder.png" ) ) ) ;

	m_isWindowClosable = true ;

	m_open_with_path = false ;

	m_table = table ;

	m_folderOpener = folderOpener ;

	m_pluginMenu = new QMenu( this ) ;
	m_pluginMenu->setFont( this->font() ) ;

	m_ui->pushButtonPlugin->setIcon( QIcon( QString( ":/module.png" ) ) ) ;

	connect( m_ui->PushButtonCancel,SIGNAL( clicked() ),this,SLOT( HideUI() ) ) ;
	connect( m_ui->PushButtonOpen,SIGNAL( clicked() ),this,SLOT( buttonOpenClicked() ) ) ;
	connect( m_ui->PushButtonMountPointPath,SIGNAL( clicked() ),this,SLOT( mount_point() ) ) ;
	connect( m_ui->PushButtonVolumePath,SIGNAL( clicked() ),this,SLOT( file_path() ) ) ;
	connect( m_ui->pushButtonPassPhraseFromFile,SIGNAL( clicked() ),this,SLOT( clickedPassPhraseFromFileButton() ) ) ;
	connect( m_ui->OpenVolumePath,SIGNAL( textChanged( QString ) ),this,SLOT( mountPointPath( QString ) ) ) ;
	connect( m_ui->checkBoxReadOnly,SIGNAL( stateChanged( int ) ),this,SLOT( cbStateChanged( int ) ) ) ;
	connect( m_ui->pushButtonPlugin,SIGNAL( clicked() ),this,SLOT( pbPlugin() ) ) ;
	connect( m_ui->pbKeyOption,SIGNAL( clicked() ),this,SLOT( pbKeyOption() ) ) ;
	connect( m_ui->cbKeyType,SIGNAL( currentIndexChanged( int ) ),this,SLOT( cbActicated( int ) ) ) ;

	m_ui->PushButtonMountPointPath->setVisible( false ) ;
	m_ui->pushButtonPassPhraseFromFile->setVisible( false ) ;
	m_ui->pushButtonPlugin->setVisible( false ) ;
}

void passwordDialog::pbPlugin()
{
	QStringList list ;

	// ZULUCRYPTpluginPath is set at config time and it equals $prefix/lib(64)/zuluCrypt

	QDir dir( QString( ZULUCRYPTpluginPath ) ) ;

	if( dir.exists() ){
		list = dir.entryList() ;

		list.removeOne( QString( "zuluCrypt-testKey" ) ) ;
		list.removeOne( QString( "." ) ) ;
		list.removeOne( QString( ".." ) ) ;
		list.removeOne( QString( "keyring" ) ) ;
		list.removeOne( QString( "kwallet" ) ) ;

		list.insert( 0,tr( INTERNAL_WALLET ) ) ;

		if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::kwalletBackEnd ) ){
			list.insert( 1,tr( KWALLET ) ) ;
		}

		if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::secretServiceBackEnd ) ){
			list.insert( 2,tr( GNOME_WALLET ) ) ;
		}
	}else{
		list.append( tr( INTERNAL_WALLET ) ) ;

		if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::kwalletBackEnd ) ){
			list.append( tr( KWALLET ) ) ;
		}
		if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::secretServiceBackEnd ) ){
			list.append( tr( GNOME_WALLET ) ) ;
		}
	}

	m_pluginMenu->clear() ;

	int j = list.size()  ;

	if( j == 0 ){
		DialogMsg msg( this ) ;
		return	msg.ShowUIOK( tr( "ERROR" ),tr( "could not find any plugin installed" ) ) ;
	}

	for( int i = 0 ; i < j ; i++ ){
		m_pluginMenu->addAction( list.at( i ) ) ;
	}

	m_pluginMenu->addSeparator() ;

	m_pluginMenu->addAction( tr( "cancel" ) ) ;

	connect( m_pluginMenu,SIGNAL( triggered( QAction * ) ),this,SLOT( pbPluginEntryClicked( QAction * ) ) ) ;

	m_pluginMenu->exec( QCursor::pos() ) ;
}

void passwordDialog::pbPluginEntryClicked( QAction * e )
{
	if( e->text() != tr( "cancel" ) ){
		m_ui->PassPhraseField->setText( e->text() ) ;
	}
}

void passwordDialog::cbStateChanged( int state )
{
	m_ui->checkBoxReadOnly->setEnabled( false ) ;
	m_ui->checkBoxReadOnly->setChecked( utility::setOpenVolumeReadOnly( this,state == Qt::Checked,QString( "zuluCrypt-gui" ) ) ) ;
	m_ui->checkBoxReadOnly->setEnabled( true ) ;
}

void passwordDialog::setDefaultOpenMode()
{
	m_ui->checkBoxReadOnly->setChecked( utility::getOpenVolumeReadOnlyOption( QString( "zuluCrypt-gui" ) ) ) ;
}

void passwordDialog::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	if( m_isWindowClosable ){
		this->HideUI() ;
	}
}

void passwordDialog::ShowUI( const QString& volumePath,const QString& mount_point )
{
	m_point = mount_point.split( QString( "/" ) ).last() ;
	if( m_point.isEmpty() ){
		m_point = volumePath.split( QString( "/" ) ).last() ;
	}

	m_open_with_path = true ;
	this->passphraseOption() ;
	m_ui->OpenVolumePath->setText( volumePath ) ;
	m_ui->OpenVolumePath->setEnabled( false ) ;
	m_ui->PushButtonVolumePath->setEnabled( false ) ;
	m_ui->MountPointPath->setText( m_point ) ;
	m_ui->PassPhraseField->setFocus() ;
	QString vp = volumePath.mid( 0,5 ) ;
	if( vp == QString( "/dev/" ) || vp == QString( "UUID=" ) ){
		m_ui->PushButtonVolumePath->setIcon( QIcon( QString( ":/partition.png" ) ) ) ;
	}else{
		m_ui->PushButtonVolumePath->setIcon( QIcon( QString( ":/file.png" ) ) ) ;
	}
	this->show() ;
}

void passwordDialog::ShowUI()
{
	this->passphraseOption() ;
	m_ui->OpenVolumePath->setFocus() ;
	m_ui->PushButtonVolumePath->setIcon( QIcon( QString( ":/file.png" ) ) ) ;
	this->show() ;
}

void passwordDialog::mountPointPath( QString path )
{
	QString p = m_ui->MountPointPath->text() ;

	int x = path.lastIndexOf( QString( "/" ) ) ;
	if( x == -1 ){
		m_ui->MountPointPath->setText( path ) ;
	}else{
		m_ui->MountPointPath->setText( path.mid( x + 1 ) ) ;
	}
}

void passwordDialog::cbActicated( int e )
{
	switch( e ){
		case passwordDialog::key     : return this->passphraseOption() ;
		case passwordDialog::keyfile : return this->passphraseFromFileOption() ;
		case passwordDialog::plugin  : return this->pluginOption() ;
	}
}

void passwordDialog::pbKeyOption()
{
	if( m_ui->cbKeyType->currentIndex() == passwordDialog::plugin ){
		this->pbPlugin() ;
	}else{
		this->clickedPassPhraseFromFileButton() ;
	}
}

void passwordDialog::pluginOption()
{
	m_ui->pushButtonPassPhraseFromFile->setToolTip( tr( "choose a module from the file system" ) ) ;
	m_ui->PassPhraseField->setToolTip( tr( "enter a module name to use to get passphrase" ) ) ;
	m_ui->PassPhraseField->setEchoMode( QLineEdit::Normal ) ;
	m_ui->pushButtonPassPhraseFromFile->setEnabled( false ) ;
	m_ui->labelPassphrase->setText( tr( "plugin name" ) ) ;
	m_ui->pushButtonPassPhraseFromFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) ) ;
	m_ui->pbKeyOption->setIcon( QIcon( QString( ":/module.png" ) ) ) ;
	m_ui->pushButtonPlugin->setEnabled( true ) ;
	m_ui->pushButtonPlugin->setToolTip( tr( "select a key module" ) ) ;
	m_ui->pbKeyOption->setEnabled( true ) ;
	m_ui->PassPhraseField->setEnabled( false ) ;
	m_ui->PassPhraseField->setText( INTERNAL_WALLET ) ;
}

void passwordDialog::passphraseOption()
{
	m_ui->PassPhraseField->setToolTip( tr( "enter a key" ) ) ;
	m_ui->PassPhraseField->setEchoMode( QLineEdit::Password ) ;
	m_ui->PassPhraseField->clear() ;
	m_ui->pushButtonPassPhraseFromFile->setEnabled( false ) ;
	m_ui->labelPassphrase->setText( tr( "key" ) ) ;
	m_ui->pushButtonPassPhraseFromFile->setIcon( QIcon( QString( ":/passphrase.png" ) ) ) ;
	m_ui->pushButtonPlugin->setEnabled( false ) ;
	m_ui->pbKeyOption->setIcon( QIcon( QString( "" ) ) ) ;
	m_ui->pbKeyOption->setEnabled( false ) ;
	m_ui->PassPhraseField->setEnabled( true ) ;
}

void passwordDialog::passphraseFromFileOption()
{
	m_ui->pushButtonPassPhraseFromFile->setToolTip( tr( "choose a key file from the file system" ) ) ;
	m_ui->PassPhraseField->setToolTip( tr( "enter a path to a keyfile location" ) ) ;
	m_ui->PassPhraseField->setEchoMode( QLineEdit::Normal ) ;
	m_ui->PassPhraseField->clear() ;
	m_ui->pushButtonPassPhraseFromFile->setEnabled( true ) ;
	m_ui->labelPassphrase->setText( tr( "keyfile path" ) ) ;
	m_ui->pushButtonPassPhraseFromFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) ) ;
	m_ui->pushButtonPlugin->setEnabled( false ) ;
	m_ui->pbKeyOption->setIcon( QIcon( QString( ":/keyfile.png" ) ) ) ;
	m_ui->pushButtonPlugin->setEnabled( true ) ;
	m_ui->pbKeyOption->setEnabled( true ) ;
	m_ui->PassPhraseField->setEnabled( false ) ;
}

void passwordDialog::clickedPassPhraseFromFileButton()
{
	QString msg ;
	if( m_ui->cbKeyType->currentIndex() == passwordDialog::keyfile ){
		msg = tr( "Select a keyfile" ) ;
	}else{
		msg = tr( "Select a key module" ) ;
	}

	QString Z = QFileDialog::getOpenFileName( this,msg,QDir::homePath(),0 ) ;
	if( !Z.isEmpty() ){
		m_ui->PassPhraseField->setText( Z ) ;
	}
}

void passwordDialog::clickedPartitionOption( QString dev )
{
	QString m_point = QDir::homePath() + QString( "/" ) + dev.split( "/" ).last() ;
	this->ShowUI( dev,m_point ) ;
}

void passwordDialog::mount_point( void )
{
	QString p = tr( "Select Path to mount point folder" ) ;
	QString Z = QFileDialog::getExistingDirectory( this,p,QDir::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( !Z.isEmpty() ){
		Z = Z + QString( "/" ) + m_ui->OpenVolumePath->text().split( "/" ).last() ;
		m_ui->MountPointPath->setText( Z ) ;
	}

	if( m_ui->MountPointPath->text().isEmpty() ){
		m_ui->MountPointPath->setFocus() ;
	}else if( m_ui->PassPhraseField->text().isEmpty() ){
		m_ui->PassPhraseField->setFocus() ;
	}
}

void passwordDialog::file_path( void )
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "Select encrypted volume" ),QDir::homePath(),0 ) ;
	m_ui->OpenVolumePath->setText( Z ) ;
	if( !Z.isEmpty() ){
		m_ui->MountPointPath->setText( Z.split( QString( "/" ) ).last() ) ;
	}
}

void passwordDialog::HideUI()
{
	this->hide() ;
	emit HideUISignal() ;
}

void passwordDialog::getPassWord( QString password )
{
	_internalPassWord = password ;
}

void passwordDialog::getPassWordFromWallet( QString key )
{
	if( key.isEmpty() ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "the volume does not appear to have an entry in the wallet" ) ) ;
		this->enableAll() ;
	}else{
		m_key = key ;
		this->openVolume() ;
	}

	m_wallet->deleteLater() ;
}

void passwordDialog::walletIsOpen( bool opened )
{
	if( opened ){
		Task * t = new Task( m_wallet,m_ui->OpenVolumePath->text() ) ;
		connect( t,SIGNAL( finished( QString ) ),this,SLOT( getPassWordFromWallet( QString ) ) ) ;
		t->start( Task::getKey ) ;
	}else{
		_internalPassWord.clear() ;
		this->enableAll() ;
		m_wallet->deleteLater() ;
	}
}

void passwordDialog::buttonOpenClicked( void )
{
	this->disableAll() ;
	if( m_ui->cbKeyType->currentIndex() == passwordDialog::plugin ){
		QString wallet = m_ui->PassPhraseField->text() ;
		if( wallet == tr( KWALLET ) ){
			m_wallet = LxQt::Wallet::getWalletBackend( LxQt::Wallet::kwalletBackEnd ) ;
			m_wallet->setInterfaceObject( this ) ;
			m_wallet->open( m_wallet->localDefaultWalletName(),utility::applicationName() ) ;
		}else if( wallet == tr( INTERNAL_WALLET ) ){
			QString walletName = utility::walletName() ;
			QString appName    = utility::applicationName() ;
			if( LxQt::Wallet::walletExists( LxQt::Wallet::internalBackEnd,walletName,appName ) ){
				m_wallet = LxQt::Wallet::getWalletBackend( LxQt::Wallet::internalBackEnd ) ;
				m_wallet->setInterfaceObject( this ) ;
				QObject * obj = m_wallet->qObject() ;
				connect( obj,SIGNAL( getPassWord( QString ) ),this,SLOT( getPassWord( QString ) ) ) ;
				m_wallet->open( walletName,appName,_internalPassWord ) ;
			}else{
				DialogMsg msg( this ) ;
				msg.ShowUIOK( tr( "ERROR!" ),tr( "internal wallet is not configured" ) ) ;
				this->enableAll() ;
			}
		}else if( wallet == tr( GNOME_WALLET ) ){
			m_wallet = LxQt::Wallet::getWalletBackend( LxQt::Wallet::secretServiceBackEnd ) ;
			m_wallet->setInterfaceObject( this ) ;
			m_wallet->open( utility::walletName(),utility::applicationName() ) ;
		}else{
			m_key = wallet ;
			this->openVolume() ;
		}
	}else{
		m_key = m_ui->PassPhraseField->text() ;
		this->openVolume() ;
	}
}

void passwordDialog::openVolume()
{
	m_device = utility::resolvePath( m_ui->OpenVolumePath->text() ) ;

	m_point = m_ui->MountPointPath->text() ;
	if( m_point.isEmpty() || m_device.isEmpty() ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR!" ),tr( "atleast one required field is empty" ) ) ;
		return this->enableAll() ;
	}

	if( m_point.contains( QString( "/" ) ) ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "\"/\" character is not allowed in mount name field" ) ) ;
		m_ui->OpenVolumePath->setFocus() ;
		return this->enableAll() ;
	}

	QString mode ;

	if( m_ui->checkBoxReadOnly->isChecked() ){
		mode = QString( "ro" ) ;
	}else{
		mode = QString( "rw" ) ;
	}

	QString passtype ;

	QString keyPath ;

	int keySource = m_ui->cbKeyType->currentIndex() ;

	if( keySource == passwordDialog::keyfile ){
		if( m_key.isEmpty() ){
			DialogMsg msg( this ) ;
			msg.ShowUIOK( tr( "ERROR!" ),tr( "atleast one required field is empty" ) ) ;
			return this->enableAll() ;
		}else{
			passtype = QString( "-f" ) ;
			keyPath = utility::resolvePath( m_key ) ;
		}
	}else if( keySource == passwordDialog::key ){
		passtype = QString( "-f" ) ;
		keyPath = socketSendKey::getSocketPath() ;
		this->sendKey( keyPath ) ;
	}else if( keySource == passwordDialog::plugin ){
		if( m_key.isEmpty() ){
			DialogMsg msg( this ) ;
			msg.ShowUIOK( tr( "ERROR!" ),tr( "atleast one required field is empty" ) ) ;
			return this->enableAll() ;
		}else{
			QString r = m_ui->PassPhraseField->text() ;
			if( r == tr( KWALLET ) || r == tr( INTERNAL_WALLET ) || r == tr( GNOME_WALLET ) ){
				passtype = QString( "-f" ) ;
				keyPath = socketSendKey::getSocketPath() ;
				this->sendKey( keyPath ) ;
			}else{
				passtype = QString( "-G" ) ;
				keyPath  = r ;
			}
		}
	}

	QString a = QString( ZULUCRYPTzuluCrypt ) ;
	QString b = m_device ;
	b.replace( "\"","\"\"\"" ) ;
	QString c = m_point ;
	c.replace( "\"","\"\"\"" ) ;
	QString d = mode ;
	QString e = passtype ;
	QString f = keyPath ;

	QString exe = QString( "%1 -o -d \"%2\" -m \"%3\" -e %4 %5 \"%6\"" ).arg( a ).arg( b ).arg( c ).arg( d ).arg( e ).arg( f ) ;

	this->disableAll() ;

	Task * t = new Task( exe ) ;
	connect( t,SIGNAL( finished( int,QString ) ),this,SLOT( taskFinished( int,QString ) ) ) ;
	m_isWindowClosable = false ;
	t->start() ;
}

void passwordDialog::sendKey( const QString& sockpath )
{
	socketSendKey * sk = new socketSendKey( this,sockpath,m_key.toLatin1() ) ;
	sk->sendKey() ;
}

void passwordDialog::disableAll()
{
	m_ui->pushButtonPlugin->setEnabled( false ) ;
	m_ui->checkBoxReadOnly->setEnabled( false ) ;
	m_ui->labelMoutPointPath->setEnabled( false ) ;
	m_ui->labelPassphrase->setEnabled( false ) ;
	m_ui->labelVolumePath->setEnabled( false ) ;
	m_ui->MountPointPath->setEnabled( false ) ;
	m_ui->OpenVolumePath->setEnabled( false ) ;
	m_ui->PassPhraseField->setEnabled( false ) ;
	m_ui->PushButtonCancel->setEnabled( false ) ;
	m_ui->PushButtonMountPointPath->setEnabled( false ) ;
	m_ui->PushButtonOpen->setEnabled( false ) ;
	m_ui->pushButtonPassPhraseFromFile->setEnabled( false ) ;
	m_ui->PushButtonVolumePath->setEnabled( false ) ;
	m_ui->pbKeyOption->setEnabled( false ) ;
	m_ui->cbKeyType->setEnabled( false ) ;
}

void passwordDialog::enableAll()
{
	m_ui->pushButtonPlugin->setEnabled( true ) ;
	m_ui->checkBoxReadOnly->setEnabled( true ) ;
	m_ui->labelMoutPointPath->setEnabled( true ) ;
	m_ui->labelPassphrase->setEnabled( true ) ;
	m_ui->labelVolumePath->setEnabled( true ) ;
	m_ui->MountPointPath->setEnabled( true ) ;
	m_ui->OpenVolumePath->setEnabled( true ) ;
	m_ui->PassPhraseField->setEnabled( true ) ;
	m_ui->PushButtonCancel->setEnabled( true ) ;
	m_ui->PushButtonMountPointPath->setEnabled( true ) ;
	m_ui->PushButtonOpen->setEnabled( true ) ;
	m_ui->pushButtonPassPhraseFromFile->setEnabled( true ) ;
	m_ui->PushButtonVolumePath->setEnabled( true ) ;
	m_ui->cbKeyType->setEnabled( true ) ;

	if( m_open_with_path ){
		m_ui->OpenVolumePath->setEnabled( false ) ;
		m_ui->PushButtonVolumePath->setEnabled( false ) ;
	}

	if( m_ui->cbKeyType->currentIndex() == passwordDialog::key ){
		m_ui->pushButtonPassPhraseFromFile->setEnabled( false ) ;
		m_ui->pushButtonPlugin->setEnabled( false ) ;
		m_ui->PassPhraseField->setEnabled( true ) ;
	}else{
		m_ui->pbKeyOption->setEnabled( true ) ;
		m_ui->PassPhraseField->setEnabled( false ) ;
	}

	if( m_ui->cbKeyType->currentIndex() == passwordDialog::keyfile ){
		m_ui->pushButtonPlugin->setEnabled( false ) ;
	}
}

void passwordDialog::fileManagerOpenStatus( int exitCode,int exitStatus,int startError )
{
	Q_UNUSED( startError ) ;
	if( exitCode != 0 || exitStatus != 0 ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "warning" ),tr( "could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_folderOpener ) ) ;
	}
}

void passwordDialog::success( const QString& output )
{
	if( utility::mapperPathExists( m_device ) ){
		this->complete( output ) ;
		Task * t = new Task( m_folderOpener,utility::mountPath( m_point ) ) ;
		connect( t,SIGNAL( errorStatus( int,int,int ) ),this,SLOT( fileManagerOpenStatus( int,int,int ) ) ) ;
		t->start( Task::openMountPoint ) ;
	}else{
		/*
		 * we arrive here if zuluCrypt-cli reports a volume was opened but it was not.
		 * most likely reason for getting here is if it crashed.
		 */
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR!" ),tr( "An error has occured and the volume could not be opened" ) ) ;
		this->HideUI() ;
	}
}

void passwordDialog::complete( QString output )
{
	QStringList list ;
	list.append( utility::resolvePath( m_ui->OpenVolumePath->text() ) ) ;
	list.append( utility::mountPath( m_point ) ) ;

	if( output.contains( QString( "luks" ) ) ){
		list.append( QString( "luks" ) ) ;
	}else if( output.contains( QString( "plain" ) ) ){
		list.append( QString( "plain" ) ) ;
	}else if( output.contains( QString( "tcrypt" ) ) ){
		list.append( QString( "tcrypt" ) ) ;
	}else{
		list.append( QString( "Nil" ) ) ;
	}
	tablewidget::addRowToTable( m_table,list ) ;

	this->HideUI() ;
}

void passwordDialog::taskFinished( int status,QString output )
{
	m_isWindowClosable = true ;
	DialogMsg msg( this ) ;
	switch ( status ){
		case 0 : return success( output ) ;
		case 1 : msg.ShowUIOK( tr( "ERROR!" ),tr( "failed to mount ntfs/exfat file system using ntfs-3g,is ntfs-3g/exfat package installed?" ) ) ; break ;
		case 2 : msg.ShowUIOK( tr( "ERROR!" ),tr( "there seem to be an open volume accociated with given address" ) ) ;				break ;
		case 3 : msg.ShowUIOK( tr( "ERROR!" ),tr( "no file or device exist on given path" ) ) ; 						break ;
		case 4 : msg.ShowUIOK( tr( "ERROR!" ),tr( "volume could not be opened with the presented key" ) ) ;					break ;
		case 5 : msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to mount the device with given options" ) ) ;				break ;
		case 6 : msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open device in read write mode or device does not exist" ) ) ;	break ;
		case 7 : msg.ShowUIOK( tr( "ERROR!" ),tr( "only root user can perform this operation" ) ) ;						break ;
		case 8 : msg.ShowUIOK( tr( "ERROR!" ),tr( "-O and -m options can not be used together" ) ) ;						break ;
		case 9 : msg.ShowUIOK( tr( "ERROR!" ),tr( "could not create mount point, invalid path or path already taken" ) ) ;			break ;
		case 10: msg.ShowUIOK( tr( "ERROR!" ),tr( "shared mount point path aleady taken" ) ) ;							break ;
		case 11: msg.ShowUIOK( tr( "ERROR!" ),tr( "there seem to be an opened mapper associated with the device" ) ) ;				break ;
		case 12: msg.ShowUIOK( tr( "ERROR!" ),tr( "could not get a passphrase from the module" ) ) ;						break ;
		case 13: msg.ShowUIOK( tr( "ERROR!" ),tr( "could not get passphrase in silent mode" ) ) ;						break ;
		case 14: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient memory to hold passphrase" ) ) ;							break ;
		case 15: msg.ShowUIOK( tr( "ERROR!" ),tr( "one or more required argument(s) for this operation is missing" ) ) ;				break ;
		case 16: msg.ShowUIOK( tr( "ERROR!" ),tr( "invalid path to key file" ) ) ;								break ;
		case 17: msg.ShowUIOK( tr( "ERROR!" ),tr( "could not get enought memory to hold the key file" ) ) ;					break ;
		case 18: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open key file for reading" ) ) ;					break ;
		case 19: msg.ShowUIOK( tr( "ERROR!" ),tr( "could not get a passphrase through a local socket" ) ) ;					break ;
		case 20: msg.ShowUIOK( tr( "ERROR!" ),tr( "failed to mount a filesystem:invalid/unsupported mount option or unsupported file system encountered" ) ) ;	break ;
		case 21: msg.ShowUIOK( tr( "ERROR!" ),tr( "could not create a lock on /etc/mtab" ) ) ;							break ;
		case 22: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open a system volume\n" ) ) ;					break ;
		case 113:msg.ShowUIOK( tr( "ERROR!" ),tr( "a non supported device encountered,device is missing or permission denied\n\
Possible reasons for getting the error are:\n1.Device path is invalid.\n2.The device has LVM or MDRAID signature" ) ) ;					break ;
		default: msg.ShowUIOK( tr( "ERROR!" ),tr( "unrecognized ERROR with status number %1 encountered" ).arg( status ) ) ;
	}

	this->enableAll() ;

	if( status == 4 ){
		if( m_ui->cbKeyType->currentIndex() == passwordDialog::key ){
			m_ui->PassPhraseField->clear() ;
			m_ui->PassPhraseField->setFocus() ;
		}
	}
}

passwordDialog::~passwordDialog()
{
	m_pluginMenu->deleteLater() ;
	delete m_ui ;
}


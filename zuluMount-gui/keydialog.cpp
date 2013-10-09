/*
 *
 *  Copyright (c) 2012
 *  name : mhogo mchungu
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

#include "keydialog.h"
#include "ui_keydialog.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QDir>
#include <QTableWidget>

#include "../zuluCrypt-gui/dialogmsg.h"
#include "task.h"
#include "../zuluCrypt-cli/constants.h"
#include "plugin_path.h"
#include "../zuluCrypt-gui/socketsendkey.h"
#include "../zuluCrypt-gui/utility.h"
#include "../zuluCrypt-gui/lxqt_wallet/frontend/lxqt_wallet.h"

#define KWALLET         "kde wallet"
#define INTERNAL_WALLET "internal wallet"
#define GNOME_WALLET    "gnome wallet"

/*
 * this ugly global variable is defined in zulucrypt.cpp to prevent multiple prompts when opening multiple volumes
 */
static QString _internalPassWord ;

keyDialog::keyDialog( QWidget * parent,QTableWidget * table,const QString& path,
		      const QString& type,const QString& folderOpener,bool autoOpenFolderOnMount ) :
	QDialog( parent ),m_ui(new Ui::keyDialog)
{
	m_ui->setupUi( this ) ;
	m_ui->checkBoxShareMountPoint->setToolTip( utility::shareMountPointToolTip() ) ;
	m_table = table ;
	m_path = path ;
	m_working = false ;
	m_folderOpener = folderOpener ;

	m_autoOpenFolderOnMount = autoOpenFolderOnMount ;

	QString msg ;
	if( type == QString( "crypto_LUKS" ) ){
		msg = tr( "unlock and mount a luks volume in \"%1\"").arg( m_path ) ;
	}else{
		msg = tr( "unlock and mount an encrypted volume in \"%1\"").arg( m_path ) ;
	}
	this->setWindowTitle( msg ) ;

	m_ui->lineEditMountPoint->setText( path ) ;
	m_ui->pbkeyFile->setVisible( false ) ;
	m_ui->pbOpenMountPoint->setIcon( QIcon( QString( ":/folder.png" ) ) ) ;

	m_menu = new QMenu( this ) ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;

	m_ui->lineEditKey->setFocus() ;

	m_ui->checkBoxOpenReadOnly->setChecked( utility::getOpenVolumeReadOnlyOption( QString( "zuluMount-gui" ) ) ) ;

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbkeyOption,SIGNAL( clicked() ),this,SLOT( pbkeyOption() ) ) ;
	connect( m_ui->rbKey,SIGNAL( toggled( bool ) ),this,SLOT( rbKey( bool ) ) ) ;
	connect( m_ui->rbKeyFile,SIGNAL( toggled( bool ) ),this,SLOT( rbKeyFile( bool ) ) ) ;
	connect( m_ui->rbPlugIn,SIGNAL( toggled( bool ) ),this,SLOT( rbPlugIn( bool ) ) ) ;
	connect( m_ui->lineEditKey,SIGNAL( textChanged( QString ) ),this,SLOT( keyTextChanged( QString ) ) ) ;
	connect( m_ui->pbOpenMountPoint,SIGNAL( clicked() ),this,SLOT( pbMountPointPath() ) ) ;
	connect( m_ui->checkBoxOpenReadOnly,SIGNAL( stateChanged( int ) ),this,SLOT( cbMountReadOnlyStateChanged( int ) ) ) ;
	m_ui->rbKey->setChecked( true ) ;

	m_ui->pbOpenMountPoint->setVisible( false ) ;

	m_point = m_path.split( "/" ).last() ;
	m_ui->lineEditMountPoint->setText( m_point ) ;
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

void keyDialog::keyTextChanged( QString txt )
{
	if( m_ui->rbPlugIn->isChecked() ){
		if( txt.contains( QString( "/") ) ){
			m_ui->label->setText( tr( "plugin path" ) ) ;
		}else{
			m_ui->label->setText( tr( "plugin name" ) ) ;
		}
	}
}

void keyDialog::pbMountPointPath()
{
	QString msg = tr( "select a folder to create a mount point in" ) ;
	QString Z = QFileDialog::getExistingDirectory( this,msg,QDir::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( !Z.isEmpty() ){
		Z = Z + QString( "/" ) + m_ui->lineEditMountPoint->text().split( "/" ).last() ;
		m_ui->lineEditMountPoint->setText( Z ) ;
	}
}

void keyDialog::enableAll()
{
	m_ui->label_2->setEnabled( true ) ;
	m_ui->lineEditMountPoint->setEnabled( true ) ;
	m_ui->pbOpenMountPoint->setEnabled( true ) ;
	m_ui->pbCancel->setEnabled( true ) ;
	m_ui->pbOpen->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->rbKey->setEnabled( true ) ;
	m_ui->rbKeyFile->setEnabled( true ) ;
	m_ui->lineEditKey->setEnabled( true ) ;
	m_ui->rbPlugIn->setEnabled( true ) ;
	m_ui->pbkeyOption->setEnabled( true ) ;
	m_ui->checkBoxOpenReadOnly->setEnabled( true ) ;
	m_ui->checkBoxShareMountPoint->setEnabled( true ) ;
}

void keyDialog::disableAll()
{
	m_ui->pbkeyOption->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->lineEditMountPoint->setEnabled( false ) ;
	m_ui->pbOpenMountPoint->setEnabled( false ) ;
	m_ui->lineEditKey->setEnabled( false ) ;
	m_ui->pbCancel->setEnabled( false ) ;
	m_ui->pbOpen->setEnabled( false ) ;
	m_ui->pbkeyFile->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->rbKey->setEnabled( false ) ;
	m_ui->rbKeyFile->setEnabled( false ) ;
	m_ui->rbPlugIn->setEnabled( false ) ;
	m_ui->checkBoxOpenReadOnly->setEnabled( false ) ;
	m_ui->checkBoxShareMountPoint->setEnabled( false ) ;
}

void keyDialog::KeyFile()
{
	if( m_ui->rbKeyFile->isChecked() ){
		QString msg = tr( "select a file to be used as a keyfile" ) ;
		QString Z = QFileDialog::getOpenFileName( this,msg,QDir::homePath() ) ;

		if( !Z.isEmpty() ){
			m_ui->lineEditKey->setText( Z ) ;
		}
	}
}

void keyDialog::pbkeyOption()
{
	if( m_ui->rbPlugIn->isChecked() ){
		this->Plugin() ;
	}else if( m_ui->rbKeyFile->isChecked() ){
		this->KeyFile() ;
	}
}

void keyDialog::Plugin()
{
	QStringList list ;

	// ZULUCRYPTpluginPath is set at config time and it equals $prefix/lib(64)/zuluCrypt

	QDir dir( QString( ZULUCRYPTpluginPath ) ) ;

	if( dir.exists() ){
		list = dir.entryList() ;
	}

	list.removeOne( QString( "zuluCrypt-testKey" ) ) ;
	list.removeOne( QString( "." ) ) ;
	list.removeOne( QString( ".." ) ) ;
	list.removeOne( "keyring" ) ;
	list.removeOne( "kwallet" ) ;

	if( lxqt::Wallet::backEndIsSupported( lxqt::Wallet::secretServiceBackEnd ) ){
		list.prepend( tr( GNOME_WALLET ) ) ;
	}
	if( lxqt::Wallet::backEndIsSupported( lxqt::Wallet::kwalletBackEnd ) ){
		list.prepend( tr( KWALLET ) ) ;
	}

	list.prepend( tr( INTERNAL_WALLET ) ) ;

	m_menu->clear() ;

	int j = list.size()  ;

	if( j == 0 ){
		return ;
	}
	for( int i = 0 ; i < j ; i++ ){
		m_menu->addAction( list.at( i ) ) ;
	}
	m_menu->addSeparator() ;

	m_menu->addAction( tr( "cancel" ) ) ;

	connect( m_menu,SIGNAL( triggered( QAction * ) ),this,SLOT( pbPluginEntryClicked( QAction * ) ) ) ;

	m_menu->exec( QCursor::pos() ) ;
}

void keyDialog::pbPluginEntryClicked( QAction * e )
{
	if( e->text() != tr( "cancel" ) ){
		m_ui->lineEditKey->setText( e->text() ) ;
	}
}

void keyDialog::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}

void keyDialog::fileManagerOpenStatus( int exitCode, int exitStatus,int startError )
{
	Q_UNUSED( startError ) ;
	if( exitCode != 0 || exitStatus != 0 ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "warning" ),tr( "could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_folderOpener ) ) ;
	}
}

void keyDialog::slotMountComplete( int st,QString m )
{
	m_working = false ;
	if( st == 0 ){
		if( utility::mapperPathExists( m_path ) ) {
			/*
			 * The volume is reported as opened and it actually is
			 */
			if( m_autoOpenFolderOnMount ){
				Task * t = new Task() ;
				t->setMountPoint( utility::mountPath( m_point ) ) ;
				t->setMountPointOpener( m_folderOpener ) ;
				connect( t,SIGNAL( errorStatus( int,int,int ) ),this,SLOT( fileManagerOpenStatus( int,int,int ) ) ) ;
				t->start( Task::openMountPoint ) ;
			}
		}else{
			/*
			 * The volume is reported as opened but it isnt,possible reason is a backe end crash
			 */

			DialogMsg m( this ) ;

			m.ShowUIOK( tr( "ERROR" ),tr( "An error has occured and the volume could not be opened" ) ) ;
			emit cancel() ;
		}
		this->HideUI() ;
	}else{
		DialogMsg msg( this ) ;

		msg.ShowUIOK( tr( "ERROR" ),m ) ;
		m_ui->lineEditKey->clear() ;
		m_ui->rbKey->setChecked( true ) ;
		this->enableAll() ;
		m_ui->lineEditKey->setFocus() ;
	}
}

void keyDialog::walletIsOpen( bool opened )
{
	if( opened ){

		QString key ;
		QString id = m_path ;

		if( id.startsWith( QString( "UUID=" ) ) ){
			key = m_wallet->readValue( id ) ;
			if( key.isEmpty() ){
				key = m_wallet->readValue( id.replace( "\"","" ) ) ;
			}
		}else{
			QString uuid = utility::getUUIDFromPath( id ) ;
			if( uuid.isEmpty() ){
				key = m_wallet->readValue( id ) ;
			}else{
				key = m_wallet->readValue( uuid ) ;
				if( key.isEmpty() ){
					key = m_wallet->readValue( id ) ;
				}
			}
		}

		if( key.isEmpty() ){
			DialogMsg msg( this ) ;
			msg.ShowUIOK( tr( "ERROR" ),tr( "the volume does not appear to have an entry in the wallet" ) ) ;
			this->enableAll() ;
		}else{
			m_key = key ;
			this->openVolume() ;
		}
	}else{
		_internalPassWord.clear() ;
		this->enableAll() ;
		//DialogMsg msg( this ) ;
		//msg.ShowUIOK( tr( "ERROR" ),tr( "failed to open wallet" ) ) ;
	}

	m_wallet->deleteLater() ;
}

void keyDialog::getPassWord( QString password )
{
	_internalPassWord = password ;
}

void keyDialog::pbOpen()
{
	this->disableAll() ;
	m_key.clear() ;
	if( m_ui->rbPlugIn->isChecked() ){
		QString r = m_ui->lineEditKey->text() ;
		if( r == tr( KWALLET ) ){
			m_wallet = lxqt::Wallet::getWalletBackend( lxqt::Wallet::kwalletBackEnd ) ;
			m_wallet->setInterfaceObject( this ) ;
			m_wallet->open( utility::defaultKDEWalletName(),utility::applicationName() ) ;
		}else if( r == tr( INTERNAL_WALLET ) ){
			m_wallet = lxqt::Wallet::getWalletBackend( lxqt::Wallet::internalBackEnd ) ;
			m_wallet->setInterfaceObject( this ) ;
			QObject * obj = m_wallet->qObject() ;
			connect( obj,SIGNAL( getPassWord( QString ) ),this,SLOT( getPassWord( QString ) ) ) ;
			m_wallet->open( utility::walletName(),utility::applicationName(),_internalPassWord ) ;
		}else if( r == tr( GNOME_WALLET ) ){
			m_wallet = lxqt::Wallet::getWalletBackend( lxqt::Wallet::secretServiceBackEnd ) ;
			m_wallet->setInterfaceObject( this ) ;
			m_wallet->open( utility::walletName(),utility::applicationName() ) ;
		}else{
			this->openVolume() ;
		}
	}else{
		this->openVolume() ;
	}
}

void keyDialog::openVolume()
{
	if( m_ui->lineEditKey->text().isEmpty() ){
		if( m_ui->rbKey->isChecked() ){
			;
		}else if( m_ui->rbPlugIn->isChecked() ){
			DialogMsg msg( this ) ;
			msg.ShowUIOK( tr( "ERROR" ),tr( "plug in name field is empty" ) ) ;
			m_ui->lineEditKey->setFocus() ;
			return this->enableAll() ;
		}else if( m_ui->rbKeyFile->isChecked() ){
			DialogMsg msg( this ) ;
			msg.ShowUIOK( tr( "ERROR" ),tr( "keyfile field is empty" ) ) ;
			m_ui->lineEditKey->setFocus() ;
			return this->enableAll() ;
		}
	}

	QString test_name = m_ui->lineEditMountPoint->text() ;
	if( test_name.contains( QString( "/" ) ) ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "\"/\" character is not allowed in the mount name field" ) ) ;
		m_ui->lineEditKey->setFocus() ;
		return this->enableAll() ;
	}

	QString m ;
	if( m_ui->rbKey->isChecked() ){
		QString addr = socketSendKey::getSocketPath() ;
		m = QString( "-f ") + addr ;
		socketSendKey * s = new socketSendKey( this,addr,m_ui->lineEditKey->text().toAscii() ) ;
		s->sendKey() ;
	}else if( m_ui->rbKeyFile->isChecked() ){
		m = QString( "-f ") + m_ui->lineEditKey->text().replace( "\"","\"\"\"" ) ;
	}else if( m_ui->rbPlugIn->isChecked() ){
		if( m_key.isEmpty() ){
			m = QString( "-G ") + m_ui->lineEditKey->text().replace( "\"","\"\"\"" ) ;
		}else{
			QString addr = socketSendKey::getSocketPath() ;
			m = QString( "-f ") + addr ;
			socketSendKey * s = new socketSendKey( this,addr,m_key.toAscii() ) ;
			s->sendKey() ;
		}
	}

	Task * t = new Task() ;
	connect( t,SIGNAL( signalMountComplete( int,QString ) ),this,SLOT( slotMountComplete( int,QString ) ) ) ;

	t->setDevice( m_path ) ;

	if( m_ui->checkBoxOpenReadOnly->isChecked() ){
		t->setMode( QString( "ro" ) ) ;
	}else{
		t->setMode( QString( "rw" ) ) ;
	}

	t->setKeySource( m ) ;

	m_point = m_ui->lineEditMountPoint->text() ;

	t->setMountPoint( m_point ) ;
	t->setMakeMountPointPublic( m_ui->checkBoxShareMountPoint->isChecked() ) ;

	m_working = true ;

	t->start( Task::CryptoOpen ) ;
}

void keyDialog::rbPlugIn( bool opt )
{
	if( opt ){
		m_ui->pbkeyOption->setIcon( QIcon( QString( ":/module.png" ) ) ) ;
		m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
		m_ui->label->setText( tr( "plugin name" ) ) ;
		m_ui->lineEditKey->clear() ;
		m_ui->pbkeyOption->setEnabled( true ) ;
		m_ui->lineEditKey->setEnabled( false ) ;
	}
}

void keyDialog::rbKey( bool opt )
{
	if( opt ){
		m_ui->pbkeyOption->setIcon( QIcon( QString( ":/passphrase.png" ) ) ) ;
		m_ui->pbkeyOption->setEnabled( false ) ;
		m_ui->label->setText( tr( "key" ) ) ;
		m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;
		m_ui->pbkeyFile->setEnabled( false ) ;
		m_ui->lineEditKey->clear() ;
		m_ui->lineEditKey->setEnabled( true ) ;
	}
}

void keyDialog::rbKeyFile( bool opt )
{
	if( opt ){
		m_ui->pbkeyOption->setIcon( QIcon( QString( ":/keyfile.png" ) ) ) ;
		m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
		m_ui->label->setText( tr( "keyfile path" ) ) ;
		m_ui->pbkeyOption->setEnabled( true ) ;
		m_ui->lineEditKey->clear() ;
		m_ui->lineEditKey->setEnabled( false ) ;
	}
}

void keyDialog::pbCancel()
{
	this->HideUI() ;
	emit cancel() ;
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
	delete m_ui;
}

/*
 *
 *  Copyright ( c ) 2011-2015
 *  name : Francis Banyikwa
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
#include <QByteArray>
#include <QFileDialog>
#include <QStringList>
#include <QCloseEvent>
#include <QTableWidget>
#include <stdlib.h>

#include "ui_password.h"
#include "openvolume.h"
#include "tcrypt.h"
#include "task.h"
#include "utility.h"
#include "dialogmsg.h"
#include "plugin_path.h"
#include "tablewidget.h"
#include "../zuluCrypt-cli/constants.h"

#include "utility.h"

#define KWALLET         "KDE Wallet"
#define INTERNAL_WALLET "Internal Wallet"
#define GNOME_WALLET    "GNOME keyring"

/*
 * this header is created at config time
 */
#include "truecrypt_support.h"

/*
 * this ugly global variable is defined in zulucrypt.cpp to prevent multiple prompts when opening multiple volumes
 */
static QString _internalPassWord ;

passwordDialog::passwordDialog( QTableWidget * table,QWidget * parent ) : QDialog( parent )
{
	m_ui = new Ui::PasswordDialog() ;
	m_ui->setupUi( this ) ;

	m_parent = parent ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;
	this->setDefaultOpenMode() ;

	m_ui->PushButtonMountPointPath->setIcon( QIcon( ":/folder.png" ) ) ;

	m_open_with_path = false ;

	m_table = table ;

	m_pluginMenu = new QMenu( this ) ;
	m_pluginMenu->setFont( this->font() ) ;

	m_ui->pushButtonPlugin->setIcon( QIcon( ":/module.png" ) ) ;

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

	m_veraCryptWarning.setWarningLabel( m_ui->veraCryptWarning ) ;

	this->installEventFilter( this ) ;
}

bool passwordDialog::eventFilter( QObject * watched,QEvent * event )
{
	if( utility::eventFilter( this,watched,event ) ){
		this->HideUI() ;
		return true ;
	}else{
		return false ;
	}
}

void passwordDialog::pbPlugin()
{
	utility::createPlugInMenu( m_pluginMenu,tr( INTERNAL_WALLET ),
				   tr( GNOME_WALLET ),tr( KWALLET ),true ) ;

	m_pluginMenu->addSeparator() ;

	m_pluginMenu->addAction( tr( "Cancel" ) ) ;

	connect( m_pluginMenu,SIGNAL( triggered( QAction * ) ),this,SLOT( pbPluginEntryClicked( QAction * ) ) ) ;

	m_pluginMenu->exec( QCursor::pos() ) ;
}

void passwordDialog::pbPluginEntryClicked( QAction * e )
{
	QString text = e->text() ;
	if( text != tr( "Cancel" ) ){
		m_ui->PassPhraseField->setText( text ) ;
	}
}

void passwordDialog::tcryptCancelled( void )
{
	m_key.clear() ;
	m_keyFiles.clear() ;
	m_ui->cbKeyType->setCurrentIndex( passwordDialog::key ) ;
	m_ui->PassPhraseField->setText( "" ) ;
	this->enableAll() ;
}

void passwordDialog::tcryptGui()
{
	this->disableAll() ;
	m_ui->PassPhraseField->setText( QString() ) ;

	tcrypt * t = new tcrypt( this ) ;
	connect( t,SIGNAL( Keys( QString,QStringList ) ),this,SLOT( keys( QString,QStringList ) ) ) ;
	connect( t,SIGNAL( cancelled() ),this,SLOT( tcryptCancelled() ) ) ;

	t->ShowUI() ;
}

void passwordDialog::keys( QString key,QStringList keyFiles )
{
	m_key = key ;
	m_keyFiles = keyFiles ;
	this->openVolume() ;
	m_ui->cbKeyType->setCurrentIndex( passwordDialog::key ) ;
	m_ui->PassPhraseField->setText( "" ) ;
}

void passwordDialog::cbStateChanged( int state )
{
	m_ui->checkBoxReadOnly->setEnabled( false ) ;
	m_ui->checkBoxReadOnly->setChecked( utility::setOpenVolumeReadOnly( this,state == Qt::Checked,"zuluCrypt-gui" ) ) ;
	m_ui->checkBoxReadOnly->setEnabled( true ) ;
}

void passwordDialog::setDefaultOpenMode()
{
	m_ui->checkBoxReadOnly->setChecked( utility::getOpenVolumeReadOnlyOption( "zuluCrypt-gui" ) ) ;
}

void passwordDialog::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

void passwordDialog::addTcryptVcryptKeyOption()
{
	if( m_veraCryptVolume ){
		m_ui->cbKeyType->addItem( tr( "VeraCrypt Keys" ) ) ;
	}else{
		#if TRUECRYPT_CRYPTSETUP
			m_ui->cbKeyType->addItem( tr( "TrueCrypt Keys" ) ) ;
		#endif
	}
}

void passwordDialog::ShowUI( const QString& volumePath,const QString& mount_point )
{
	this->addTcryptVcryptKeyOption() ;

	if( mount_point.isEmpty() ){
		m_point = utility::mountPathPostFix( volumePath.split( "/" ).last() ) ;
	}else {
		m_point = utility::mountPathPostFix( mount_point.split( "/" ).last() ) ;
	}

	m_open_with_path = true ;
	this->passphraseOption() ;
	m_ui->OpenVolumePath->setText( volumePath ) ;
	m_ui->OpenVolumePath->setEnabled( false ) ;
	m_ui->PushButtonVolumePath->setEnabled( false ) ;
	m_ui->MountPointPath->setText( m_point ) ;
	m_ui->PassPhraseField->setFocus() ;
	QString vp = volumePath.mid( 0,5 ) ;
	if( vp == "/dev/" || vp == "UUID=" ){
		m_ui->PushButtonVolumePath->setIcon( QIcon( ":/partition.png" ) ) ;
	}else{
		m_ui->PushButtonVolumePath->setIcon( QIcon( ":/file.png" ) ) ;
	}
	this->show() ;
}

void passwordDialog::setTitle()
{
	if( m_veraCryptVolume ){

		this->setWindowTitle( tr( "Unlock VeraCrypt Volume" ) ) ;
	}else{
		this->setWindowTitle( tr( "Unlock Encrypted Volume" ) ) ;
	}
}

void passwordDialog::ShowUI( QString dev )
{
	this->addTcryptVcryptKeyOption() ;

	this->setTitle() ;

	QString m_point = utility::homePath() + "/" + dev.split( "/" ).last() ;
	this->ShowUI( dev,m_point ) ;
}

void passwordDialog::ShowUI()
{
	this->addTcryptVcryptKeyOption() ;

	this->setTitle() ;

	this->passphraseOption() ;
	m_ui->OpenVolumePath->setFocus() ;
	m_ui->PushButtonVolumePath->setIcon( QIcon( ":/file.png" ) ) ;
	this->show() ;
}

void passwordDialog::ShowVeraUI()
{
	m_veraCryptVolume = true ;
	this->ShowUI() ;
}

void passwordDialog::ShowVeraUI( QString dev )
{
	m_veraCryptVolume = true ;
	this->ShowUI( dev ) ;
}

void passwordDialog::mountPointPath( QString path )
{
	m_ui->MountPointPath->setText( utility::mountPathPostFix( path.split( "/" ).last() ) ) ;
}

void passwordDialog::cbActicated( int e )
{
	switch( e ){
		case passwordDialog::key     : return this->passphraseOption() ;
		case passwordDialog::keyfile : return this->passphraseFromFileOption() ;
		case passwordDialog::plugin  : return this->pluginOption() ;
		case passwordDialog::tcryptKeys : return this->tcryptGui() ;
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
	m_ui->pushButtonPassPhraseFromFile->setToolTip( tr( "Choose A Module From The File System" ) ) ;
	m_ui->PassPhraseField->setToolTip( tr( "Enter A Module Name To Use To Get Passphrase" ) ) ;
	m_ui->PassPhraseField->setEchoMode( QLineEdit::Normal ) ;
	m_ui->pushButtonPassPhraseFromFile->setEnabled( false ) ;
	m_ui->labelPassphrase->setText( tr( "Plugin Name" ) ) ;
	m_ui->pushButtonPassPhraseFromFile->setIcon( QIcon( ":/keyfile.png" ) ) ;
	m_ui->pbKeyOption->setIcon( QIcon( ":/module.png" ) ) ;
	m_ui->pushButtonPlugin->setEnabled( true ) ;
	m_ui->pushButtonPlugin->setToolTip( tr( "Select A Key Module" ) ) ;
	m_ui->pbKeyOption->setEnabled( true ) ;
	m_ui->PassPhraseField->setEnabled( false ) ;
	m_ui->PassPhraseField->setText( INTERNAL_WALLET ) ;
}

void passwordDialog::passphraseOption()
{
	m_ui->PassPhraseField->setToolTip( tr( "Enter A Key" ) ) ;
	m_ui->PassPhraseField->setEchoMode( QLineEdit::Password ) ;
	m_ui->PassPhraseField->clear() ;
	m_ui->pushButtonPassPhraseFromFile->setEnabled( false ) ;
	m_ui->labelPassphrase->setText( tr( "Key" ) ) ;
	m_ui->pushButtonPassPhraseFromFile->setIcon( QIcon( ":/passphrase.png" ) ) ;
	m_ui->pushButtonPlugin->setEnabled( false ) ;
	m_ui->pbKeyOption->setIcon( QIcon( QString() ) ) ;
	m_ui->pbKeyOption->setEnabled( false ) ;
	m_ui->PassPhraseField->setEnabled( true ) ;
}

void passwordDialog::passphraseFromFileOption()
{
	m_ui->pushButtonPassPhraseFromFile->setToolTip( tr( "Choose A Key File From The File System" ) ) ;
	m_ui->PassPhraseField->setToolTip( tr( "Enter A path To A Keyfile Location" ) ) ;
	m_ui->PassPhraseField->setEchoMode( QLineEdit::Normal ) ;
	m_ui->PassPhraseField->clear() ;
	m_ui->pushButtonPassPhraseFromFile->setEnabled( true ) ;
	m_ui->labelPassphrase->setText( tr( "Keyfile Path" ) ) ;
	m_ui->pushButtonPassPhraseFromFile->setIcon( QIcon( ":/keyfile.png" ) ) ;
	m_ui->pushButtonPlugin->setEnabled( false ) ;
	m_ui->pbKeyOption->setIcon( QIcon( ":/keyfile.png" ) ) ;
	m_ui->pushButtonPlugin->setEnabled( true ) ;
	m_ui->pbKeyOption->setEnabled( true ) ;
	m_ui->PassPhraseField->setEnabled( true ) ;
}

void passwordDialog::clickedPassPhraseFromFileButton()
{
	QString msg ;
	if( m_ui->cbKeyType->currentIndex() == passwordDialog::keyfile ){
		msg = tr( "Select A Keyfile" ) ;
	}else{
		msg = tr( "Select A Key Module" ) ;
	}

	QString Z = QFileDialog::getOpenFileName( this,msg,utility::homePath(),0 ) ;
	if( !Z.isEmpty() ){
		m_ui->PassPhraseField->setText( Z ) ;
	}
}

void passwordDialog::mount_point( void )
{
	QString p = tr( "Select Path To Mount Point Folder" ) ;
	QString Z = QFileDialog::getExistingDirectory( this,p,utility::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( !Z.isEmpty() ){
		Z = Z + "/" + m_ui->OpenVolumePath->text().split( "/" ).last() ;
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

	QString Z = QFileDialog::getOpenFileName( this,tr( "Select Encrypted volume" ),utility::homePath(),0 ) ;
	m_ui->OpenVolumePath->setText( Z ) ;
	if( !Z.isEmpty() ){
		m_ui->MountPointPath->setText( utility::mountPathPostFix( Z.split( "/" ).last() ) ) ;
	}
}

void passwordDialog::HideUI()
{
	if( !m_working ){
		this->hide() ;
		emit HideUISignal() ;
	}
}

void passwordDialog::buttonOpenClicked( void )
{
	this->disableAll() ;

	if( m_ui->cbKeyType->currentIndex() == passwordDialog::plugin ){

		QString wallet = m_ui->PassPhraseField->text() ;
		QString keyID = m_ui->OpenVolumePath->text() ;

		utility::wallet w ;

		if( wallet == tr( KWALLET ) ){

			w = utility::getKeyFromWallet( LxQt::Wallet::kwalletBackEnd,keyID ) ;

		}else if( wallet == tr( INTERNAL_WALLET ) ){

			w = utility::getKeyFromWallet( LxQt::Wallet::internalBackEnd,keyID,_internalPassWord ) ;

			if( w.notConfigured ){

				DialogMsg msg( this ) ;
				msg.ShowUIOK( tr( "ERROR!" ),tr( "Internal wallet is not configured" ) ) ;
				return this->enableAll() ;

			}else{
				_internalPassWord = w.password ;
			}

		}else if( wallet == tr( GNOME_WALLET ) ){

			w = utility::getKeyFromWallet( LxQt::Wallet::secretServiceBackEnd,keyID ) ;
		}else{
			m_key = m_ui->PassPhraseField->text() ;
			return this->openVolume() ;
		}

		if( w.opened ){

			if( w.key.isEmpty() ){
				DialogMsg msg( this ) ;
				msg.ShowUIOK( tr( "ERROR!" ),tr( "The volume does not appear to have an entry in the wallet" ) ) ;
				this->enableAll() ;
			}else{
				m_key = w.key ;
				this->openVolume() ;
			}
		}else{
			_internalPassWord.clear() ;
			this->enableAll() ;
		}
	}else{
		m_key = m_ui->PassPhraseField->text() ;
		this->openVolume() ;
	}
}

void passwordDialog::sendKey( const QString& sockpath )
{
	utility::keySend( sockpath,m_key ) ;
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

void passwordDialog::openVolume()
{
	m_device = utility::resolvePath( m_ui->OpenVolumePath->text() ) ;

	m_point = m_ui->MountPointPath->text() ;
	if( m_point.isEmpty() || m_device.isEmpty() ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
		return this->enableAll() ;
	}

	if( m_point.contains( "/" ) ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR!" ),tr( "\"/\" character is not allowed in mount name field" ) ) ;
		m_ui->OpenVolumePath->setFocus() ;
		return this->enableAll() ;
	}

	QString mode ;

	if( m_ui->checkBoxReadOnly->isChecked() ){
		mode =" ro" ;
	}else{
		mode = "rw" ;
	}

	QString passtype ;

	QString keyPath ;

	int keySource = m_ui->cbKeyType->currentIndex() ;

	if( keySource == passwordDialog::keyfile ){
		if( m_key.isEmpty() ){
			DialogMsg msg( this ) ;
			msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
			return this->enableAll() ;
		}else{
			passtype = "-f" ;
			keyPath = utility::resolvePath( m_key ).replace( "\"","\"\"\"" ) ;
		}
	}else if( keySource == passwordDialog::key ){
		passtype = "-f" ;
		keyPath = utility::keyPath() ;
		this->sendKey( keyPath ) ;
	}else if( keySource == passwordDialog::plugin ){
		if( m_key.isEmpty() ){
			DialogMsg msg( this ) ;
			msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
			return this->enableAll() ;
		}else{
			QString r = m_ui->PassPhraseField->text() ;
			if( r == tr( KWALLET ) || r == tr( INTERNAL_WALLET ) || r == tr( GNOME_WALLET ) ){
				passtype = "-f" ;
				keyPath = utility::keyPath() ;
				this->sendKey( keyPath ) ;
			}else{
				passtype = "-G" ;
				keyPath  = r ;
			}
		}
	}else if( keySource == passwordDialog::tcryptKeys ){
		passtype = "-f" ;
		keyPath = utility::keyPath() ;
		this->sendKey( keyPath ) ;
	}else{
		qDebug() << "Error: uncaught condition" ;
	}

	QString a = ZULUCRYPTzuluCrypt ;
	QString b = m_device ;
	b.replace( "\"","\"\"\"" ) ;
	QString c = m_point ;
	c.replace( "\"","\"\"\"" ) ;
	const QString& d = mode ;
	const QString& e = passtype ;
	const QString& f = keyPath ;

	QString exe = QString( "%1 -o -d \"%2\" -m \"%3\" -e %4 %5 \"%6\"" ).arg( a,b,c,d,e,f ) ;

	if( !m_keyFiles.isEmpty() ){

		for( const auto& it : m_keyFiles ){

			QString e = it ;
			e.replace( "\"","\"\"\"" ) ;

			exe += " -F \"" + e + "\"" ;
		}
	}

	if( m_veraCryptVolume ){

		exe += " -t vera" ;
	}

	this->disableAll() ;

	m_veraCryptWarning.show( m_veraCryptVolume ) ;

	m_working = true ;

	auto r = utility::Task::run( utility::appendUserUID( exe ) ).await() ;

	m_working = false ;

	m_veraCryptWarning.stopTimer() ;

	if( r.success() ){

		this->success( r.output() ) ;
	}else{
		this->failed( r.exitCode() ) ;

		m_veraCryptWarning.hide() ;
	}
}

void passwordDialog::success( const QByteArray& r )
{
	if( utility::mapperPathExists( m_device ) ){

		QStringList list ;

		list.append( utility::resolvePath( m_ui->OpenVolumePath->text() ) ) ;

		QString m_p = utility::mountPath( m_point ) ;

		list.append( m_p ) ;

		if( r.contains( "luks" ) ){
			list.append( "luks" ) ;
		}else if( r.contains( "plain" ) ){
			list.append( "plain" ) ;
		}else if( r.contains( "tcrypt" ) ){
			list.append( "tcrypt" ) ;
		}else if( r.contains( "vcrypt" ) ){
			list.append( "vcrypt" ) ;
		}else{
			list.append( "Nil" ) ;
		}

		tablewidget::addRowToTable( m_table,list ) ;

		emit openFolder( m_p ) ;

		this->HideUI() ;
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

void passwordDialog::failed( int r )
{
	if( r == 12 && m_ui->cbKeyType->currentIndex() == passwordDialog::plugin ){
		/*
		 * A user cancelled the plugin
		 */
		return this->enableAll() ;
	}

	DialogMsg msg( this ) ;
	switch ( r ){
		case 0 : ;
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
		case 14: msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient memory to hold passphrase" ) ) ;							break ;
		case 15: msg.ShowUIOK( tr( "ERROR!" ),tr( "One or more required argument(s) for this operation is missing" ) ) ;				break ;
		case 16: msg.ShowUIOK( tr( "ERROR!" ),tr( "Invalid path to key file" ) ) ;								break ;
		case 17: msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not get enought memory to hold the key file" ) ) ;					break ;
		case 18: msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to open key file for reading" ) ) ;					break ;
		case 19: msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not get a passphrase through a local socket" ) ) ;					break ;
		case 20: msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to mount a filesystem:invalid/unsupported mount option or unsupported file system encountered" ) ) ;	break ;
		case 21: msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not create a lock on /etc/mtab" ) ) ;							break ;
		case 22: msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to open a system volume.\n\nConsult menu->help->permission for more informaion\n" ) ) ;					break ;
		case 113:msg.ShowUIOK( tr( "ERROR!" ),tr( "A non supported device encountered,device is missing or permission denied\n\
Possible reasons for getting the error are:\n1.Device path is invalid.\n2.The device has LVM or MDRAID signature" ) ) ;					break ;
		default: msg.ShowUIOK( tr( "ERROR!" ),tr( "Unrecognized ERROR with status number %1 encountered" ).arg( r ) ) ;
	}

	this->enableAll() ;

	if( r == 4 ){
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

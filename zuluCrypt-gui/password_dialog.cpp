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
#include "lxqt_wallet.h"
#include "../plugin_path.h"

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
#include "task.hpp"
#include "utility.h"
#include "dialogmsg.h"
#include "plugin_path.h"
#include "tablewidget.h"
#include "../zuluCrypt-cli/constants.h"
#include "favorites2.h"
#include "utility.h"

passwordDialog::passwordDialog( QTableWidget * table,
				QWidget * parent,
				secrets& s,
				std::function< void( const QString& ) > f ) :
	QDialog( parent ),
	m_ui( new Ui::PasswordDialog() ),
	m_secrets( s ),
	m_openFolder( std::move( f ) )
{
	m_ui->setupUi( this ) ;

	m_ui->frameOptions->setVisible( false ) ;

	m_label.setOptions( m_ui->veraCryptWarning,m_ui->pbOK ) ;

	m_ui->PassPhraseField->setMaxLength( 32767 ) ;

	m_parent = parent ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;
	this->setDefaultOpenMode() ;

	m_ui->cbShareMountPoint->setChecked( utility::mountWithSharedMountPoint() ) ;

	m_ui->PushButtonMountPointPath->setIcon( QIcon( ":/folder.png" ) ) ;

	m_ui->pushButtonLuksExternalHeaderPath->setIcon( QIcon( ":/file.png" ) ) ;

	m_open_with_path = false ;

	m_table = table ;

	m_pluginMenu = new QMenu( this ) ;
	m_pluginMenu->setFont( this->font() ) ;

	auto cc = static_cast< void( QComboBox::* )( int )>( &QComboBox::currentIndexChanged ) ;

	connect( m_ui->PushButtonCancel,&QPushButton::clicked,this,&passwordDialog::HideUI ) ;
	connect( m_ui->PushButtonMountPointPath,&QPushButton::clicked,this,&passwordDialog::mount_point ) ;
	connect( m_ui->PushButtonVolumePath,&QPushButton::clicked,this,&passwordDialog::file_path ) ;
	connect( m_ui->pushButtonPassPhraseFromFile,&QPushButton::clicked,this,&passwordDialog::clickedPassPhraseFromFileButton ) ;
	connect( m_ui->OpenVolumePath,&QLineEdit::textChanged,this,&passwordDialog::mountPointPath ) ;
	connect( m_ui->checkBoxReadOnly,&QCheckBox::stateChanged,this,&passwordDialog::cbStateChanged ) ;
	connect( m_ui->pbKeyOption,&QPushButton::clicked,this,&passwordDialog::pbKeyOption ) ;
	connect( m_ui->cbKeyType,cc,this,&passwordDialog::cbActicated ) ;
	connect( m_ui->cbVolumeType,cc,this,&passwordDialog::cbVolumeType ) ;
	connect( m_ui->checkBoxVisibleKey,&QCheckBox::stateChanged,this,&passwordDialog::cbVisibleKeyStateChanged ) ;

	connect( m_ui->cbShareMountPoint,&QCheckBox::stateChanged,[]( int s ){

		utility::mountWithSharedMountPoint( s == Qt::Checked ) ;
	} ) ;

	connect( m_ui->PushButtonOpen,&QPushButton::clicked,[ this ](){

		this->openVolume( m_ui->PassPhraseField->text() ) ;
	} ) ;

	m_ui->cbVolumeType->setCurrentIndex( utility::defaultUnlockingVolumeType() ) ;
	this->cbVolumeType( utility::defaultUnlockingVolumeType() ) ;

	m_ui->PushButtonMountPointPath->setVisible( false ) ;
	m_ui->pushButtonPassPhraseFromFile->setVisible( false ) ;

	m_veraCryptWarning.setWarningLabel( m_ui->veraCryptWarning ) ;

	m_ui->cbShareMountPoint->setToolTip( utility::shareMountPointToolTip() ) ;

	m_ui->cbKeyType->addItem( tr( "TrueCrypt/VeraCrypt Keys" ) ) ;

	m_ui->cbKeyType->addItem( tr( "YubiKey Challenge/Response" ) ) ;

	connect( m_ui->PushButtonOptions,&QPushButton::clicked,[ this ](){

		auto m = m_ui->OpenVolumePath->text() ;

		m_ui->lineEditFsOptions->setText( utility::fileSystemOptions( m ) ) ;
		m_ui->lineEditFsOptions->setText( m_fsOptions ) ;
		m_ui->frameOptions->setVisible( true ) ;
	} ) ;

	connect( m_ui->pbSet,&QPushButton::clicked,[ this ](){

		m_fsOptions = m_ui->lineEditFsOptions->text() ;
		m_ui->frameOptions->setVisible( false ) ;
	} ) ;

	connect( m_ui->pbCancel,&QPushButton::clicked,[ this ](){

		m_ui->frameOptions->setVisible( false ) ;
	} ) ;

	m_ui->pushButtonPlainDmCryptOptions->setMenu( [ this ](){

		auto m = new QMenu( this ) ;

		connect( m,&QMenu::triggered,this,&passwordDialog::plainDmCryptOption ) ;

		auto s = utility::plainDmCryptOptions() ;

		if( s.isEmpty() ){

			m_plainDmCryptProperty = "aes.cbc-essiv:sha256.256.ripemd160" ;
			m->addAction( "aes.cbc-essiv:sha256.256.ripemd160" ) ;
		}else{
			m_plainDmCryptProperty = s.first() ;

			for( const auto& it : s ){

				m->addAction( it ) ;
			}
		}

		return m ;
	}() ) ;

	connect( m_ui->pushButtonLuksExternalHeaderPath,&QPushButton::clicked,[ this ](){

		auto a = tr( "Select External LUKS Header File" ) ;

		auto Z = QFileDialog::getOpenFileName( this,a,utility::homePath() ) ;

		if( !Z.isEmpty() ){

			m_ui->lineEditVolumeProperty->setText( Z ) ;
		}
	} ) ;

	m_ui->checkBoxVisibleKey->setToolTip( tr( "Check This Box To Make Password Visible" ) ) ;

	this->setWindowTitle( tr( "Unlock Encrypted Volume" ) ) ;

	this->installEventFilter( this ) ;
}

void passwordDialog::plainDmCryptOption( QAction * ac )
{
	m_plainDmCryptProperty = ac->text().remove( "&" ) ;
}

void passwordDialog::cbVolumeType( int e )
{
	m_ui->lineEditVolumeProperty->clear() ;

	m_ui->lineEditVolumeProperty->setToolTip( QString() ) ;

	if( e == 0 ){

		/*
		 * LUKS,TrueCrypt,BitLocker
		 */
		m_ui->labelVolumeProperty->setText( tr( "LUKS External Header Path" ) ) ;
		m_ui->lineEditVolumeProperty->setEnabled( true ) ;
		m_ui->pushButtonPlainDmCryptOptions->setEnabled( false ) ;
		m_veraCryptVolume = false ;
		m_ui->pushButtonLuksExternalHeaderPath->setEnabled( true ) ;
		m_ui->lineEditVolumeProperty->setEchoMode( QLineEdit::Normal ) ;

	}else if( e == 1 || e == 2 ){

		/*
		 * VeraCrypt volume
		 */
		m_ui->labelVolumeProperty->setText( tr( "PIM Value" ) ) ;
		m_ui->lineEditVolumeProperty->setEnabled( true ) ;
		m_ui->pushButtonPlainDmCryptOptions->setEnabled( false ) ;
		m_veraCryptVolume = true ;
		m_ui->pushButtonLuksExternalHeaderPath->setEnabled( false ) ;
		m_ui->lineEditVolumeProperty->setEchoMode( QLineEdit::Password ) ;

	}else if( e == 3 ){

		/*
		 * PLAIN dm-crypt
		 */
		m_ui->pushButtonPlainDmCryptOptions->setEnabled( true ) ;
		m_ui->labelVolumeProperty->setText( tr( "Offset" ) ) ;
		m_ui->lineEditVolumeProperty->setEnabled( true ) ;
		m_ui->lineEditVolumeProperty->setToolTip( tr( "Offset Will Be In Sectors If The Entry Is Made Up Of Only Digits\nAnd In Bytes If The Entry Ends With \"b\"\nAnd In Kilobytes If The Entry Ends With \"k\"\nAnd In Megabytes If The Entry Ends With \"m\"\nAnd In Terabytes If The Entry Ends With \"t\"" ) ) ;
		m_veraCryptVolume = false ;
		m_ui->pushButtonLuksExternalHeaderPath->setEnabled( false ) ;
		m_ui->lineEditVolumeProperty->setEchoMode( QLineEdit::Password ) ;
	}

	utility::defaultUnlockingVolumeType( e ) ;
}

bool passwordDialog::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void passwordDialog::pbPlugin()
{
	m_pluginMenu->clear() ;

	m_pluginMenu->addSeparator() ;

	utility::addPluginsToMenu( *m_pluginMenu ) ;

	m_pluginMenu->addAction( tr( "Cancel" ) )->setObjectName( "Cancel" ) ;

	connect( m_pluginMenu,&QMenu::triggered,this,&passwordDialog::pbPluginEntryClicked ) ;

	m_pluginMenu->exec( QCursor::pos() ) ;
}

void passwordDialog::pbPluginEntryClicked( QAction * e )
{
	auto m = e->objectName() ;

	utility::setDefaultPlugin( m ) ;

	auto text = e->text() ;

	text.remove( "&" ) ;

	if( m != "Cancel" ){

		m_ui->PassPhraseField->setText( text ) ;
	}
}

void passwordDialog::tcryptGui()
{
	this->disableAll() ;

	m_ui->PassPhraseField->setText( QString() ) ;

	tcrypt::instance( this,false,[ this ]( const QString& key,const QStringList& keyFiles ) {

		m_keyFiles = keyFiles ;

		this->openVolume( key ) ;

		m_ui->cbKeyType->setCurrentIndex( passwordDialog::key ) ;
		m_ui->PassPhraseField->setText( QString() ) ;

	},[ this ](){

		m_key.clear() ;
		m_keyFiles.clear() ;
		m_ui->cbKeyType->setCurrentIndex( passwordDialog::key ) ;
		m_ui->PassPhraseField->setText( QString() ) ;

		this->enableAll() ;
	} ) ;
}

void passwordDialog::cbStateChanged( int state )
{
	m_ui->checkBoxReadOnly->setEnabled( false ) ;
	m_ui->checkBoxReadOnly->setChecked( utility::setOpenVolumeReadOnly( this,state == Qt::Checked,"zuluCrypt-gui" ) ) ;
	m_ui->checkBoxReadOnly->setEnabled( true ) ;
}

void passwordDialog::autoSetPassword( const QString& keyID )
{
	auto m = favorites2::settings().autoMountBackEnd() ;

	if( m.isInvalid() ){

		return ;
	}

	auto secret = m_secrets.walletBk( m.bk() ).getKey( keyID ) ;

	if( secret.notConfigured ){

		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR!" ),tr( "Internal wallet is not configured" ) ) ;
	}else{
		if( secret.key.isEmpty() ){

			secret = m_secrets.walletBk( m.bk() ).getKey( utility::pathToUUID( keyID ) ) ;
		}

		m_ui->PassPhraseField->setText( secret.key ) ;
	}
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

void passwordDialog::ShowUI( const QString& volumePath,const QString& mount_point )
{
	auto volume = volumePath.split( "/" ).last() ;

	this->setWindowTitle( tr( "Mount \"%1\"" ).arg( volume ) ) ;

	if( mount_point.isEmpty() ){

		m_point = utility::mountPathPostFix( volume ) ;
	}else{
		m_point = utility::mountPathPostFix( mount_point.split( "/" ).last() ) ;
	}

	m_open_with_path = true ;

	this->passphraseOption() ;

	m_ui->OpenVolumePath->setText( volumePath ) ;

	m_ui->OpenVolumePath->setEnabled( false ) ;
	m_ui->PushButtonVolumePath->setEnabled( false ) ;
	m_ui->MountPointPath->setText( m_point ) ;
	m_ui->PassPhraseField->setFocus() ;

	if( volumePath.startsWith( "/dev/" ) || volumePath.startsWith( "UUID=" ) ){

		m_ui->PushButtonVolumePath->setIcon( QIcon( ":/partition.png" ) ) ;
	}else{
		m_ui->PushButtonVolumePath->setIcon( QIcon( ":/file.png" ) ) ;
	}

	this->autoSetPassword( volumePath ) ;
	this->show() ;
}

void passwordDialog::ShowUI( QString dev )
{
	auto m_point = utility::homePath() + "/" + dev.split( "/" ).last() ;

	this->ShowUI( dev,m_point ) ;
}

void passwordDialog::cbVisibleKeyStateChanged( int s )
{
	if( m_ui->cbKeyType->currentIndex() == passwordDialog::key ){

		if( s == Qt::Checked ){

			m_ui->PassPhraseField->setEchoMode( QLineEdit::Normal ) ;
		}else{
			m_ui->PassPhraseField->setEchoMode( QLineEdit::Password ) ;
		}

		m_ui->PassPhraseField->setFocus() ;
	}
}

void passwordDialog::ShowUI()
{
	this->passphraseOption() ;

	m_ui->OpenVolumePath->setFocus() ;
	m_ui->PushButtonVolumePath->setIcon( QIcon( ":/file.png" ) ) ;

	this->show() ;
}

void passwordDialog::mountPointPath( QString path )
{
	m_ui->MountPointPath->setText( utility::mountPathPostFix( path.split( "/" ).last() ) ) ;
}

void passwordDialog::cbActicated( int e )
{
	if( e == passwordDialog::key || e == passwordDialog::yubikey ){

		m_ui->checkBoxVisibleKey->setEnabled( true ) ;
	}else{
		m_ui->checkBoxVisibleKey->setEnabled( false ) ;
		m_ui->checkBoxVisibleKey->setChecked( false ) ;
	}

	switch( e ){

		case passwordDialog::key         : return this->passphraseOption() ;
		case passwordDialog::yubikey     : return this->passphraseOption() ;
		case passwordDialog::keyfile     : return this->passphraseFromFileOption() ;
		case passwordDialog::keyKeyFile  : return this->keyAndKeyFile() ;
		case passwordDialog::plugin      : return this->pluginOption() ;
		case passwordDialog::tcryptKeys  : return this->tcryptGui() ;
	}
}

void passwordDialog::keyAndKeyFile()
{
	QByteArray key ;

	if( utility::pluginKey( this,&key,"hmac" ) ){

		m_ui->cbKeyType->setCurrentIndex( 0 ) ;
	}else{
		this->passphraseOption() ;

		m_ui->PassPhraseField->setEnabled( false ) ;
		m_ui->PassPhraseField->setText( key ) ;
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
	m_ui->pbKeyOption->setEnabled( true ) ;
	m_ui->PassPhraseField->setEnabled( false ) ;

	m_ui->PassPhraseField->setText( utility::defaultPlugin() ) ;
}

void passwordDialog::passphraseOption()
{
	m_ui->PassPhraseField->setToolTip( tr( "Enter A Key" ) ) ;
	m_ui->PassPhraseField->setEchoMode( QLineEdit::Password ) ;
	m_ui->PassPhraseField->clear() ;
	m_ui->pushButtonPassPhraseFromFile->setEnabled( false ) ;
	m_ui->labelPassphrase->setText( tr( "Password" ) ) ;
	m_ui->pushButtonPassPhraseFromFile->setIcon( QIcon( ":/passphrase.png" ) ) ;
	m_ui->pbKeyOption->setIcon( QIcon() ) ;
	m_ui->pbKeyOption->setEnabled( false ) ;
	m_ui->PassPhraseField->setFocus() ;
	m_ui->PassPhraseField->setEnabled( true ) ;
}

void passwordDialog::passphraseFromFileOption()
{
	m_ui->pushButtonPassPhraseFromFile->setToolTip( tr( "Choose A KeyFile From The File System" ) ) ;
	m_ui->PassPhraseField->setToolTip( tr( "Enter A Path To A Keyfile Location" ) ) ;
	m_ui->PassPhraseField->setEchoMode( QLineEdit::Normal ) ;
	m_ui->PassPhraseField->clear() ;
	m_ui->pushButtonPassPhraseFromFile->setEnabled( true ) ;
	m_ui->pushButtonPassPhraseFromFile->setFocus() ;
	m_ui->labelPassphrase->setText( tr( "KeyFile Path" ) ) ;
	m_ui->pushButtonPassPhraseFromFile->setIcon( QIcon( ":/keyfile.png" ) ) ;
	m_ui->pbKeyOption->setIcon( QIcon( ":/keyfile.png" ) ) ;
	m_ui->pbKeyOption->setEnabled( true ) ;
	m_ui->PassPhraseField->setEnabled( true ) ;
}

void passwordDialog::clickedPassPhraseFromFileButton()
{
	QString msg ;

	if( m_ui->cbKeyType->currentIndex() == passwordDialog::keyfile ){

		msg = tr( "Select A KeyFile" ) ;
	}else{
		msg = tr( "Select A Key Module" ) ;
	}

	auto Z = QFileDialog::getOpenFileName( this,msg,utility::homePath() ) ;

	if( !Z.isEmpty() ){

		m_ui->PassPhraseField->setText( Z ) ;
	}
}

void passwordDialog::mount_point( void )
{
	auto p = tr( "Select Path To Mount Point Folder" ) ;
	auto Z = QFileDialog::getExistingDirectory( this,p,utility::homePath(),QFileDialog::ShowDirsOnly ) ;

	while( true ){

		if( Z.endsWith( '/' ) ){

			Z.truncate( Z.length() - 1 ) ;
		}else{
			break ;
		}
	}

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
	auto Z = QFileDialog::getOpenFileName( this,tr( "Select Encrypted volume" ),utility::homePath() ) ;

	m_ui->OpenVolumePath->setText( Z ) ;

	if( !Z.isEmpty() ){
		this->autoSetPassword( Z ) ;
		m_ui->MountPointPath->setText( utility::mountPathPostFix( Z.split( "/" ).last() ) ) ;
		m_ui->PassPhraseField->setFocus() ;
	}
}

void passwordDialog::HideUI()
{
	if( !m_working ){

		this->hide() ;
		this->deleteLater() ;
	}
}

void passwordDialog::sendKey( const QString& sockpath )
{
	utility::keySend( sockpath,m_key ) ;
}

void passwordDialog::disableAll()
{
	m_ui->PushButtonOptions->setEnabled( false ) ;
	m_ui->labelVolumeProperty->setEnabled( false ) ;
	m_ui->lineEditVolumeProperty->setEnabled( false ) ;
	m_ui->pushButtonPlainDmCryptOptions->setEnabled( false ) ;
	m_ui->cbVolumeType->setEnabled( false ) ;
	m_ui->labelVolumeType->setEnabled( false ) ;
	m_ui->cbShareMountPoint->setEnabled( false ) ;
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
	m_ui->checkBoxVisibleKey->setEnabled( false ) ;
	m_ui->lineEditVolumeProperty->setEnabled( false ) ;
	m_ui->labelVolumeProperty->setEnabled( false ) ;
	m_ui->pushButtonLuksExternalHeaderPath->setEnabled( false ) ;
}

void passwordDialog::enableAll()
{
	auto index = m_ui->cbVolumeType->currentIndex() ;

	if( index == 0 ){

		m_ui->pushButtonLuksExternalHeaderPath->setEnabled( true ) ;

	}

	m_ui->PushButtonOptions->setEnabled( true ) ;
	m_ui->lineEditVolumeProperty->setEnabled( true ) ;
	m_ui->labelVolumeProperty->setEnabled( true ) ;
	m_ui->pushButtonPlainDmCryptOptions->setEnabled( index == 3 ) ;
	m_ui->cbVolumeType->setEnabled( true ) ;
	m_ui->labelVolumeType->setEnabled( true ) ;
	m_ui->cbShareMountPoint->setEnabled( true ) ;
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

	auto m = m_ui->cbKeyType->currentIndex() ;

	if( m_open_with_path ){

		m_ui->OpenVolumePath->setEnabled( false ) ;
		m_ui->PushButtonVolumePath->setEnabled( false ) ;
	}

	if( m == passwordDialog::key || m == passwordDialog::yubikey ){

		m_ui->checkBoxVisibleKey->setEnabled( true ) ;
		m_ui->pushButtonPassPhraseFromFile->setEnabled( false ) ;
		m_ui->PassPhraseField->setEnabled( true ) ;
	}else{
		m_ui->checkBoxVisibleKey->setEnabled( false ) ;
		m_ui->pbKeyOption->setEnabled( true ) ;
		m_ui->PassPhraseField->setEnabled( false ) ;
	}
}

void passwordDialog::openVolume( const QString& key )
{
	this->disableAll() ;

	m_key = key.toLatin1() ;

	m_device = utility::resolvePath( m_ui->OpenVolumePath->text() ) ;

	m_point = m_ui->MountPointPath->text() ;

	if( m_point.isEmpty() || m_device.isEmpty() ){

		m_label.show( tr( "Atleast one required field is empty" ) ) ;

		return this->enableAll() ;
	}

	if( m_point.contains( "/" ) ){

		m_label.show( tr( "\"/\" character is not allowed in mount name field" ) ) ;

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

	if( keySource == passwordDialog::yubikey ){

		auto m = utility::yubiKey( m_ui->PassPhraseField->text() ) ;

		if( m.has_value() ){

			m_key = m.value() ;

			passtype = "-f" ;
			keyPath = utility::keyPath() ;

			this->sendKey( keyPath ) ;
		}else{
			DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Failed To Locate Or Run Yubikey's \"ykchalresp\" Program." ) ) ;
			return this->enableAll() ;
		}

	}else if( keySource == passwordDialog::keyfile ){

		if( m_key.isEmpty() ){

			m_label.show( tr( "Atleast one required field is empty" ) ) ;

			return this->enableAll() ;
		}else{
			passtype = "-f" ;
			keyPath = utility::resolvePath( m_key ).replace( "\"","\"\"\"" ) ;
		}
	}else if( keySource == passwordDialog::key ||
		  keySource == passwordDialog::keyKeyFile ){

		passtype = "-f" ;
		keyPath = utility::keyPath() ;

		this->sendKey( keyPath ) ;

	}else if( keySource == passwordDialog::plugin ){

		if( m_key.isEmpty() ){

			m_label.show( tr( "Atleast one required field is empty" ) ) ;

			return this->enableAll() ;
		}else{
			auto r = m_ui->PassPhraseField->text() ;

			if( utility::equalsAtleastOne( r,"hmac","gpg","keykeyfile" ) ){

				if( utility::pluginKey( m_secrets.parent(),&m_key,r ) ){

					return this->enableAll() ;
				}else{
					passtype = "-f" ;
					keyPath = utility::keyPath() ;
					this->sendKey( keyPath ) ;
				}

			}else if( r == "network" ){

				auto e = utility::Task::makePath( m_device ) ;

				auto z = QString( "%1 -i -d %2" ).arg( ZULUCRYPTzuluCrypt,e ) ;

				auto& s = utility::Task::run( z ) ;

				auto q = utility::split( s.await().stdOut() ) ;

				if( q.size() < 2 ){

					m_label.show( tr( "Volume is not a LUKS volume" ) ) ;

					m_ui->OpenVolumePath->setFocus() ;

					return this->enableAll() ;
				}else{
					this->disableAll() ;

					auto s =  utility::getKeyFromNetwork( q.at( 1 ) ) ;

					if( s.first ){

						m_key = s.second ;

						passtype = "-f" ;
						keyPath = utility::keyPath() ;

						this->sendKey( keyPath ) ;
					}else{
						m_label.show( tr( "Failed to get a key from the network" ) ) ;

						m_ui->OpenVolumePath->setFocus() ;
						return this->enableAll() ;
					}
				}
			}else{
				auto env = QProcessEnvironment::systemEnvironment() ;

				env.insert( "zuluCryptPrintToStdOut","true" ) ;

				m_key = Task::process::run( ZULUCRYPTpluginPath + r,{},-1,"",env ).await().std_out() ;

				if( m_key.isEmpty() ){

					m_label.show( tr( "Failed to get a key from a plugin" ) ) ;

					m_ui->OpenVolumePath->setFocus() ;
					return this->enableAll() ;
				}else{
					passtype = "-f" ;
					keyPath = utility::keyPath() ;
					this->sendKey( keyPath ) ;
				}
			}
		}
	}else if( keySource == passwordDialog::tcryptKeys ){

		passtype = "-f" ;
		keyPath = utility::keyPath() ;

		this->sendKey( keyPath ) ;
	}else{
		utility::debug() << "Error: uncaught condition" ;
	}

	QString a = ZULUCRYPTzuluCrypt ;
	QString b = m_device ;
	b.replace( "\"","\"\"\"" ) ;
	QString c = m_point ;
	c.replace( "\"","\"\"\"" ) ;
	const QString& d = mode ;
	const QString& e = passtype ;
	const QString& f = keyPath ;

	if( !b.startsWith( "/dev/" ) ){

		auto s = utility::loopDevicePath( b ) ;

		if( !s.isEmpty() ){

			b = s ;
		}
	}

	auto exe = QString( "%1 -o -d \"%2\" -m \"%3\" -e %4 %5 \"%6\"" ).arg( a,b,c,d,e,f ) ;

	if( m_ui->cbVolumeType->currentIndex() == 0 ){

		auto s = m_ui->lineEditVolumeProperty->text() ;

		if( !s.isEmpty() ){

			exe += " -z " + utility::Task::makePath( s ) ;
		}
	}

	if( !m_keyFiles.isEmpty() ){

		for( const auto& it : m_keyFiles ){

			QString e = it ;
			e.replace( "\"","\"\"\"" ) ;

			exe += " -F \"" + e + "\"" ;
		}
	}

	if( m_ui->cbVolumeType->currentIndex() == 1 ){

		auto e = m_ui->lineEditVolumeProperty->text() ;

		if( e.isEmpty() ){

			exe += " -t vcrypt" ;
		}else {
			exe += " -t vcrypt." + e ;
		}

	}else if( m_ui->cbVolumeType->currentIndex() == 2 ){

		auto e = m_ui->lineEditVolumeProperty->text() ;

		if( e.isEmpty() ){

			exe += " -t vcrypt-sys" ;
		}else {
			exe += " -t vcrypt-sys." + e ;
		}

	}else if( m_ui->cbVolumeType->currentIndex() == 3 ){

		auto e = m_ui->lineEditVolumeProperty->text() ;

		if( e.isEmpty() ){

			exe += " -t " + m_plainDmCryptProperty + ".0" ;
		}else {
			exe += " -t " + m_plainDmCryptProperty + "." + e ;
		}
	}

	if( m_ui->cbShareMountPoint->isChecked() ){

		exe += " -M" ;
	}

	utility::setFileSystemOptions( exe,m_device,m_point,m_fsOptions ) ;

	this->disableAll() ;

	m_veraCryptWarning.show( m_veraCryptVolume ) ;

	m_working = true ;

	auto r = utility::Task::run( utility::appendUserUID( exe ) ).await() ;

	m_working = false ;

	m_veraCryptWarning.stopTimer() ;

	if( r.success() ){

		m_openFolder( utility::mountPath( m_point ) ) ;

		this->HideUI() ;
	}else{
		this->failed( r ) ;

		m_veraCryptWarning.hide() ;
	}
}

void passwordDialog::failed( const utility::Task& e )
{
	int r = e.exitCode() ;

	if( r == 12 && m_ui->cbKeyType->currentIndex() == passwordDialog::plugin ){
		/*
		 * A user cancelled the plugin
		 */
		return this->enableAll() ;
	}


	switch ( r ){
		case 0 : return ;
		case 1 : m_label.show( tr( "Failed to mount ntfs/exfat file system using ntfs-3g,is ntfs-3g/exfat package installed?" ) ) ; break ;
		case 2 : m_label.show( tr( "There seem to be an open volume accociated with given address" ) ) ;				break ;
		case 3 : m_label.show( tr( "No file or device exist on given path" ) ) ; 						break ;
		case 4 : m_label.show( tr( "Volume could not be opened with the presented key" ) ) ;					break ;
		case 5 : m_label.show( tr( "Insufficient privilege to mount the device with given options" ) ) ;				break ;
		case 6 : m_label.show( tr( "Insufficient privilege to open device in read write mode or device does not exist" ) ) ;	break ;
		case 7 : m_label.show( tr( "Only root user can perform this operation" ) ) ;						break ;
		case 8 : m_label.show( tr( "-O and -m options can not be used together" ) ) ;						break ;
		case 9 : m_label.show( tr( "Could not create mount point, invalid path or path already taken" ) ) ;			break ;
		case 10: m_label.show( tr( "Shared mount point path already taken" ) ) ;							break ;
		case 11: m_label.show( tr( "There seem to be an opened mapper associated with the device" ) ) ;				break ;
		case 12: m_label.show( tr( "Could not get a passphrase from the module" ) ) ;						break ;
		case 13: m_label.show( tr( "Could not get passphrase in silent mode" ) ) ;						break ;
		case 14: m_label.show( tr( "Insufficient memory to hold passphrase" ) ) ;							break ;
		case 15: m_label.show( tr( "One or more required argument(s) for this operation is missing" ) ) ;				break ;
		case 16: m_label.show( tr( "Invalid path to key file" ) ) ;								break ;
		case 17: m_label.show( tr( "Could not get enought memory to hold the key file" ) ) ;					break ;
		case 18: m_label.show( tr( "Insufficient privilege to open key file for reading" ) ) ;					break ;
		case 19: m_label.show( tr( "Could not get a passphrase through a local socket" ) ) ;					break ;
		case 20: m_label.show( tr( "Failed to mount a filesystem:invalid/unsupported mount option or unsupported file system encountered" ) ) ;	break ;
		case 21: m_label.show( tr( "Could not create a lock on /etc/mtab" ) ) ;							break ;
		case 22: m_label.show( tr( "Insufficient privilege to open a system volume.\n\nConsult menu->help->permission for more informaion\n" ) ) ;					break ;
		case 113:m_label.show( tr( "A non supported device encountered,device is missing or permission denied\n\
Possible reasons for getting the error are:\n1.Device path is invalid.\n2.The device has LVM or MDRAID signature" ) ) ;					break ;
		default: m_label.show( tr( "Error Code: %1\n--\nStdOut: %2\n--\nStdError: %3").arg( QString::number( e.exitCode() ),QString( e.stdError() ),QString( e.stdOut() ) ) ) ;
	}

	utility::debug() << e.stdOut() << "--" << e.stdError() ;

	this->enableAll() ;

	auto index = m_ui->cbKeyType->currentIndex() ;

	if( utility:: clearPassword() && ( index == passwordDialog::key || index == passwordDialog::yubikey ) ){

		m_ui->PassPhraseField->clear() ;
		m_ui->PassPhraseField->setFocus() ;
	}

	if( r == 4 ){

		if( index == passwordDialog::key || index == passwordDialog::yubikey ){

			m_ui->PassPhraseField->setFocus() ;

		}else if( index == passwordDialog::keyKeyFile ){

			m_ui->cbKeyType->setCurrentIndex( 0 ) ;
			this->passphraseOption() ;
		}
	}
}

passwordDialog::~passwordDialog()
{
	m_pluginMenu->deleteLater() ;
	delete m_ui ;
}

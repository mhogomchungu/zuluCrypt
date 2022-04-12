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

#include "ui_luksaddkey.h"
#include "luksaddkey.h"
#include "../zuluCrypt-cli/constants.h"
#include "zulucrypt.h"
#include "utility.h"

#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <QProcess>
#include <QIcon>
#include <QDebug>

#include <QCloseEvent>

#include "openvolume.h"
#include "task.hpp"
#include "dialogmsg.h"
#include "plugin.h"
#include "utility.h"

luksaddkey::luksaddkey( QWidget * parent ) : QDialog( parent )
{
	m_ui = new Ui::luksaddkey() ;
	m_ui->setupUi( this ) ;

	m_label.setOptions( m_ui->label_5,m_ui->pushButton ) ;

	m_isWindowClosable = true ;

	m_ui->textEditPathToVolume->setText( QString() ) ;

	m_ui->textEditExistingPassphrase->setEchoMode( QLineEdit::Password ) ;
	m_ui->textEditPassphraseToAdd->setEchoMode( QLineEdit::Password ) ;

	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_ui->lineEditPIM->setEnabled( false ) ;
	m_ui->label_4->setEnabled( false ) ;

	m_ui->groupBoxLUKS2Options->setVisible( false ) ;
	m_ui->labelAdvanceLuks2Options->setVisible( false ) ;

	connect( m_ui->pbLuks2Set,&QPushButton::clicked,[ this ](){

		m_ui->groupBoxLUKS2Options->setVisible( false ) ;
		m_ui->labelAdvanceLuks2Options->setVisible( false ) ;
	} ) ;

	connect( m_ui->pushButtonLuks2AdvancedOptions,&QPushButton::clicked,[ this ](){

		m_showingLuks2AdvanceOptions = true ;
		m_ui->groupBoxLUKS2Options->setVisible( true ) ;
		m_ui->labelAdvanceLuks2Options->setVisible( true ) ;
	} ) ;

	connect( m_ui->pbLuks2Cancel,&QPushButton::clicked,[ this ](){

		this->luks2Cancel() ;
	} ) ;

	auto m = static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ) ;

	connect( m_ui->cbLuks2Pbkdf,m,[ this ]( int s ){

		if( s == 2 ){

			m_ui->lineEditLuks2MaxMemory->clear() ;
			m_ui->lineEditLuks2ParallelThreads->clear() ;

			m_ui->lineEditLuks2MaxMemory->setEnabled( false ) ;
			m_ui->lineEditLuks2ParallelThreads->setEnabled( false ) ;
		}else{
			m_ui->lineEditLuks2MaxMemory->setEnabled( true ) ;
			m_ui->lineEditLuks2ParallelThreads->setEnabled( true ) ;
		}
	} ) ;

	connect( m_ui->cbVolumeType,SIGNAL( activated( int ) ),this,SLOT( cbVolumeType( int ) ) ) ;
	connect( m_ui->pushButtonOpenFile,SIGNAL( clicked() ),this,SLOT( pbOpenFile() ) ) ;
	connect( m_ui->pushButtonOpenExistingKeyFile,SIGNAL( clicked() ),this,SLOT( pbOpenExisitingKeyFile() ) ) ;
	connect( m_ui->pushButtonOpenNewKeyFile,SIGNAL( clicked() ),this,SLOT( pbOpenNewKeyFile() ) ) ;
	connect( m_ui->pushButtonOpenPartition,SIGNAL( clicked() ),this,SLOT( pbOpenPartition( void ) ) ) ;
	connect( m_ui->pushButtonAdd,SIGNAL( clicked() ),this,SLOT( pbAdd() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	if( m_ui->cbNewKey->currentIndex() == 0 && m_keystrength.canCheckQuality() ){

		connect( m_ui->textEditPassphraseToAdd,SIGNAL( textChanged( QString ) ),this,SLOT( keyChanged_0( QString ) ) ) ;
	}else{
		connect( m_ui->textEditPassphraseToAdd,SIGNAL( textChanged( QString ) ),this,SLOT( keyChanged_1( QString ) ) ) ;
	}

	connect( m_ui->cbExistingKey,SIGNAL( activated( int ) ),this,SLOT( cbExistingKey( int ) ) ) ;
	connect( m_ui->cbNewKey,SIGNAL( activated( int ) ),this,SLOT( cbNewKey( int ) ) ) ;

	m_ui->lineEditReEnterPassphrase->setEchoMode( QLineEdit::Password ) ;

	m_ui->pushButtonOpenExistingKeyFile->setEnabled( false ) ;
	m_ui->pushButtonOpenNewKeyFile->setEnabled( false ) ;

	m_ui->pushButtonOpenPartition->setIcon( QIcon( ":/partition.png" ) ) ;
	m_ui->pushButtonOpenFile->setIcon( QIcon( ":/file.png" ) ) ;

	this->cbExistingKey( 0 ) ;
	this->cbNewKey( 0 ) ;
	this->cbVolumeType( 0 ) ;

	m_veraCryptWarning.setWarningLabel( m_ui->label_5 ) ;

	this->installEventFilter( this ) ;
}

bool luksaddkey::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void luksaddkey::keyChanged_1( QString key )
{
	Q_UNUSED( key )
	this->setWindowTitle( tr( "Add A Key To A Volume" ) ) ;
}

void luksaddkey::keyChanged_0( QString key )
{
	auto st = m_keystrength.quality( key ) ;

	if( st < 0 ){

		this->setWindowTitle( tr( "Passphrase Quality: 0%" ) ) ;
	}else{
		this->setWindowTitle( tr( "Passphrase Quality: %1%" ).arg( QString::number( st ) ) ) ;
	}
}

void luksaddkey::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	if( m_isWindowClosable ){

		this->HideUI() ;
	}
}

void luksaddkey::ShowUI( const QString& path )
{
	m_ui->textEditPathToVolume->setText( path ) ;
	this->ShowUI() ;
}

void luksaddkey::ShowUI( const QString& path,const QString& slot,std::function< void() > onExit )
{
	m_ui->textEditPathToVolume->setText( path ) ;
	m_ui->lineEditPIM->setText( slot ) ;
	m_onExit = std::move( onExit ) ;

	this->ShowUI() ;
}

void luksaddkey::ShowUI()
{
	this->setFieldFocus() ;
	this->show() ;
}

void luksaddkey::setFieldFocus()
{
	if( m_ui->textEditPathToVolume->text().isEmpty() ){

		m_ui->textEditPathToVolume->setFocus() ;

	}else if( m_ui->textEditExistingPassphrase->text().isEmpty() ){

		m_ui->textEditExistingPassphrase->setFocus() ;

	}else if( m_ui->textEditPassphraseToAdd->text().isEmpty() ){

		m_ui->textEditPassphraseToAdd->setFocus() ;

	}else if( m_ui->cbNewKey->currentIndex() == 0 ){

		if( m_ui->lineEditReEnterPassphrase->text().isEmpty() ){

			m_ui->lineEditReEnterPassphrase->setFocus() ;
		}else{
			m_ui->pushButtonAdd->setFocus() ;
		}
	}else{
		m_ui->pushButtonAdd->setFocus() ;
	}
}

void luksaddkey::HideUI()
{
	this->hide() ;
	m_onExit() ;
	this->deleteLater() ;
}

void luksaddkey::cbVolumeType( int e )
{
	if( e == 0 ){
		/*
		 * LUKS volume
		 */
		m_ui->lineEditPIM->setEnabled( false ) ;
		m_ui->label_4->setEnabled( false ) ;
		m_ui->lineEditPIM->setEnabled( false ) ;
		m_ui->lineEditPIM->setEchoMode( QLineEdit::Normal ) ;
		m_ui->pushButtonLuks2AdvancedOptions->setEnabled( true ) ;

	}else if( e == 1 ){
		/*
		 * TrueCrypt
		 */
		m_ui->lineEditPIM->setEnabled( false ) ;
		m_ui->label_4->setEnabled( false ) ;
		m_ui->lineEditPIM->setEnabled( false ) ;
		m_ui->pushButtonLuks2AdvancedOptions->setEnabled( false ) ;
	}else{
		/*
		 * VeraCrypt
		 */
		m_ui->lineEditPIM->setEnabled( true ) ;
		m_ui->label_4->setEnabled( true ) ;
		m_ui->lineEditPIM->setEnabled( true ) ;
		m_ui->lineEditPIM->setEchoMode( QLineEdit::Password ) ;
		m_ui->label_4->setText( tr( "PIM" ) ) ;
		m_ui->pushButtonLuks2AdvancedOptions->setEnabled( false ) ;
	}
}

void luksaddkey::cbExistingKey( int e )
{
	m_yubikeyExistingKey = false ;

	auto _key_ui = [ this ](){

		m_ui->textEditExistingPassphrase->setToolTip( tr( "Enter A Key" ) ) ;
		m_ui->textEditExistingPassphrase->setEchoMode( QLineEdit::Password ) ;
		m_ui->pushButtonOpenExistingKeyFile->setEnabled( false ) ;
		m_ui->labelExistingPassphrase->setText( tr( "Password" ) ) ;
		m_ui->textEditExistingPassphrase->clear() ;
		m_ui->pushButtonOpenExistingKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;
		m_ui->textEditExistingPassphrase->setFocus() ;
		m_ui->textEditExistingPassphrase->setEnabled( true ) ;
	} ;

	if( e == 0 ){

		_key_ui() ;

	}else if( e == 1 ){

		m_ui->textEditExistingPassphrase->setToolTip( tr( "Enter a path to a keyfile location" ) ) ;
		m_ui->textEditExistingPassphrase->setEchoMode( QLineEdit::Normal ) ;
		m_ui->pushButtonOpenExistingKeyFile->setEnabled( true ) ;
		m_ui->labelExistingPassphrase->setText( tr( "KeyFile Path" ) ) ;
		m_ui->textEditExistingPassphrase->clear() ;
		m_ui->pushButtonOpenExistingKeyFile->setIcon( QIcon( ":/keyfile.png" ) ) ;
		m_ui->textEditExistingPassphrase->setFocus() ;
		m_ui->textEditExistingPassphrase->setEnabled( true ) ;

	}else if( e == 2 ){

		_key_ui() ;

		m_ui->textEditExistingPassphrase->setEnabled( false ) ;

		plugin::instance( this,plugins::plugin::hmac_key,[ this ]( const QString& key ){

			m_ui->textEditExistingPassphrase->setText( key ) ;

			if( key.isEmpty() ){

				m_ui->cbExistingKey->setCurrentIndex( 0 ) ;
				this->cbExistingKey( 0 ) ;
			}
		} ) ;
	}else{
		_key_ui() ;
		m_yubikeyExistingKey = true ;
	}
}

void luksaddkey::cbNewKey( int e )
{
	m_yubekeyNewKey = false ;

	auto _key_ui = [ this ](){

		m_ui->textEditPassphraseToAdd->setToolTip( tr( "Enter a key" ) ) ;
		m_ui->textEditPassphraseToAdd->setEchoMode( QLineEdit::Password ) ;
		m_ui->pushButtonOpenNewKeyFile->setEnabled( false ) ;
		m_ui->labelNewPassphrase->setText( tr( "Password" ) ) ;
		m_ui->textEditPassphraseToAdd->clear() ;
		m_ui->lineEditReEnterPassphrase->clear() ;
		m_ui->lineEditReEnterPassphrase->setEnabled( true ) ;
		m_ui->labelReEnterPassphrase->setEnabled( true ) ;
		m_ui->pushButtonOpenNewKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;
		m_ui->textEditPassphraseToAdd->setFocus() ;
		m_ui->textEditPassphraseToAdd->setEnabled( true ) ;
	} ;

	if( e == 0 ){

		_key_ui() ;

	}else if( e == 1 ){

		m_ui->textEditPassphraseToAdd->clear() ;
		m_ui->lineEditReEnterPassphrase->clear() ;
		m_ui->textEditPassphraseToAdd->setToolTip( tr( "Enter a path to a keyfile location" ) ) ;
		m_ui->textEditPassphraseToAdd->setEchoMode( QLineEdit::Normal ) ;
		m_ui->pushButtonOpenNewKeyFile->setEnabled( true ) ;
		m_ui->labelNewPassphrase->setText( tr( "KeyFile path" ) ) ;
		m_ui->lineEditReEnterPassphrase->setEnabled( false ) ;
		m_ui->labelReEnterPassphrase->setEnabled( false ) ;
		m_ui->pushButtonOpenNewKeyFile->setIcon( QIcon( ":/keyfile.png" ) ) ;
		m_ui->textEditPassphraseToAdd->setFocus() ;
		m_ui->textEditPassphraseToAdd->setEnabled( true ) ;

	}else if( e == 2 ){

		_key_ui() ;

		m_ui->textEditPassphraseToAdd->setEnabled( false ) ;
		m_ui->lineEditReEnterPassphrase->setEnabled( false ) ;

		plugin::instance( this,plugins::plugin::hmac_key,[ this ]( const QString& key ){

			m_ui->textEditPassphraseToAdd->setText( key ) ;
			m_ui->lineEditReEnterPassphrase->setText( key ) ;

			if( key.isEmpty() ){

				m_ui->cbNewKey->setCurrentIndex( 0 ) ;
				this->cbNewKey( 0 ) ;
			}else{
				if( m_keystrength.canCheckQuality() ){

					this->setWindowTitle( tr( "Passphrase Quality: 100%" ) ) ;
				}
			}
		} ) ;
	}else{
		m_yubekeyNewKey = true ;
		m_ui->lineEditReEnterPassphrase->setEnabled( false ) ;
		m_ui->lineEditReEnterPassphrase->clear() ;
		m_ui->textEditPassphraseToAdd->clear() ;
		m_ui->textEditPassphraseToAdd->setEchoMode( QLineEdit::Password ) ;
	}
}

void luksaddkey::pbOpenExisitingKeyFile( void )
{
	auto Z = QFileDialog::getOpenFileName( this,tr( "Existing KeyFile" ),utility::homePath() ) ;

	if( !Z.isEmpty() ){

		m_ui->textEditExistingPassphrase->setText( Z ) ;
	}

	this->setFieldFocus() ;
}

void luksaddkey::pbOpenNewKeyFile( void )
{
	auto Z = QFileDialog::getOpenFileName( this,tr( "New KeyFile" ),utility::homePath() ) ;

	if( !Z.isEmpty() ){

		m_ui->textEditPassphraseToAdd->setText( Z ) ;
	}

	this->setFieldFocus() ;
}

void luksaddkey::pbOpenFile( void )
{
	auto Z = QFileDialog::getOpenFileName( this,tr( "Encrypted Volume Path" ),utility::homePath() ) ;

	if( !Z.isEmpty() ){

		m_ui->textEditPathToVolume->setText( Z ) ;
	}

	this->setFieldFocus() ;
}

void luksaddkey::pbOpenPartition( void )
{
	openvolume::instance( this,false ).showLuksOnly().ShowAllPartitions( [ this ]( const QString& e ){

		this->setFieldFocus() ;
		this->ShowUI( e ) ;
	} ) ;
}

void luksaddkey::pbAdd( void )
{
	DialogMsg msg( this ) ;

	std::function< void() > sendNewKey = [](){} ;
	std::function< void() > sendExistingKey = [](){} ;

	this->disableAll() ;

	utility2::raii raii( [ this ](){ this->enableAll() ; } ) ;

	auto ExistingKey = m_ui->textEditExistingPassphrase->text() ;

	auto NewKey = m_ui->textEditPassphraseToAdd->text() ;
	auto NewKey_1 = m_ui->lineEditReEnterPassphrase->text() ;

	m_volumePath = m_ui->textEditPathToVolume->text() ;

	auto existingKeyIsKeyFile = m_ui->cbExistingKey->currentIndex() == 1 ;
	auto newKeyIsKeyFile = m_ui->cbNewKey->currentIndex() == 1 ;

	if( m_volumePath.isEmpty() ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
	}

	m_volumePath = utility::resolvePath( m_volumePath ) ;

	if( utility::requireSystemPermissions( m_volumePath ) ){

		if( !utility::enablePolkit( utility::background_thread::False ) ){

			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to enable polkit support" ) ) ;
		}
	}

	if( existingKeyIsKeyFile ){

		if( ExistingKey.isEmpty() ){

			return msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
		}
	}

	if( newKeyIsKeyFile ){

		if( NewKey.isEmpty() ){

			return msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
		}
	}else{
		if( !m_yubekeyNewKey ){

			if( NewKey != NewKey_1 ){

				msg.ShowUIOK( tr( "ERROR!" ),tr( "Keys do not match" ) ) ;

				m_ui->textEditPassphraseToAdd->clear() ;
				m_ui->lineEditReEnterPassphrase->clear() ;
				m_ui->textEditPassphraseToAdd->setFocus() ;

				return ;
			}
		}
	}

	QString existingPassType ;

	if( existingKeyIsKeyFile ){

		ExistingKey = utility::resolvePath( ExistingKey ).replace( "\"","\"\"\"" ) ;
		existingPassType = "-u" ;
	}else{
		auto k = m_ui->textEditExistingPassphrase->text() ;

		if( m_yubikeyExistingKey ){

			auto m = utility::yubiKey( k ) ;

			if( m.has_value() ){

				k = m.value() ;
			}else{
				this->cbExistingKey( 0 ) ;
				return m_label.show( tr( "Failed To Locate Or Run Yubikey's \"ykchalresp\" Program." ) ) ;
			}
		}

		existingPassType = "-u" ;
		ExistingKey = utility::keyPath() + "-existingKey" ;

		sendExistingKey = [ = ](){

			utility::keySend( ExistingKey,k ) ;
		} ;
	}

	QString newPassType ;

	if( newKeyIsKeyFile ){

		NewKey = utility::resolvePath( NewKey ).replace( "\"","\"\"\"" ) ;
		newPassType = "-n" ;
	}else{
		auto k = m_ui->textEditPassphraseToAdd->text() ;

		if( m_yubekeyNewKey ){

			auto m = utility::yubiKey( k ) ;

			if( m.has_value() ){

				k = m.value() ;
			}else{
				this->cbNewKey( 0 ) ;
				return m_label.show( tr( "Failed To Locate Or Run Yubikey's \"ykchalresp\" Program." ) ) ;
			}
		}

		newPassType = "-n" ;

		NewKey = utility::keyPath() + "-newKey" ;

		sendNewKey = [ = ](){

			utility::keySend( NewKey,k ) ;
		} ;
	}

	const QString& a = QString( ZULUCRYPTzuluCrypt ) ;

	QString b = m_volumePath ;

	b.replace( "\"","\"\"\"" ) ;

	const QString& c = existingPassType ;
	const QString& d = ExistingKey ;
	const QString& e = newPassType ;
	const QString& f = NewKey ;

	auto r = "%1 -a -d \"%2\" %3 \"%4\" %5 \"%6\"" ;

	auto exe = utility::appendUserUID( r ).arg( a,b,c,d,e,f ) ;

	if( m_ui->cbVolumeType->currentIndex() == 2 ){

		exe += " -t vcrypt" ;

		auto e = m_ui->lineEditPIM->text() ;

		if( !e.isEmpty() ){

			exe += "." + e ;
		}

	}else if( m_ui->cbVolumeType->currentIndex() == 0 ){

		auto forcedIterations = m_ui->lineEditLuks2ForcedIteration->text() ;
		auto unlockingTime    = m_ui->lineEditLuks2UnlockingTime->text() ;
		auto maxThreads       = m_ui->lineEditLuks2ParallelThreads->text() ;
		auto maxMemory        = m_ui->lineEditLuks2MaxMemory->text() ;
		auto keyslot          = m_ui->lineEditLuks2KeySlot->text() ;
		auto pbkdf            = m_ui->cbLuks2Pbkdf->currentText() ;

		if( unlockingTime.isEmpty() ){

			unlockingTime = "-1" ;
		}

		if( forcedIterations.isEmpty() ){

			forcedIterations = "-1" ;
		}

		if( maxThreads.isEmpty() ){

			maxThreads = "-1" ;
		}

		if( maxMemory.isEmpty() ){

			maxMemory = "-1" ;
		}

		if( keyslot.isEmpty() ){

			keyslot = "-1" ;
		}

		QString opts = "%1.%2.%3.%4.%5.%6" ;

		exe += " -g " + opts.arg( unlockingTime,forcedIterations,pbkdf,maxMemory,maxThreads,keyslot) ;
	}

	m_isWindowClosable = false ;

	m_veraCryptWarning.show( m_ui->cbVolumeType->currentIndex() == 2 ) ;

	raii.cancel() ;

	sendNewKey() ;
	sendExistingKey() ;

	this->taskFinished( utility::Task::run( exe ).await() ) ;
}

void luksaddkey::keyAdded()
{
	m_volumePath.replace( "\"","\"\"\"" ) ;

	auto l = utility::luksEmptySlots( m_volumePath ).await() ;

	QString success ;

	if( l.isEmpty() ){

		m_label.show( tr( "Key added successfully." ) ) ;
	}else{
		QString x = tr( "Key added successfully.\n%1 / %2 slots are now in use" ) ;
		m_label.show( x.arg( l.first() ).arg( l.at( 1 ) ) ) ;
	}

	this->HideUI() ;
}

void luksaddkey::taskFinished( const utility::Task& e )
{
	m_veraCryptWarning.stopTimer() ;

	m_isWindowClosable = true ;

	switch( e.exitCode() ){
		case 0  : return this->keyAdded() ;
		case 1  : m_label.show( tr( "Presented key does not match any key in the volume" ) ) ;		      	break ;
		case 2  : m_label.show( tr( "Could not open luks volume" ) ) ;					     	break ;
		case 3  : m_label.show( tr( "Volume is not a luks volume" ) ) ;					     	break ;
		case 4  : m_label.show( tr( "Insufficient privilege to add a key to a system device,\nonly root user or members of group \"zulucrypt\" can do that\n" ) )	;break ;
		case 5  : m_label.show( tr( "Could not open volume in write mode" ) ) ;					break ;
		case 6  : m_label.show( tr( "All key slots are occupied, can not add any more keys" ) ) ;		break ;
		case 7  : m_label.show( tr( "Can not get passphrase in silent mode" ) ) ;			   	break ;
		case 8  : m_label.show( tr( "Insufficient memory to hold passphrase" ) ) ;	                      	break ;
		case 9  : m_label.show( tr( "New passphrases do not match" ) ) ;						break ;
		case 10 : m_label.show( tr( "One or more required argument(s) for this operation is missing" ) ) ;      	break ;
		case 11 : m_label.show( tr( "One or both keyfile(s) does not exist" ) ) ;				break ;
		case 12 : m_label.show( tr( "Insufficient privilege to open key file for reading" ) ) ;			break ;
		case 13 : m_label.show( tr( "Couldnt get enought memory to hold the key file" ) ) ;			break ;
		case 14 : m_label.show( tr( "Could not get a key from a socket" ) ) ;					break ;
		case 15 : m_label.show( tr( "Could not get elevated privilege,check binary permissions" ) ) ;		break ;
		case 16 : m_label.show( tr( "Key slot already occupied" ) ) ;					break ;
		case 17 : m_label.show( tr( "Failed to find empty key slot or key slot out of range" ) ) ;		break ;
		case 110: m_label.show( tr( "Can not find a partition that match presented UUID" ) ) ;			break ;
		case 113: m_label.show( tr( "Device is not a luks device" ) ) ;						break ;
		default:  m_label.show( tr( "Error Code: %1\n--\nStdOut: %2\n--\nStdError: %3").arg( QString::number( e.exitCode() ),QString( e.stdError() ),QString( e.stdOut() ) ) ) ;
	}

	m_veraCryptWarning.hide() ;

	this->enableAll() ;
}

void luksaddkey::disableAll()
{
	m_ui->lineEditPIM->setEnabled( false ) ;
	m_ui->label_4->setEnabled( false ) ;
	m_ui->cbVolumeType->setEnabled( false ) ;
	m_ui->label_3->setEnabled( false ) ;
	m_ui->labelExistingPassphrase->setEnabled( false ) ;
	m_ui->labelLuksVolume->setEnabled( false ) ;
	m_ui->labelNewPassphrase->setEnabled( false ) ;
	m_ui->textEditExistingPassphrase->setEnabled( false ) ;
	m_ui->textEditPassphraseToAdd->setEnabled( false ) ;
	m_ui->textEditPathToVolume->setEnabled( false ) ;
	m_ui->lineEditReEnterPassphrase->setEnabled( false ) ;
	m_ui->labelNewPassphrase->setEnabled( false ) ;
	m_ui->lineEditReEnterPassphrase->setEnabled( false ) ;
	m_ui->pushButtonAdd->setEnabled( false ) ;
	m_ui->pushButtonCancel->setEnabled( false ) ;
	m_ui->pushButtonOpenExistingKeyFile->setEnabled( false ) ;
	m_ui->pushButtonOpenFile->setEnabled( false ) ;
	m_ui->pushButtonOpenNewKeyFile->setEnabled( false ) ;
	m_ui->pushButtonOpenPartition->setEnabled( false ) ;
	m_ui->labelReEnterPassphrase->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->cbNewKey->setEnabled( false ) ;
	m_ui->cbExistingKey->setEnabled( false ) ;
	m_ui->pushButtonLuks2AdvancedOptions->setEnabled( false ) ;
}

void luksaddkey::enableAll()
{
	auto index = m_ui->cbVolumeType->currentIndex() ;

	m_ui->pushButtonLuks2AdvancedOptions->setEnabled( index == 0 ) ;
	m_ui->label_4->setEnabled( index == 2 ) ;
	m_ui->lineEditPIM->setEnabled( index == 2 ) ;
	m_ui->cbVolumeType->setEnabled( true ) ;
	m_ui->label_3->setEnabled( true ) ;
	m_ui->labelReEnterPassphrase->setEnabled( true ) ;
	m_ui->labelExistingPassphrase->setEnabled( true ) ;
	m_ui->labelLuksVolume->setEnabled( true ) ;
	m_ui->labelNewPassphrase->setEnabled( true ) ;
	m_ui->textEditExistingPassphrase->setEnabled( true ) ;
	m_ui->textEditPassphraseToAdd->setEnabled( true ) ;
	m_ui->textEditPathToVolume->setEnabled( true ) ;
	m_ui->labelNewPassphrase->setEnabled( true ) ;
	m_ui->pushButtonAdd->setEnabled( true ) ;
	m_ui->pushButtonCancel->setEnabled( true ) ;
	m_ui->pushButtonOpenExistingKeyFile->setEnabled( true ) ;
	m_ui->pushButtonOpenFile->setEnabled( true ) ;
	m_ui->pushButtonOpenNewKeyFile->setEnabled( true ) ;
	m_ui->pushButtonOpenPartition->setEnabled( true ) ;
	m_ui->lineEditPIM->setEnabled( true ) ;
	m_ui->cbNewKey->setEnabled( true ) ;
	m_ui->cbExistingKey->setEnabled( true ) ;

	index = m_ui->cbNewKey->currentIndex() ;

	if( index == 0 ){

		m_ui->lineEditReEnterPassphrase->setEnabled( true ) ;

	}else if( index == 1 ){

		m_ui->lineEditReEnterPassphrase->setEnabled( false ) ;

	}else if( index == 2 ){

		m_ui->lineEditReEnterPassphrase->setEnabled( false ) ;
		m_ui->textEditPassphraseToAdd->setEnabled( false ) ;
	}

	m_ui->label->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
}

void luksaddkey::luks2Cancel()
{
	m_showingLuks2AdvanceOptions = false ;
	m_ui->groupBoxLUKS2Options->setVisible( false ) ;
	m_ui->labelAdvanceLuks2Options->setVisible( false ) ;

	m_ui->lineEditLuks2ForcedIteration->clear() ;
	m_ui->lineEditLuks2UnlockingTime->clear() ;
	m_ui->lineEditLuks2ParallelThreads->clear() ;
	m_ui->lineEditLuks2MaxMemory->clear() ;
	m_ui->lineEditLuks2KeySlot->clear() ;
	m_ui->cbLuks2Pbkdf->setCurrentIndex( 0 ) ;
}

void luksaddkey::pbCancel( void )
{
	this->HideUI() ;
}

luksaddkey::~luksaddkey()
{
	delete m_ui ;
}

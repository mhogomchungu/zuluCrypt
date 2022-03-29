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
#include "utility.h"
#include "createvolume.h"
#include "ui_createvolume.h"

#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <QAbstractButton>
#include <QProcess>
#include <QThread>

#include <QCloseEvent>
#include <QThreadPool>

#include "task.hpp"
#include "utility.h"
#include "erasedevice.h"
#include "createvolumedialog.h"
#include "dialogmsg.h"
#include "tcrypt.h"
#include <initializer_list>

#include "plugin.h"

#include <QDebug>
#include "../zuluCrypt-cli/constants.h"

createvolume::createvolume( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::createvolume )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_ui->lineEditVolumePath->setEnabled( false ) ;
	m_ui->lineEditPassphrase1->setFocus() ;

	m_isWindowClosable = true ;
	m_warned = false ;

	m_ui->lineEditIterationNumber->setVisible( false ) ;
	m_ui->labelIterationNumber->setVisible( false ) ;

	m_ui->groupBoxLUKS2Options->setVisible( false ) ;
	m_ui->labelLUKS2Options->setVisible( false ) ;

	connect( m_ui->pbLuks2Set,&QPushButton::clicked,[ this ](){

		m_ui->groupBoxLUKS2Options->setVisible( false ) ;
		m_ui->labelLUKS2Options->setVisible( false ) ;
	} ) ;

	connect( m_ui->pbLUKS2Options,&QPushButton::clicked,[ this ](){

		m_ui->groupBoxLUKS2Options->setVisible( true ) ;
		m_ui->labelLUKS2Options->setVisible( true ) ;
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

#ifdef CRYPT_LUKS2
	m_ui->cbLuks2AllowDiscard->setEnabled( true ) ;
#else
	m_ui->cbLuks2AllowDiscard->setEnabled( false ) ;
#endif
	connect( m_ui->pbLUKS2Options,SIGNAL( clicked() ),this,SLOT( luks2Options() ) ) ;
	connect( m_ui->pbOpenKeyFile,SIGNAL( clicked() ),this,SLOT( pbOpenKeyFile() ) ) ;
	connect( m_ui->pbCreate,SIGNAL( clicked() ),this,SLOT( pbCreateClicked() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancelClicked() ) ) ;
	connect( m_ui->cbNormalVolume,SIGNAL( activated( int ) ),this,SLOT( cbNormalVolume( int ) ) ) ;
	connect( m_ui->cbHiddenVolume,SIGNAL( activated( int ) ),this,SLOT( cbHiddenVolume( int ) ) ) ;
	connect( m_ui->comboBoxVolumeType,SIGNAL( currentIndexChanged( int ) ),this,SLOT( volumeType( int ) ) ) ;
	connect( m_ui->lineEditPassphrase1,SIGNAL( textChanged( QString ) ),this,SLOT( keyChanged_0( QString ) ) ) ;
	connect( m_ui->lineEditHiddenKey,SIGNAL( textChanged( QString ) ),this,SLOT( keyChanged_1( QString ) ) ) ;
	connect( m_ui->pbHiddenKeyFile,SIGNAL( clicked() ),this,SLOT( pbOpenHiddenKeyFile() ) ) ;
	connect( m_ui->comboBoxVolumeType,SIGNAL( activated( int ) ),this,SLOT( setOptions( int ) ) ) ;

        this->installEventFilter( this ) ;

	m_ui->labelvolumeOptions->setVisible( false ) ;

	auto a = tr( "Options are separated by a \".\" character.\n\n" ) ;
	auto b = tr( "Multiple algorithms are separated by \":\" character.\n\n" ) ;
	auto c = tr( "Options are in a format of \"algorithm.cipher mode.key size in bits.hash\"\n\n") ;
	auto d = tr( "Default option is the first entry on the list" ) ;

	m_ui->comboBoxOptions->setToolTip( a + b + c + d ) ;

	m_ui->cbNormalVolume->addItem( tr( "Key" ) ) ;
	m_ui->cbNormalVolume->addItem( tr( "KeyFile" ) ) ;
	m_ui->cbNormalVolume->addItem( tr( "Key+KeyFile" ) ) ;
	m_ui->cbNormalVolume->addItem( tr( "YubiKey Challenge/Response" ) ) ;

	m_ui->cbHiddenVolume->addItem( tr( "Key" ) ) ;
	m_ui->cbHiddenVolume->addItem( tr( "KeyFile" ) ) ;
	m_ui->cbHiddenVolume->addItem( tr( "Key+KeyFile" ) ) ;
	m_ui->cbHiddenVolume->addItem( tr( "YubiKey Challenge/Response" ) ) ;

	m_veraCryptWarning.setWarningLabel( m_ui->veraCryptWarning ) ;

	/*
	 * for simplicity's sake,lets only show most popular file systems.
	 */

	m_ui->comboBoxFS->addItems( utility::supportedFileSystems() ) ;

	m_ui->comboBoxVolumeType->clear() ;

	m_ui->comboBoxVolumeType->addItem( tr( "PLAIN dm-crypt" ) ) ;
	m_ui->comboBoxVolumeType->addItem( tr( "PLAIN dm-crypt with offset" ) ) ;
#ifdef CRYPT_LUKS2
	m_ui->comboBoxVolumeType->addItem( tr( "LUKS1" ) ) ;
	m_ui->comboBoxVolumeType->addItem( tr( "LUKS1+External Header" ) ) ;
	m_ui->comboBoxVolumeType->addItem( tr( "LUKS2" ) ) ;
	m_ui->comboBoxVolumeType->addItem( tr( "LUKS2+External Header" ) ) ;
#else
	m_ui->comboBoxVolumeType->addItem( tr( "LUKS" ) ) ;
	m_ui->comboBoxVolumeType->addItem( tr( "LUKS+External Header" ) ) ;
#endif
	m_ui->comboBoxVolumeType->addItem( tr( "Normal TrueCrypt" ) ) ;
	m_ui->comboBoxVolumeType->addItem( tr( "Normal+Hidden TrueCrypt" ) ) ;
	m_ui->comboBoxVolumeType->addItem( tr( "Normal VeraCrypt" ) ) ;
	m_ui->comboBoxVolumeType->addItem( tr( "Normal+Hidden VeraCrypt" ) ) ;
#ifdef CRYPT_LUKS2
	m_ui->comboBoxVolumeType->setCurrentIndex( int( createvolume::luks2 ) ) ;
#else
	m_ui->comboBoxVolumeType->setCurrentIndex( int( createvolume::luks ) ) ;
#endif
	m_ui->comboBoxHiddenSize->setCurrentIndex( 2 ) ;

	this->setOptions( 1 ) ;
}

void createvolume::keyChanged_0( QString key )
{
	if( m_keyStrength.canCheckQuality() ){

		if( m_ui->cbNormalVolume->currentIndex() == 0 ){

			return this->keyChanged( true,key ) ;
		}
	}

	this->keyChanged( false,QString() ) ;
}

void createvolume::keyChanged_1( QString key )
{
	if( m_keyStrength.canCheckQuality() ){

		if( m_ui->cbHiddenVolume->currentIndex() == 0 ){

			return this->keyChanged( true,key ) ;
		}
	}

	this->keyChanged( false,QString() ) ;
}

void createvolume::keyChanged( bool check,const QString& key )
{
	if( check ){

		int st = m_keyStrength.quality( key ) ;

		if( st < 0 ){

			this->setWindowTitle( tr( "Passphrase Quality: 0%" ) ) ;
		}else{
			this->setWindowTitle( tr( "Passphrase Quality: %1%" ).arg( QString::number( st ) ) ) ;
		}
	}else{
		this->setWindowTitle( tr( "Create A New Volume" ) ) ;
	}
}

bool createvolume::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void createvolume::volumeType( int s )
{
	m_ui->cbNormalVolume->clear() ;
	m_ui->cbHiddenVolume->clear() ;

	m_ui->cbNormalVolume->addItem( tr( "Key" ) ) ;
	m_ui->cbNormalVolume->addItem( tr( "KeyFile" ) ) ;
	m_ui->cbNormalVolume->addItem( tr( "Key+KeyFile" ) ) ;
	m_ui->cbNormalVolume->addItem( tr( "YubiKey Challenge/Response" ) ) ;

	m_ui->cbHiddenVolume->addItem( tr( "Key" ) ) ;
	m_ui->cbHiddenVolume->addItem( tr( "KeyFile" ) ) ;
	m_ui->cbHiddenVolume->addItem( tr( "Key+KeyFile" ) ) ;
	m_ui->cbHiddenVolume->addItem( tr( "YubiKey Challenge/Response" ) ) ;

	auto _enableHidden = [ this ](){

		m_ui->lineEditHiddenKey->setEnabled( true ) ;
		m_ui->lineEditHiddenKey1->setEnabled( true ) ;
		m_ui->lineEditHiddenSize->setEnabled( true ) ;
		m_ui->pbHiddenKeyFile->setEnabled( true ) ;
		m_ui->comboBoxHiddenSize->setEnabled( true ) ;
		m_ui->cbHiddenVolume->setEnabled( true ) ;

		m_ui->label->setEnabled( true ) ;
		m_ui->labelHidden->setEnabled( true ) ;
		m_ui->label_2Hidden->setEnabled( true ) ;
	} ;

	auto _disableHidden = [ this ](){

		m_ui->lineEditHiddenKey->clear() ;
		m_ui->lineEditHiddenKey1->clear() ;
		m_ui->lineEditHiddenSize->clear() ;

		m_ui->lineEditHiddenKey->setEnabled( false ) ;
		m_ui->lineEditHiddenKey1->setEnabled( false ) ;
		m_ui->pbHiddenKeyFile->setEnabled( false ) ;
		m_ui->comboBoxHiddenSize->setEnabled( false ) ;
		m_ui->cbHiddenVolume->setEnabled( false ) ;

		m_ui->labelHidden->setEnabled( false ) ;
		m_ui->label_2Hidden->setEnabled( false ) ;

		auto e = m_ui->comboBoxVolumeType->currentIndex() == createvolume::plain_with_offset ;

		m_ui->label->setEnabled( e ) ;
		m_ui->lineEditHiddenSize->setEnabled( e ) ;
		m_ui->comboBoxHiddenSize->setEnabled( e ) ;
	} ;

	auto type = createvolume::createVolumeType( s ) ;

	switch( type ){

	case createvolume::luks :
	case createvolume::luks_external_header :
#ifdef CRYPT_LUKS2
	case createvolume::luks2 :
	case createvolume::luks2_external_header :
#endif
		m_ui->comboBoxRNG->setEnabled( true ) ;

		m_ui->label_2->setEnabled( false ) ;
		m_ui->lineEditPIM->setEnabled( false ) ;

		_disableHidden() ;

		break ;
	case createvolume::plain :
	case createvolume::plain_with_offset :

		m_ui->comboBoxRNG->setEnabled( false ) ;

		m_ui->label_2->setEnabled( false ) ;
		m_ui->lineEditPIM->setEnabled( false ) ;

		_disableHidden() ;

		break ;
	case createvolume::normal_truecrypt :

		m_ui->comboBoxRNG->setEnabled( true ) ;
		m_ui->cbNormalVolume->addItem( tr( "TrueCrypt Keys" ) ) ;

		m_ui->label_2->setEnabled( false ) ;
		m_ui->lineEditPIM->setEnabled( false ) ;

		_disableHidden() ;

		break ;
	case createvolume::normal_veracrypt :

		m_ui->comboBoxRNG->setEnabled( true ) ;
		m_ui->cbNormalVolume->addItem( tr( "VeraCrypt Keys" ) ) ;

		m_ui->label_2->setEnabled( true ) ;
		m_ui->lineEditPIM->setEnabled( true ) ;

		_disableHidden() ;

		break ;
	case createvolume::normal_and_hidden_truecrypt :

		m_ui->comboBoxRNG->setEnabled( true ) ;
		m_ui->cbHiddenVolume->setCurrentIndex( 0 ) ;
		m_ui->cbHiddenVolume->addItem( tr( "TrueCrypt Keys" ) ) ;
		m_ui->cbNormalVolume->addItem( tr( "TrueCrypt Keys" ) ) ;

		m_ui->label_2->setEnabled( false ) ;
		m_ui->lineEditPIM->setEnabled( false ) ;

		this->cbHiddenVolume( 0 ) ;

		_enableHidden() ;

		break ;
	case createvolume::normal_and_hidden_veracrypt :

		m_ui->comboBoxRNG->setEnabled( true ) ;
		m_ui->cbHiddenVolume->setCurrentIndex( 0 ) ;
		m_ui->cbHiddenVolume->addItem( tr( "VeraCrypt Keys" ) ) ;
		m_ui->cbNormalVolume->addItem( tr( "VeraCrypt Keys" ) ) ;

		m_ui->label_2->setEnabled( true ) ;
		m_ui->lineEditPIM->setEnabled( true ) ;

		this->cbHiddenVolume( 0 ) ;

		_enableHidden() ;

		break ;
	}

	if( type == createvolume::plain_with_offset ){

		m_ui->label->setText( tr( "Volume Offset" ) ) ;
	}else{
		m_ui->label->setText( tr( "Volume Size" ) ) ;
	}
}

void createvolume::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	if( m_isWindowClosable ){

		this->pbCancelClicked() ;
	}
}

void createvolume::ShowPartition( QString volume )
{
	this->ShowUI( tr( "Path To Device" ),volume ) ;
}

void createvolume::ShowFile( QString volume )
{
	this->ShowUI( tr( "Path To File" ),volume ) ;
}

void createvolume::eraseDataPartition()
{
	auto path = m_ui->lineEditVolumePath->text() ;

	if( path.startsWith( "/dev/" ) ){

		createVolumeDialog::instance( path,this,[ this ]( int r ){

			if( r == 0 ){

				this->HideUI() ;

			}else if( r == 1 ){

			}else if( r == 2 ){

				erasedevice::instance( this ).ShowUI( m_ui->lineEditVolumePath->text() ) ;
			}
		} ) ;
	}
}

void createvolume::setOptions( int e )
{
	auto options = m_ui->comboBoxOptions ;

	options->clear() ;

	/*
	 * constructed structure format is: algorithm.cipher mode.key size in bits.hash function.
	 * examples structures :
	 *  "aes.cbc-essiv:256.256.ripemd160"
	 *  "aes.xts-plain64.256.sha1"
	 *  "serpent:twofish:aes.xts-plain64.256.sha1"
	 */

	/*
	 * we only support whirlpool if the user has libgcrypt >= 1.6.1 and cryptsetup >= 1.6.4
	 * read section 8.3 of the following link for more info:
	 * https://code.google.com/p/cryptsetup/wiki/FrequentlyAskedQuestions
	 *
	 */
	bool supportWhirlpool = utility::userHasGoodVersionOfWhirlpool() ;

	Q_UNUSED( e )

	auto _plain_dmcrypt = [ this ](){

		auto type = m_ui->comboBoxVolumeType->currentIndex() ;

		return type == createvolume::plain || type == createvolume::plain_with_offset ;
	} ;

	auto _luks = [ this ](){

		auto type = m_ui->comboBoxVolumeType->currentIndex() ;

		#ifdef CRYPT_LUKS2
			return	type == createvolume::luks                  ||
				type == createvolume::luks_external_header  ||
				type == createvolume::luks2_external_header ||
				type == createvolume::luks2 ;

		#else
			return type == createvolume::luks || type == createvolume::luks_external_header ;
		#endif
	} ;

	auto luksSelected = _luks() ;

#ifdef CRYPT_LUKS2
	auto type = m_ui->comboBoxVolumeType->currentIndex() ;
	m_ui->pbLUKS2Options->setEnabled( type == createvolume::luks2_external_header || type == createvolume::luks2 ) ;
#else
	m_ui->pbLUKS2Options->setEnabled( false ) ;
#endif
	if( _plain_dmcrypt() ){

		/*
		 * crypto options for plain dm-crypt volumes
		 */

		auto s = utility::plainDmCryptOptions() ;

		if( s.isEmpty() ){

			options->addItem( "aes.cbc-essiv:256.256.ripemd160" ) ;
		}else{
			options->addItems( s ) ;
		}

	}else if( luksSelected ){

		/*
		 * cryto options for LUKS volumes.
		 */
		auto _add_options = [ & ]( const std::initializer_list<QString>& list ){

			auto _add_option = [ & ]( const QString& algo,const QString& keySize ){

				options->addItem( algo + ".xts-plain64." + keySize + ".sha256" ) ;
				options->addItem( algo + ".xts-plain64." + keySize + ".sha512" ) ;
				options->addItem( algo + ".xts-plain64." + keySize + ".sha1" ) ;
				options->addItem( algo + ".xts-plain64." + keySize + ".ripemd160" ) ;

				if( supportWhirlpool ){

					options->addItem( algo + ".xts-plain64." + keySize + ".whirlpool" ) ;
				}
			} ;

			for( const auto& it : list ){

				_add_option( it,"512" ) ;
			}
			for( const auto& it : list ){

				_add_option( it,"256" ) ;
			}
		} ;

		_add_options( { "aes","serpent","twofish" } ) ;
	}else{
		/*
		 * crypto options for TrueCrypt and VeraCrypt volumes
		 */

		auto _add_options = [ & ]( const std::initializer_list<QString>& list ){

			auto _veraCryptVolume = [ & ](){

				using cv = createvolume ;

				auto e = cv::createVolumeType( m_ui->comboBoxVolumeType->currentIndex() ) ;

				return e == cv::normal_veracrypt || e == cv::normal_and_hidden_veracrypt ;
			} ;

			auto _add_option = [ & ]( const QString& algo ){

				if( _veraCryptVolume() ){

					options->addItem( algo + ".xts-plain64.256.sha512" ) ;
					options->addItem( algo + ".xts-plain64.256.ripemd160" ) ;
				}else{
					options->addItem( algo + ".xts-plain64.256.ripemd160" ) ;
					options->addItem( algo + ".xts-plain64.256.sha512" ) ;
				}

				if( supportWhirlpool ){

					options->addItem( algo + ".xts-plain64.256.whirlpool" ) ;
				}
			} ;

			for( const auto& it : list ){

				_add_option( it ) ;
			}
		} ;

		_add_options( { "aes",
				"serpent",
				"twofish",
				"aes:serpent",
				"twofish:aes",
				"serpent:twofish",
				"aes:twofish:serpent",
				"serpent:twofish:aes" } ) ;
	}
}

void createvolume::ShowUI( const QString& l,const QString& v )
{
	this->enableAll() ;

	m_ui->labelVolumePath->setText( l ) ;
	m_ui->lineEditVolumePath->setText( v ) ;
	m_ui->pbOpenKeyFile->setEnabled( false ) ;
	m_ui->lineEditVolumePath->setEnabled( false ) ;

	this->cbNormalVolume( 0 ) ;

	m_ui->lineEditPassphrase1->setFocus() ;
	m_ui->comboBoxFS->setCurrentIndex( 0 ) ;
	m_ui->comboBoxRNG->setCurrentIndex( 0 ) ;
	m_ui->labelRepeatPassPhrase->setEnabled( true ) ;
	m_created = false ;

	this->show() ;
	this->eraseDataPartition() ;
}

void createvolume::pbOpenKeyFile()
{
	auto Z = QFileDialog::getOpenFileName( this,tr( "Keyfile Path" ),utility::homePath() ) ;

	if( !Z.isEmpty() ){

		m_ui->lineEditPassphrase1->setText( Z ) ;
	}
}

void createvolume::pbOpenHiddenKeyFile()
{
	auto Z = QFileDialog::getOpenFileName( this,tr( "Keyfile Path" ),utility::homePath() ) ;

	if( !Z.isEmpty() ){

		m_ui->lineEditHiddenKey->setText( Z ) ;
	}
}

void createvolume::pbCancelClicked()
{
#if 0
	if( m_created == false ){

		auto s = m_ui->lineEditVolumePath->text() ;

		if( !s.startsWith( "/dev/" ) ){

			if( utility::loopDevicePath( s ).isEmpty() ){

				QFile::remove( s ) ;
			}
		}
	}
#endif

	this->HideUI() ;
}

void createvolume::tcryptGui( bool e )
{
	m_normalVolume = e ;

	//this->disableAll() ;

	tcrypt::instance( this,true,[ this ]( const QString& key,const QStringList& keyFiles ){

		//this->enableAll() ;

		if( m_normalVolume ){

			m_key = key ;
			m_keyFiles = keyFiles ;
		}else{
			m_hiddenKey = key ;
			m_hiddenKeyFiles = keyFiles ;
		}

	},[ this ](){

		//this->enableAll() ;

		bool x = m_ui->cbNormalVolume->currentIndex() == createvolume::normal_truecrypt ;
		bool y = m_ui->cbNormalVolume->currentIndex() == createvolume::normal_veracrypt ;

		if( x || y ){

			m_ui->pbOpenKeyFile->setEnabled( false ) ;
			m_ui->lineEditPassphrase1->setEnabled( false ) ;
		}

		if( m_normalVolume ){

			m_key.clear() ;
			m_keyFiles.clear() ;
		}else{
			m_hiddenKey.clear() ;
			m_hiddenKeyFiles.clear() ;
		}
	} ) ;
}

void createvolume::cbNormalVolume( int r )
{
	this->setWindowTitle( tr( "Create A New Volume" ) ) ;

	auto _set_key_ui = [ this ](){

		m_ui->pbOpenKeyFile->setEnabled( false ) ;
		m_ui->lineEditPassPhrase2->setEnabled( true ) ;
		m_ui->lineEditPassphrase1->setEnabled( true ) ;
		m_ui->lineEditPassphrase1->clear() ;
		m_ui->lineEditPassPhrase2->clear() ;
		m_ui->lineEditPassphrase1->setEchoMode( QLineEdit::Password ) ;
		m_ui->lineEditPassPhrase2->setEchoMode( QLineEdit::Password ) ;
		m_ui->labelPassPhrase->setText( tr( "Password" ) ) ;

		m_ui->labelRepeatPassPhrase->setEnabled( true ) ;
		m_ui->pbOpenKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;
	} ;

	if( r == 0 ){

		_set_key_ui() ;

	}else if( r == 1 ){

		m_ui->pbOpenKeyFile->setEnabled( true ) ;
		m_ui->lineEditPassphrase1->clear() ;
		m_ui->lineEditPassPhrase2->clear() ;
		m_ui->lineEditPassphrase1->setEchoMode( QLineEdit::Normal ) ;
		m_ui->lineEditPassPhrase2->setEnabled( false ) ;
		m_ui->labelPassPhrase->setText( tr( "KeyFile" ) ) ;
		m_ui->labelRepeatPassPhrase->setEnabled( false ) ;
		m_ui->pbOpenKeyFile->setIcon( QIcon( ":/keyfile.png" ) ) ;

	}else if( r == 2 ){

		_set_key_ui() ;

		m_ui->lineEditPassphrase1->setEnabled( false ) ;
		m_ui->lineEditPassPhrase2->setEnabled( false ) ;

		plugin::instance( this,plugins::plugin::hmac_key,[ this ]( const QString& key ){

			m_key = key ;

			m_ui->lineEditPassphrase1->setText( m_key ) ;
			m_ui->lineEditPassPhrase2->setText( m_key ) ;

			if( key.isEmpty() ){

				m_ui->cbNormalVolume->setCurrentIndex( 0 ) ;

				this->cbNormalVolume( 0 ) ;
			}else{
				if( m_keyStrength.canCheckQuality() ){

					this->setWindowTitle( tr( "Passphrase Quality: 100%" ) ) ;
				}
			}
		} ) ;
	}else if( r == 3 ){

		_set_key_ui() ;
	}else{
		m_ui->pbOpenKeyFile->setEnabled( false ) ;
		m_ui->lineEditPassphrase1->clear() ;
		m_ui->lineEditPassPhrase2->clear() ;
		m_ui->lineEditPassphrase1->setEchoMode( QLineEdit::Normal ) ;
		m_ui->lineEditPassPhrase2->setEnabled( false ) ;
		m_ui->lineEditPassphrase1->setEnabled( false ) ;
		m_ui->lineEditPassphrase1->setText( QString() ) ;
		m_ui->lineEditPassPhrase2->setText( QString() ) ;
		m_ui->labelPassPhrase->setText( tr( "Keys" ) ) ;
		m_ui->labelRepeatPassPhrase->setEnabled( false ) ;
		m_ui->pbOpenKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;

		this->tcryptGui( true ) ;
	}
}

void createvolume::cbHiddenVolume( int r )
{
	this->setWindowTitle( tr( "Create A New Volume" ) ) ;

	auto _set_key_ui = [ this ](){

		m_ui->pbHiddenKeyFile->setEnabled( false ) ;
		m_ui->lineEditHiddenKey1->setEnabled( true ) ;
		m_ui->lineEditHiddenKey->setEnabled( true ) ;
		m_ui->lineEditHiddenKey->clear() ;
		m_ui->lineEditHiddenKey1->clear() ;
		m_ui->lineEditHiddenKey->setEchoMode( QLineEdit::Password ) ;
		m_ui->lineEditHiddenKey1->setEchoMode( QLineEdit::Password ) ;
		m_ui->labelHidden->setText( tr( "Password" ) ) ;
		m_ui->pbHiddenKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;
	} ;

	if( r == 0 ){

		_set_key_ui() ;

	}else if( r == 1 ){

		m_ui->pbHiddenKeyFile->setEnabled( true ) ;
		m_ui->lineEditHiddenKey->clear() ;
		m_ui->lineEditHiddenKey1->clear() ;
		m_ui->lineEditHiddenKey->setEchoMode( QLineEdit::Normal ) ;
		m_ui->lineEditHiddenKey->setEnabled( true ) ;
		m_ui->lineEditHiddenKey1->setEnabled( false ) ;
		m_ui->labelHidden->setText( tr( "KeyFile" ) ) ;
		m_ui->pbHiddenKeyFile->setIcon( QIcon( ":/keyfile.png" ) ) ;

	}else if( r == 2 ){

		_set_key_ui() ;

		m_ui->lineEditHiddenKey->setEnabled( false ) ;
		m_ui->lineEditHiddenKey1->setEnabled( false ) ;

		plugin::instance( this,plugins::plugin::hmac_key,[ this ]( const QString& key ){

			m_hiddenKey = key ;

			m_ui->lineEditHiddenKey->setText( m_hiddenKey ) ;
			m_ui->lineEditHiddenKey1->setText( m_hiddenKey ) ;

			if( key.isEmpty() ){

				m_ui->cbHiddenVolume->setCurrentIndex( 0 ) ;

				this->cbHiddenVolume( 0 ) ;
			}else{
				if( m_keyStrength.canCheckQuality() ){

					this->setWindowTitle( tr( "Passphrase Quality: 100%" ) ) ;
				}
			}
		} ) ;
	} else if( r == 3 ){

		_set_key_ui() ;
	}else{
		m_ui->pbHiddenKeyFile->setEnabled( false ) ;
		m_ui->lineEditHiddenKey->clear() ;
		m_ui->lineEditHiddenKey1->clear() ;
		m_ui->lineEditHiddenKey->setEchoMode( QLineEdit::Normal ) ;
		m_ui->lineEditHiddenKey1->setEnabled( false ) ;
		m_ui->lineEditHiddenKey->setEnabled( false ) ;
		m_ui->lineEditHiddenKey->setText( QString() ) ;
		m_ui->lineEditHiddenKey1->setText( QString() ) ;
		m_ui->labelHidden->setText( tr( "Keys" ) ) ;
		m_ui->pbHiddenKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;

		this->tcryptGui( false ) ;
	}
}

void createvolume::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void createvolume::enableAll()
{
	m_ui->lineEditIterationNumber->setEnabled( true ) ;
	m_ui->labelIterationNumber->setEnabled( true ) ;

	auto enable = m_ui->comboBoxVolumeType->currentText().contains( "VeraCrypt" ) ;

	m_ui->label_2->setEnabled( enable ) ;
	m_ui->lineEditPIM->setEnabled( enable ) ;

	m_ui->labelPassPhrase->setEnabled( true ) ;
	m_ui->labelVolumePath->setEnabled( true ) ;
	m_ui->labelRepeatPassPhrase->setEnabled( true ) ;
	m_ui->lineEditPassphrase1->setEnabled( true ) ;

	if( m_ui->cbNormalVolume->currentIndex() == 0 || m_ui->cbNormalVolume->currentIndex() == 3 ){

		m_ui->lineEditPassPhrase2->setEnabled( true ) ;
	}

	//m_ui->lineEditVolumePath->setEnabled( true ) ;
	m_ui->pbCancel->setEnabled( true ) ;
	m_ui->pbCreate->setEnabled( true ) ;
	m_ui->pbOpenKeyFile->setEnabled( true ) ;
	m_ui->labelfs->setEnabled( true ) ;
	m_ui->labelvolumetype->setEnabled( true ) ;
	m_ui->labelrng->setEnabled( true ) ;
	m_ui->comboBoxFS->setEnabled( true ) ;
	m_ui->comboBoxVolumeType->setEnabled( true ) ;

	m_ui->cbNormalVolume->setEnabled( true ) ;
	m_ui->labelvolumeOptions->setEnabled( true ) ;
	m_ui->comboBoxOptions->setEnabled( true ) ;

	auto e = m_ui->comboBoxVolumeType->currentIndex() ;

	if( e != createvolume::plain ){

		m_ui->comboBoxRNG->setEnabled( true ) ;
	}

	if( e == createvolume::plain_with_offset ){

		m_ui->lineEditHiddenSize->setEnabled( true ) ;
		m_ui->label->setEnabled( true ) ;
		m_ui->comboBoxHiddenSize->setEnabled( true ) ;

	}else if( e == createvolume::normal_and_hidden_truecrypt || e == createvolume::normal_and_hidden_veracrypt ){

		m_ui->lineEditHiddenSize->setEnabled( true ) ;
		m_ui->label->setEnabled( true ) ;
		m_ui->comboBoxHiddenSize->setEnabled( true ) ;

		m_ui->cbHiddenVolume->setEnabled( true ) ;
		m_ui->pbHiddenKeyFile->setEnabled( true ) ;

		m_ui->cbHiddenVolume->setEnabled( true ) ;

		m_ui->labelHidden->setEnabled( true ) ;
		m_ui->label_2Hidden->setEnabled( true ) ;
		m_ui->lineEditHiddenKey->setEnabled( true ) ;
		m_ui->lineEditHiddenKey1->setEnabled( true ) ;

		m_ui->pbHiddenKeyFile->setEnabled(  m_ui->cbHiddenVolume->currentIndex() == 1 ) ;
		m_ui->lineEditHiddenKey1->setEnabled(  m_ui->cbHiddenVolume->currentIndex() != 1 ) ;
	}

	if( m_ui->comboBoxVolumeType->currentText().startsWith( "LUKS2" ) ){

		m_ui->pbLUKS2Options->setEnabled( true ) ;
	}
}

void createvolume::disableAll()
{
	m_ui->pbLUKS2Options->setEnabled( false ) ;
	m_ui->lineEditIterationNumber->setEnabled( false ) ;
	m_ui->labelIterationNumber->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->lineEditPIM->setEnabled( false ) ;
	m_ui->labelPassPhrase->setEnabled( false ) ;
	m_ui->labelVolumePath->setEnabled( false ) ;
	m_ui->labelRepeatPassPhrase->setEnabled( false ) ;
	m_ui->lineEditPassphrase1->setEnabled( false ) ;
	m_ui->lineEditPassPhrase2->setEnabled( false ) ;
	m_ui->lineEditVolumePath->setEnabled( false ) ;
	m_ui->pbCancel->setEnabled( false ) ;
	m_ui->pbCreate->setEnabled( false ) ;
	m_ui->pbOpenKeyFile->setEnabled( false ) ;
	m_ui->labelfs->setEnabled( false ) ;
	m_ui->labelrng->setEnabled( false ) ;
	m_ui->labelvolumetype->setEnabled( false ) ;
	m_ui->comboBoxFS->setEnabled( false ) ;
	m_ui->comboBoxVolumeType->setEnabled( false ) ;
	m_ui->comboBoxRNG->setEnabled( false ) ;
	m_ui->cbNormalVolume->setEnabled( false ) ;
	m_ui->labelvolumeOptions->setEnabled( false ) ;
	m_ui->comboBoxOptions->setEnabled( false ) ;
	m_ui->lineEditHiddenSize->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->comboBoxHiddenSize->setEnabled( false ) ;
	m_ui->labelHidden->setEnabled( false ) ;
	m_ui->label_2Hidden->setEnabled( false ) ;
	m_ui->lineEditHiddenKey->setEnabled( false ) ;
	m_ui->lineEditHiddenKey1->setEnabled( false ) ;
	m_ui->cbHiddenVolume->setEnabled( false ) ;
	m_ui->pbHiddenKeyFile->setEnabled( false ) ;
}

void createvolume::pbCreateClicked()
{
	DialogMsg msg( this ) ;

	auto volumePath   = m_ui->lineEditVolumePath->text() ;
	auto passphrase_1 = m_ui->lineEditPassphrase1->text() ;
	auto passphrase_2 = m_ui->lineEditPassPhrase2->text() ;

	this->disableAll() ;

	utility2::raii raii( [ this ](){ this->enableAll() ; } ) ;

	if( utility::requireSystemPermissions( volumePath ) ){

		if( !utility::enablePolkit( utility::background_thread::False ) ){

			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to enable polkit support" ) ) ;
		}
	}

	createvolume::createVolumeType type = createvolume::createVolumeType( m_ui->comboBoxVolumeType->currentIndex() ) ;

	if( volumePath.isEmpty() ){

		return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Volume path field is empty" ) ) ;
	}

	if( type == createvolume::plain_with_offset ){

		if( m_ui->lineEditHiddenSize->text().isEmpty() ){

			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
		}
	}

	if( type == createvolume::normal_and_hidden_truecrypt || type == createvolume::normal_and_hidden_veracrypt ){

		auto x = m_ui->lineEditHiddenSize->text() ;

		if( x.isEmpty() ){

			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
		}

		bool ok ;
		x.toUInt( &ok ) ;

		if( !ok ){

			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Illegal character detected in the hidden volume size field" ) ) ;
		}

		if( m_ui->cbHiddenVolume->currentIndex() < 3 ){

			if( m_ui->cbHiddenVolume->currentIndex() == 0 ){

				if( m_ui->lineEditHiddenKey->text() != m_ui->lineEditHiddenKey1->text() ){

					return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Hidden passphrases do not match" ) ) ;
				}

			}else if( m_ui->cbHiddenVolume->currentIndex() == 1 ){

				if( m_ui->lineEditHiddenKey->text().isEmpty() ){

					return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
				}
			}
		}
		if( !m_ui->comboBoxFS->currentText().contains( "fat" ) && m_warned == false ){

			m_warned = true ;

			return msg.ShowUIOK( tr( "WARNING" ),tr( "It is best to create a hidden volume with vfat/fat file system." ) ) ;
		}
	}

	QString source ;

	int r = m_ui->cbNormalVolume->currentIndex() ;

	if( r == 0 ){

		if( passphrase_1 != passphrase_2 ){

			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Passphrases do not match" ) ) ;
		}else{
			source = "-f" ;
			passphrase_1 = utility::keyPath() + "-2" ;

			auto key = m_ui->lineEditPassphrase1->text() ;

			utility::keySend( passphrase_1,key ) ;
		}
	}else if( r == 1 ){

		if( passphrase_1.isEmpty() ){

			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
		}else{
			source = "-f" ;
			passphrase_1 = utility::resolvePath( passphrase_1 ).replace( "\"","\"\"\"" ) ;
		}
	}else if( r == 2 ){

		source = "-f" ;

		passphrase_1 = utility::keyPath() + "-2" ;

		utility::keySend( passphrase_1,m_key ) ;

	}else if( r == 3 ){

		if( passphrase_1.isEmpty() ){

			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
		}
		if( passphrase_1 != passphrase_2 ){

			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Passphrases do not match" ) ) ;
		}

		auto m = utility::yubiKey( passphrase_1 ) ;

		if( m.has_value() ){

			source = "-f" ;
			passphrase_1 = utility::keyPath() + "-2" ;

			utility::keySend( passphrase_1,m.value() ) ;
		}else{
			return msg.ShowUIOK( tr( "ERROR" ),tr( "Failed To Locate Or Run Yubikey's \"ykchalresp\" Program." ) ) ;
		}
	}

	switch( type ){

	case createvolume::luks :
	case createvolume::luks_external_header :

		m_volumeType = "luks" ;

		break ;
#ifdef CRYPT_LUKS2
	case createvolume::luks2 :
	case createvolume::luks2_external_header :

		m_volumeType = "luks2" ;

		break ;
#endif
	case createvolume::plain :
	case createvolume::plain_with_offset :

		m_volumeType = "plain" ;

		break ;
	case createvolume::normal_truecrypt :
	case createvolume::normal_and_hidden_truecrypt :

		m_volumeType = "tcrypt" ;

		break ;
	case createvolume::normal_veracrypt :
	case createvolume::normal_and_hidden_veracrypt :

		m_volumeType = "vcrypt" ;

		break ;
	default: m_volumeType = "luks" ;
	}

	QString g ;

	switch( m_ui->comboBoxRNG->currentIndex() ){

		case 0 : g = "/dev/urandom" ; break ;
		case 1 : g = "/dev/random"  ; break ;
		default: g = "/dev/urandom" ;
	}

	g += "." + m_ui->comboBoxOptions->currentText() ;

	if( type == createvolume::plain_with_offset ){

		g += "." + m_ui->lineEditHiddenSize->text() ;

		switch( m_ui->comboBoxHiddenSize->currentIndex() ){

			case 0 : g += "b" ; break ;
			case 1 : g += "k" ; break ;
			case 2 : g += "m" ; break ;
			case 3 : g += "g" ; break ;
		}

	}else if( type == createvolume::plain ){

		g += ".0" ;

	}else if( type == createvolume::normal_veracrypt ||
		  type == createvolume::normal_and_hidden_veracrypt ){

		auto e = m_ui->lineEditPIM->text() ;

		if( !e.isEmpty() ){

			g += "." + e ;
		}
	}else{
		auto forcedIterations = m_ui->lineEditLuks2ForcedIteration->text() ;
		auto unlockingTime    = m_ui->lineEditLuks2UnlockingTime->text() ;
		auto maxThreads       = m_ui->lineEditLuks2ParallelThreads->text() ;
		auto maxMemory        = m_ui->lineEditLuks2MaxMemory->text() ;
		auto label            = m_ui->lineLEdituks2Label->text() ;
		auto subsystem        = m_ui->lineEditLuks2SubSystem->text() ;
		auto pbkdf            = m_ui->cbLuks2Pbkdf->currentText() ;

		QString integrity = "null" ;

		if( label.isEmpty() ){

			label = "null" ;
		}

		if( subsystem.isEmpty() ){

			subsystem = "null" ;
		}

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

		auto allowDiscard = [ this ](){

			if( m_ui->cbLuks2AllowDiscard->isChecked() ){

				return "1" ;
			}else{
				return "0" ;
			}
		}() ;

		QString m = ".%1.%2.%3.%4.%5.%6.%7.%8.%9" ;
		g += m.arg( unlockingTime,forcedIterations,integrity,pbkdf,maxMemory,maxThreads,label,subsystem,allowDiscard ) ;
	}

	volumePath.replace( "\"","\"\"\"" ) ;

	auto a = QString( ZULUCRYPTzuluCrypt ) ;
	auto b = volumePath ;
	auto c = m_ui->comboBoxFS->currentText() ;
	auto d = m_volumeType ;
	auto e = source ;
	auto f = passphrase_1 ;

	QString exe ;

	if( type == createvolume::normal_and_hidden_truecrypt || type == createvolume::normal_and_hidden_veracrypt ){

		auto x = m_ui->lineEditHiddenKey->text() ;

		QString y ;

		auto k = m_ui->cbHiddenVolume->currentIndex() ;

		if( k == 0 ){

			if( x != m_ui->lineEditHiddenKey1->text() ){

				return msg.ShowUIOK( tr( "ERROR!" ),tr( "Passphrases do not match" ) ) ;
			}

			y = utility::keyPath() + "-1" ;
			utility::keySend( y,x ) ;

		}else if( k == 1 ){

			y = utility::resolvePath( x ).replace( "\"","\"\"\"" ) ;

		}else if( k == 2 ){

			y = utility::keyPath() + "-1" ;
			utility::keySend( y,m_hiddenKey ) ;

		}else if( k == 3 ){

			if( x.isEmpty() ){

				return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
			}

			if( x != m_ui->lineEditHiddenKey1->text() ){

				return msg.ShowUIOK( tr( "ERROR!" ),tr( "Passphrases do not match" ) ) ;
			}

			auto m = utility::yubiKey( x ) ;

			if( m.has_value() ){

				y = utility::keyPath() + "-1" ;
				utility::keySend( y,m.value() ) ;
			}else{
				return msg.ShowUIOK( tr( "ERROR" ),tr( "Failed To Locate Or Run Yubikey's \"ykchalresp\" Program." ) ) ;
			}
		}

		auto r = m_ui->lineEditHiddenSize->text().toULongLong() ;

		switch( m_ui->comboBoxHiddenSize->currentIndex() ){

			case 0 :			  ; break ;
			case 1 : r *= 1024                ; break ;
			case 2 : r *= 1024 * 1024         ; break ;
			case 3 : r *= 1024 * 1024 * 1024  ; break ;
		}

		QString z = QString::number( r ) ;

		const auto arg = "%1 -c -k -d \"%2\" -z %3 -t %4 %5 \"%6\" -g %7 -e %8 -u \"%9\"" ;
		exe = QString( arg ).arg( a,b,c,d,e,f,g,z,y ) ;
	}else{
		const auto arg = "%1 -c -k -d \"%2\" -z %3 -t %4 %5 \"%6\" -g %7" ;
		exe = QString( arg ).arg( a,b,c,d,e,f,g ) ;
	}

	for( const auto& it : m_keyFiles ){

		auto e = it ;
		e.replace( "\"","\"\"\"" ) ;
		exe += " -F \"" + e + "\"" ;
	}

	for( const auto& it : m_hiddenKeyFiles ){

		auto e = it ;
		e.replace( "\"","\"\"\"" ) ;
		exe += " -V \"" + e + "\"" ;
	}

	if( type == createvolume::normal_veracrypt || type == createvolume::normal_and_hidden_veracrypt ){

		m_veraCryptWarning.show( tr( "Please be patient as creating a VeraCrypt volume may take a very long time.\n\n" ) ) ;
	}

	m_isWindowClosable = false ;

	raii.cancel() ;

#ifdef CRYPT_LUKS2
	if( type == createvolume::luks_external_header || type == createvolume::luks2_external_header ){
#else
	if( type == createvolume::luks_external_header ){
#endif
		this->taskFinished_1( utility::Task::run( utility::appendUserUID( exe ) ).await() ) ;
	}else{
		this->taskFinished( utility::Task::run( utility::appendUserUID( exe ) ).await() ) ;
	}
}

void createvolume::taskFinished_1( const utility::Task& e )
{
	if( e.exitCode() != 0 ){

		this->taskFinished( e ) ;
	}else{
		auto volumePath   = m_ui->lineEditVolumePath->text() ;
		volumePath.replace( "\"","\"\"\"" ) ;

		auto backUp = utility::homePath() + "/" + volumePath.split( '/' ).last() + ".volumeHeaderBackUp" ;

		auto e = "%1 -B -d \"%2\" -z \"%3\"" ;

		auto exe = QString( e ).arg( ZULUCRYPTzuluCrypt,volumePath,backUp ) ;

		DialogMsg msg( this ) ;

		if( utility::exec( exe ).await() != 0 ){

			msg.ShowUIOK( tr( "WARNING!" ),tr( "Volume created successfully but failed to create an external header" ) ) ;
		}else{
			std::atomic_bool exit( false ) ;

			auto function = []( quint64 size,quint64 offset ){

				Q_UNUSED( size )
				Q_UNUSED( offset )
			} ;

			auto s = quint64( QFileInfo( backUp ).size() ) ;

			if( utility::clearVolume( volumePath,
						  &exit,
						  s != 0 ? s : 2 * 1024 * 1024,
						  function ).await() == 0 ){

				msg.ShowUIOK( tr( "SUCCESS!" ),tr( "Luks volume created successfully." ) ) ;
			}else{
				msg.ShowUIOK( tr( "WARNING!" ),tr( "Luks volume created successfully,external header created successfully but failed to erase header on the device" ) ) ;
			}
		}

		this->HideUI() ;
	}
}

void createvolume::taskFinished( const utility::Task& e )
{
	DialogMsg msg( this ) ;

	m_veraCryptWarning.stopTimer() ;
	m_isWindowClosable = true ;

	auto x = tr( "Volume created successfully." ) ;

	if( utility::containsAtleastOne( m_volumeType,"luks","tcrypt","vcrypt" ) ){

		x += tr( "\nCreating a backup of the \"%1\" volume header is strongly advised.\nPlease read documentation on why this is important." ).arg( m_volumeType ) ;
	}

	auto _err = [ & ](){

		return tr( "Failed to create a volume" ) + "\n\n" + e.stdError() ;
	} ;

	switch ( e.exitCode() ){
		case 0 : msg.ShowUIOK( tr( "SUCCESS!" ),x ) ;
		return this->HideUI() ;
		case 1 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Presented file system is not supported,see documentation for more information" ) ) ;	break  ;
		case 2 : msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open a system device in read/write mode,\n\
only root user or members of group zulucrypt can do that" ) ) ;										break  ;
		case 3 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not create an encrypted volume" ) ) ;					break  ;
		case 4 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not open volume for writing" ) ) ;					break  ;
		case 5 : msg.ShowUIOK( tr( "ERROR!" ),tr( "There seem to be an opened mapper associated with the device" ) ) ;		break  ;
		case 6 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Can not create a volume on a mounted device" ) ) ;				break  ;
		case 7 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Container file must be bigger than 3MB" ) ) ;				break  ;
		case 8 : msg.ShowUIOK( tr( "ERROR!" ),tr( "%1 not found" ).arg( ZULUCRYPTmkfs ) ) ;					break  ;
		case 9 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient memory to hold your response" ) ) ;				break  ;
		case 10: msg.ShowUIOK( tr( "ERROR!" ),tr( "Operation terminated per user request" ) ) ; 				break  ;
		case 11: msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not get passphrase in silent mode" ) ) ;				break  ;
		case 12: msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient memory to hold the passphrase" ) ) ;				break  ;
		case 13: msg.ShowUIOK( tr( "ERROR!" ),tr( "Passphrases do not match" ) ) ;						break  ;
		case 14: msg.ShowUIOK( tr( "ERROR!" ),tr( "Invalid path to key file" ) ) ;						break  ;
		case 15: msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not get a key from a key file" ) ) ;					break  ;
		case 16: msg.ShowUIOK( tr( "ERROR!" ),tr( "Couldnt get enought memory to hold the key file" ) ) ;			break  ;
		case 17: msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not get a key from a socket" ) ) ;					break  ;
		case 18: msg.ShowUIOK( tr( "ERROR!" ),tr( "One or more required argument(s) for this operation is missing" ) ) ;	break  ;
		case 19: msg.ShowUIOK( tr( "ERROR!" ),tr( "Can not get passphrase in silent mode" ) ) ;					break  ;
		case 20: msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient memory to hold passphrase" ) ) ;				break  ;
		case 21: msg.ShowUIOK( tr( "ERROR!" ),tr( "Passphrases do not match") ) ;						break  ;
		case 22: msg.ShowUIOK( tr( "ERROR!" ),_err() ) ;						break  ;
		case 23: msg.ShowUIOK( tr( "ERROR!" ),tr( "Wrong argument detected for tcrypt volume" ) ) ;				break  ;
		case 110:msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not find any partition with the presented UUID" ) ) ;			break  ;
		default: msg.ShowUIOK( tr( "ERROR!" ),tr( "Error Code: %1\n--\nStdOut: %2\n--\nStdError: %3").arg( QString::number( e.exitCode() ),QString( e.stdError() ),QString( e.stdOut() ) ) ) ;
	}

	m_veraCryptWarning.hide() ;
	this->enableAll() ;
}

createvolume::~createvolume()
{
	delete m_ui ;
}

/*
 *
 *  Copyright ( c ) 2011
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

#include "task.h"
#include "utility.h"
#include "erasedevice.h"
#include "createvolumedialog.h"
#include "dialogmsg.h"
#include "tcrypt.h"

#include <QDebug>
#include "../zuluCrypt-cli/constants.h"

createvolume::createvolume( QWidget * parent ) :
    QDialog( parent ),
    m_ui( new Ui::createvolume )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_ui->lineEditVolumePath->setEnabled( false ) ;
	m_ui->lineEditPassphrase1->setFocus() ;

	m_isWindowClosable = true ;
	m_warned = false ;

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

	m_ui->groupBox->setEnabled( false ) ;

	this->setOptions( 1 ) ;

	this->installEventFilter( this ) ;

	m_ui->labelvolumeOptions->setVisible( false ) ;

	QString a = tr( "options are separated by a \".\" character.\n\n" ) ;
	QString b = tr( "multiple algorithms are separated by \":\" character.\n\n" ) ;
	QString c = tr( "options are in a format of \"algorithm.cipher mode.key size in bits.hash\"\n\n") ;
	QString d = tr( "default option is the first entry on the list" ) ;

	m_ui->comboBoxOptions->setToolTip( a + b + c + d ) ;

	m_ui->cbNormalVolume->addItem( tr( "key" ) ) ;
	m_ui->cbNormalVolume->addItem( tr( "keyfile" ) ) ;

	m_ui->cbHiddenVolume->addItem( tr( "key" ) ) ;
	m_ui->cbHiddenVolume->addItem( tr( "keyfile" ) ) ;

#if TRUECRYPT_CREATE
	m_ui->comboBoxVolumeType->addItem( tr( "normal truecrypt" ) ) ;
	m_ui->comboBoxVolumeType->addItem( tr( "normal+hidden truecrypt" ) ) ;
#endif
	m_ui->comboBoxVolumeType->setCurrentIndex( int( createvolume::luks ) ) ;
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
			this->setWindowTitle( tr( "passphrase quality: 0/100" ) ) ;
		}else{
			this->setWindowTitle( tr( "passphrase quality: %1/100" ).arg( QString::number( st ) ) ) ;
		}
	}else{
		this->setWindowTitle( tr( "create a new volume" ) ) ;
	}
}

bool createvolume::eventFilter( QObject * watched,QEvent * event )
{
	if( utility::eventFilter( this,watched,event ) ){
		this->HideUI() ;
		return true ;
	}else{
		return false ;
	}
}

void createvolume::findInstalledFs()
{
	QStringList l ;

	l << "ext4" << "vfat" << "ntfs" << "ext2" << "ext3" ;

	QFile f( "/proc/filesystems" ) ;

	if( f.open( QIODevice::ReadOnly ) ){

		QStringList r = QString( f.readAll() ).split('\n',QString::SkipEmptyParts ) ;

		for( const auto& it : r ){

			if( !it.startsWith( "nodev" ) ){

				QString e = it ;
				e.remove( '\t' ) ;

				if( !l.contains( e ) && e != "fuseblk" ){

					l.append( e ) ;
				}
			}
		}
	}

	m_ui->comboBoxFS->addItems( l ) ;
}

void createvolume::volumeType( int s )
{
	m_ui->cbNormalVolume->clear() ;
	m_ui->cbHiddenVolume->clear() ;

	m_ui->cbNormalVolume->addItem( tr( "key" ) ) ;
	m_ui->cbNormalVolume->addItem( tr( "keyfile" ) ) ;
	m_ui->cbHiddenVolume->addItem( tr( "key" ) ) ;
	m_ui->cbHiddenVolume->addItem( tr( "keyfile" ) ) ;

	switch( createvolume::createVolumeType( s ) ){
	case createvolume::luks :
		m_volumeType = "luks" ;
		m_ui->comboBoxRNG->setEnabled( true ) ;
		m_ui->groupBox->setEnabled( false ) ;
		break ;
	case createvolume::plain :
		m_volumeType = "plain"  ;
		m_ui->comboBoxRNG->setEnabled( false ) ;
		m_ui->groupBox->setEnabled( false ) ;
		break ;
	case createvolume::normal_truecrypt :
		m_volumeType = "tcrypt" ;
		m_ui->comboBoxRNG->setEnabled( true ) ;
		m_ui->groupBox->setEnabled( false ) ;
		m_ui->cbNormalVolume->addItem( tr( "TrueCrypt keys" ) ) ;
		break ;
	case createvolume::normal_and_hidden_truecrypt :
		m_volumeType = "tcrypt" ;
		m_ui->comboBoxRNG->setEnabled( true ) ;
		m_ui->groupBox->setEnabled( true ) ;
		m_ui->cbHiddenVolume->setCurrentIndex( 0 ) ;
		m_ui->cbHiddenVolume->addItem( tr( "TrueCrypt keys" ) ) ;
		m_ui->cbNormalVolume->addItem( tr( "TrueCrypt keys" ) ) ;
		this->cbHiddenVolume( 0 ) ;
		break ;
	}
}

void createvolume::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	if( m_isWindowClosable ){
		pbCancelClicked() ;
	}
}

void createvolume::ShowPartition( QString volume )
{
	this->ShowUI( tr( "path to device" ),volume ) ;
}

void createvolume::ShowFile( QString volume )
{
	this->ShowUI( tr( "path to file" ),volume ) ;
}

void createvolume::eraseDataPartition()
{
	QString path = m_ui->lineEditVolumePath->text() ;

	if( path.startsWith( "/dev/" ) ){
		CreateVolumeDialog * cpd = new CreateVolumeDialog( path,this ) ;
		connect( cpd,SIGNAL( dialogResult( int ) ),this,SLOT( dialogResult( int ) ) ) ;
		cpd->ShowUI() ;
	}
}

void createvolume::dialogResult( int result )
{
	if( result == 0 ){
		this->HideUI() ;
	}else if( result == 1 ){
		;
	}else if( result == 2 ){
		erasedevice * ed = new erasedevice( this ) ;
		connect( ed,SIGNAL( HideUISignal() ),ed,SLOT( deleteLater() ) ) ;
		ed->ShowUI( m_ui->lineEditVolumePath->text() ) ;
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

	if( e == 0 ){

		/*
		 * crypto options for plain dm-crypt volumes
		 */
		options->addItem( "aes.cbc-essiv:256.256.ripemd160" ) ;

	}else if( e == 1 ){

		/*
		 * cryto options for LUKS volumes.
		 */
		auto _add_option = [ & ]( const QString& algo ){

			options->addItem( algo + ".xts-plain64.256.sha1" ) ;
			options->addItem( algo + ".xts-plain64.256.sha256" ) ;
			options->addItem( algo + ".xts-plain64.256.sha512" ) ;
			options->addItem( algo + ".xts-plain64.256.ripemd160" ) ;

			if( supportWhirlpool ){
				options->addItem( algo + ".xts-plain64.256.whirlpool" ) ;
			}

			options->addItem( algo + ".xts-plain64.512.sha1" ) ;
			options->addItem( algo + ".xts-plain64.512.sha256" ) ;
			options->addItem( algo + ".xts-plain64.512.sha512" ) ;
			options->addItem( algo + ".xts-plain64.512.ripemd160" ) ;

			if( supportWhirlpool ){
				options->addItem( algo + ".xts-plain64.512.whirlpool" ) ;
			}
		} ;

		_add_option( "aes" ) ;
		_add_option( "serpent" ) ;
		_add_option( "twofish" ) ;
	}else{
		/*
		 * crypto options for TrueCrypt volumes
		 */
		auto _add_option = [ & ]( const QString& algo ){

			options->addItem( algo + ".xts-plain64.256.ripemd160" ) ;
			options->addItem( algo + ".xts-plain64.256.sha512" ) ;

			if( supportWhirlpool ){
				options->addItem( algo + ".xts-plain64.256.whirlpool" ) ;
			}
		} ;

		_add_option( "aes" ) ;
		_add_option( "serpent" ) ;
		_add_option( "twofish" ) ;
		_add_option( "aes:serpent" ) ;
		_add_option( "twofish:aes" ) ;
		_add_option( "serpent:twofish" ) ;
		_add_option( "aes:twofish:serpent" ) ;
		_add_option( "serpent:twofish:aes" ) ;
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
	this->findInstalledFs() ;
}

void createvolume::pbOpenKeyFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "key file path" ),QDir::homePath(),0 ) ;
	if( !Z.isEmpty() ){
		m_ui->lineEditPassphrase1->setText( Z ) ;
	}
}

void createvolume::pbOpenHiddenKeyFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "key file path" ),QDir::homePath(),0 ) ;
	if( !Z.isEmpty() ){
		m_ui->lineEditHiddenKey->setText( Z ) ;
	}
}

void createvolume::pbCancelClicked()
{
	if( m_created == false ){

		QString s = m_ui->lineEditVolumePath->text() ;
		if( !s.startsWith( "/dev/" ) ){
			QFile::remove( s ) ;
		}
	}
	this->HideUI() ;
}

void createvolume::tcryptCancelled()
{
	this->enableAll() ;

	if( m_ui->cbNormalVolume->currentIndex() == createvolume::normal_truecrypt ){

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
}

void createvolume::tcryptGui( bool e )
{
	m_normalVolume = e ;
	this->disableAll() ;

	tcrypt * t = new tcrypt( this ) ;
	connect( t,SIGNAL( Keys( QString,QStringList ) ),this,SLOT( keys( QString,QStringList ) ) ) ;
	connect( t,SIGNAL( cancelled() ),this,SLOT( tcryptCancelled() ) ) ;

	t->ShowUI_1() ;
}

void createvolume::keys( QString key,QStringList keyFiles )
{
	this->enableAll() ;

	if( m_normalVolume ){
		m_key = key ;
		m_keyFiles = keyFiles ;
		m_ui->lineEditPassphrase1->setEnabled( false ) ;
		m_ui->pbOpenKeyFile->setEnabled( false ) ;
	}else{
		m_ui->lineEditPassphrase1->setEnabled( false ) ;
		m_hiddenKey = key ;
		m_hiddenKeyFiles = keyFiles ;
	}
}

void createvolume::cbNormalVolume( int r )
{
	this->setWindowTitle( tr( "create a new volume" ) ) ;

	if( r == 0 ){

		m_ui->pbOpenKeyFile->setEnabled( false ) ;
		m_ui->lineEditPassPhrase2->setEnabled( true ) ;
		m_ui->lineEditPassphrase1->setEnabled( true ) ;
		m_ui->lineEditPassphrase1->clear() ;
		m_ui->lineEditPassPhrase2->clear() ;
		m_ui->lineEditPassphrase1->setEchoMode( QLineEdit::Password ) ;
		m_ui->lineEditPassPhrase2->setEchoMode( QLineEdit::Password ) ;
		m_ui->labelPassPhrase->setText( tr( "key" ) ) ;

		m_ui->labelRepeatPassPhrase->setEnabled( true ) ;
		m_ui->pbOpenKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;

	}else if( r == 1 ){

		m_ui->pbOpenKeyFile->setEnabled( true ) ;
		m_ui->lineEditPassphrase1->clear() ;
		m_ui->lineEditPassPhrase2->clear() ;
		m_ui->lineEditPassphrase1->setEchoMode( QLineEdit::Normal ) ;
		m_ui->lineEditPassPhrase2->setEnabled( false ) ;
		m_ui->labelPassPhrase->setText( tr( "keyfile" ) ) ;
		m_ui->labelRepeatPassPhrase->setEnabled( false ) ;
		m_ui->pbOpenKeyFile->setIcon( QIcon( ":/keyfile.png" ) ) ;
	}else{
		m_ui->pbOpenKeyFile->setEnabled( false ) ;
		m_ui->lineEditPassphrase1->clear() ;
		m_ui->lineEditPassPhrase2->clear() ;
		m_ui->lineEditPassphrase1->setEchoMode( QLineEdit::Normal ) ;
		m_ui->lineEditPassPhrase2->setEnabled( false ) ;
		m_ui->lineEditPassphrase1->setEnabled( false ) ;
		m_ui->lineEditPassphrase1->setText( QString() ) ;
		m_ui->lineEditPassPhrase2->setText( QString() ) ;
		m_ui->labelPassPhrase->setText( tr( "keys" ) ) ;
		m_ui->labelRepeatPassPhrase->setEnabled( false ) ;
		m_ui->pbOpenKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;

		this->tcryptGui( true ) ;
	}
}

void createvolume::cbHiddenVolume( int r )
{
	this->setWindowTitle( tr( "create a new volume" ) ) ;

	if( r == 0 ){

		m_ui->pbHiddenKeyFile->setEnabled( false ) ;
		m_ui->lineEditHiddenKey1->setEnabled( true ) ;
		m_ui->lineEditHiddenKey->setEnabled( true ) ;
		m_ui->lineEditHiddenKey->clear() ;
		m_ui->lineEditHiddenKey1->clear() ;
		m_ui->lineEditHiddenKey->setEchoMode( QLineEdit::Password ) ;
		m_ui->lineEditHiddenKey1->setEchoMode( QLineEdit::Password ) ;
		m_ui->labelHidden->setText( tr( "key" ) ) ;
		m_ui->pbHiddenKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;

	}else if( r == 1 ){

		m_ui->pbHiddenKeyFile->setEnabled( true ) ;
		m_ui->lineEditHiddenKey->clear() ;
		m_ui->lineEditHiddenKey1->clear() ;
		m_ui->lineEditHiddenKey->setEchoMode( QLineEdit::Normal ) ;
		m_ui->lineEditHiddenKey1->setEnabled( false ) ;
		m_ui->labelHidden->setText( tr( "keyfile" ) ) ;
		m_ui->pbHiddenKeyFile->setIcon( QIcon( ":/keyfile.png" ) ) ;
	}else{
		m_ui->pbHiddenKeyFile->setEnabled( false ) ;
		m_ui->lineEditHiddenKey->clear() ;
		m_ui->lineEditHiddenKey1->clear() ;
		m_ui->lineEditHiddenKey->setEchoMode( QLineEdit::Normal ) ;
		m_ui->lineEditHiddenKey1->setEnabled( false ) ;
		m_ui->lineEditHiddenKey->setEnabled( false ) ;
		m_ui->lineEditHiddenKey->setText( QString() ) ;
		m_ui->lineEditHiddenKey1->setText( QString() ) ;
		m_ui->labelHidden->setText( tr( "keys" ) ) ;
		m_ui->pbHiddenKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;

		this->tcryptGui( false ) ;
	}
}

void createvolume::HideUI()
{
	this->hide() ;
	emit HideUISignal() ;
}

void createvolume::enableAll()
{
	m_ui->labelPassPhrase->setEnabled( true ) ;
	m_ui->labelVolumePath->setEnabled( true ) ;
	m_ui->labelRepeatPassPhrase->setEnabled( true ) ;
	m_ui->lineEditPassphrase1->setEnabled( true ) ;
	if( m_ui->cbNormalVolume->currentIndex() == 0 ){
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
	if( m_ui->comboBoxVolumeType->currentIndex() != createvolume::plain ){
		m_ui->comboBoxRNG->setEnabled( true ) ;
	}
	m_ui->cbNormalVolume->setEnabled( true ) ;
	m_ui->labelvolumeOptions->setEnabled( true ) ;
	m_ui->comboBoxOptions->setEnabled( true ) ;

#if TRUECRYPT_CREATE
	if( m_ui->comboBoxVolumeType->currentIndex() == createvolume::normal_and_hidden_truecrypt ){
		m_ui->groupBox->setEnabled( true ) ;
	}
#endif
}

void createvolume::disableAll()
{
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

#if TRUECRYPT_CREATE
	m_ui->groupBox->setEnabled( false ) ;
#endif
}

void createvolume::pbCreateClicked()
{
	DialogMsg msg( this ) ;

	QString volumePath   = m_ui->lineEditVolumePath->text() ;
	QString passphrase_1 = m_ui->lineEditPassphrase1->text() ;
	QString passphrase_2 = m_ui->lineEditPassPhrase2->text() ;

	createvolume::createVolumeType type = createvolume::createVolumeType( m_ui->comboBoxVolumeType->currentIndex() ) ;

	if( volumePath.isEmpty() ){
		return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "volume path field is empty" ) ) ;
	}

	if( type == createvolume::normal_and_hidden_truecrypt ){

		QString x = m_ui->lineEditHiddenSize->text() ;

		if( x.isEmpty() ){
			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "atleast one required field is empty" ) ) ;
		}

		bool ok ;
		x.toUInt( &ok ) ;

		if( !ok ){
			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "illegal character detected in the hidden volume size field" ) ) ;
		}

		if( m_ui->cbHiddenVolume->currentIndex() < 3 ){
			if( m_ui->cbHiddenVolume->currentIndex() == 0 ){
				if( m_ui->lineEditHiddenKey->text() != m_ui->lineEditHiddenKey1->text() ){
					return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "hidden passphrases do not match" ) ) ;
				}
			}else if( m_ui->cbHiddenVolume->currentIndex() == 1 ){
				if( m_ui->lineEditHiddenKey->text().isEmpty() ){
					return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "atleast one required field is empty" ) ) ;
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
			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "passphrases do not match" ) ) ;
		}else{
			source = "-f" ;
			passphrase_1 = utility::keyPath() + "-2" ;

			QString key = m_ui->lineEditPassphrase1->text() ;

			utility::keySend( passphrase_1,key ) ;
		}
	}else if( r == 1 ){
		if( passphrase_1.isEmpty() ){
			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "atleast one required field is empty" ) ) ;
		}else{
			source = "-f" ;
			passphrase_1 = utility::resolvePath( passphrase_1 ).replace( "\"","\"\"\"" ) ;
		}
	}else{
		source = "-f" ;

		passphrase_1 = utility::keyPath() + "-2" ;

		utility::keySend( passphrase_1,m_key ) ;
	}

	switch( type ){
	case createvolume::luks :
		m_volumeType = "luks" ;
		break ;
	case createvolume::plain :
		m_volumeType = "plain" ;
		break ;
	case createvolume::normal_truecrypt :
	case createvolume::normal_and_hidden_truecrypt :
		m_volumeType = "truecrypt" ;
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

	volumePath.replace( "\"","\"\"\"" ) ;

	QString a = QString( ZULUCRYPTzuluCrypt ) ;
	QString b = volumePath ;
	QString c = m_ui->comboBoxFS->currentText() ;
	QString d = m_volumeType;
	QString e = source ;
	QString f = passphrase_1 ;

	QString exe ;
	if( type == createvolume::normal_and_hidden_truecrypt ){

		QString x = m_ui->lineEditHiddenKey->text() ;
		QString y ;

		int k = m_ui->cbHiddenVolume->currentIndex() ;

		if( k == 0 ){

			y = utility::keyPath() + "-1" ;
			utility::keySend( y,x ) ;

		}else if( k == 1 ){

			y = utility::resolvePath( x ).replace( "\"","\"\"\"" ) ;
		}else{
			y = utility::keyPath() + "-1" ;
			utility::keySend( y,m_hiddenKey ) ;
		}

		quint64 r = m_ui->lineEditHiddenSize->text().toULongLong() ;

		switch( m_ui->comboBoxHiddenSize->currentIndex() ){
			case 0 : r *= 1024 * 1024         ; break ;
			case 1 : r *= 1024                ; break ;
			case 2 : r *= 1024 * 1024 * 1024  ; break ;
			default: r *= 1024 * 1024         ; break ;
		}

		QString z = QString::number( r ) ;

		const char * arg = "%1 -c -k -d \"%2\" -z %3 -t %4 %5 \"%6\" -g %7 -e %8 -u \"%9\"" ;
		exe = QString( arg ).arg( a,b,c,d,e,f,g,z,y ) ;
	}else{
		const char * arg = "%1 -c -k -d \"%2\" -z %3 -t %4 %5 \"%6\" -g %7" ;
		exe = QString( arg ).arg( a,b,c,d,e,f,g ) ;
	}

	for( const auto& it : m_keyFiles ){

		QString e = it ;
		e.replace( "\"","\"\"\"" ) ;
		exe += " -F \"" + e + "\"" ;
	}

	for( const auto& it : m_hiddenKeyFiles ){

		QString e = it ;
		e.replace( "\"","\"\"\"" ) ;
		exe += " -V \"" + e + "\"" ;
	}

	m_isWindowClosable = false ;

	this->disableAll() ;

	this->taskFinished( utility::exec( exe ).await() ) ;
}

void createvolume::taskFinished( int st )
{
	DialogMsg msg( this ) ;
	m_isWindowClosable = true ;
	QString x = tr( "volume created successfully." ) ;
	if( m_volumeType.contains( "luks" ) || m_volumeType.contains( "truecrypt" ) ){
		x += tr( "\ncreating a backup of the \"%1\" volume header is strongly advised.\nPlease read documentation on why this is important." ).arg( m_volumeType ) ;
	}
	switch ( st ){
		case 0 : msg.ShowUIOK( tr( "SUCCESS!" ),x ) ;
		return this->HideUI() ;													break  ;
		case 1 : msg.ShowUIOK( tr( "ERROR!" ),tr( "presented file system is not supported,see documentation for more information" ) ) ;	break  ;
		case 2 : msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open a system device in read/write mode,\n\
only root user or members of group zulucrypt-system can do that" ) ) ;									break  ;
		case 3 : msg.ShowUIOK( tr( "ERROR!" ),tr( "could not create an encrypted volume" ) ) ;					break  ;
		case 4 : msg.ShowUIOK( tr( "ERROR!" ),tr( "could not open volume for writing" ) ) ;					break  ;
		case 5 : msg.ShowUIOK( tr( "ERROR!" ),tr( "there seem to be an opened mapper associated with the device" ) ) ;		break  ;
		case 6 : msg.ShowUIOK( tr( "ERROR!" ),tr( "can not create a volume on a mounted device" ) ) ;				break  ;
		case 7 : msg.ShowUIOK( tr( "ERROR!" ),tr( "container file must be bigger than 3MB" ) ) ;					break  ;
		case 8 : msg.ShowUIOK( tr( "ERROR!" ),tr( "%1 not found" ).arg( ZULUCRYPTmkfs ) ) ;					break  ;
		case 9 : msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient memory to hold your response" ) ) ;				break  ;
		case 10: msg.ShowUIOK( tr( "ERROR!" ),tr( "operation terminated per user request" ) ) ; 				break  ;													break  ;
		case 11: msg.ShowUIOK( tr( "ERROR!" ),tr( "could not get passphrase in silent mode" ) ) ;				break  ;
		case 12: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient memory to hold the passphrase" ) ) ;				break  ;
		case 13: msg.ShowUIOK( tr( "ERROR!" ),tr( "passphrases do not match" ) ) ;						break  ;
		case 14: msg.ShowUIOK( tr( "ERROR!" ),tr( "invalid path to key file" ) ) ;						break  ;
		case 15: msg.ShowUIOK( tr( "ERROR!" ),tr( "could not get a key from a key file" ) ) ;					break  ;
		case 16: msg.ShowUIOK( tr( "ERROR!" ),tr( "couldnt get enought memory to hold the key file" ) ) ;			break  ;
		case 17: msg.ShowUIOK( tr( "ERROR!" ),tr( "could not get a key from a socket" ) ) ;					break  ;
		case 18: msg.ShowUIOK( tr( "ERROR!" ),tr( "one or more required argument(s) for this operation is missing" ) ) ;	break  ;
		case 19: msg.ShowUIOK( tr( "ERROR!" ),tr( "can not get passphrase in silent mode" ) ) ;					break  ;
		case 20: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient memory to hold passphrase" ) ) ;				break  ;
		case 21: msg.ShowUIOK( tr( "ERROR!" ),tr( "passphrases do not match") ) ;						break  ;
		case 22: msg.ShowUIOK( tr( "ERROR!" ),tr( "failed to create a volume" ) ) ;						break  ;
		case 23: msg.ShowUIOK( tr( "ERROR!" ),tr( "wrong argument detected for tcrypt volume" ) ) ;				break  ;
		case 110:msg.ShowUIOK( tr( "ERROR!" ),tr( "could not find any partition with the presented UUID" ) ) ;			break  ;
		default: msg.ShowUIOK( tr( "ERROR!" ),tr( "unrecognized ERROR! with status number %1 encountered" ).arg( st ) ) ;
	}

	this->enableAll() ;
}

createvolume::~createvolume()
{
	delete m_ui ;
}

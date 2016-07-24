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

#include "cryptfiles.h"
#include "utility.h"
#include "../zuluCrypt-cli/constants.h"

#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <iostream>
#include <QFile>
#include <QKeyEvent>

#include <QFileDialog>
#include <QDir>

#include <functional>
#include <utility>

#include "ui_cryptfiles.h"
#include "utility.h"
#include "openvolume.h"
#include "dialogmsg.h"
#include "lxqtwallet.h"

#include "plugin.h"

cryptfiles::cryptfiles( QWidget * parent ) :QDialog( parent ),m_ui( new Ui::cryptfiles )
{
	m_ui->setupUi( this ) ;
	this->setFont( parent->font() ) ;
	this->setFixedSize( this->size() ) ;

	m_ui->progressBar->setMinimum( 0 ) ;
	m_ui->progressBar->setMaximum( 100 ) ;
	m_ui->progressBar->setValue( 0 ) ;

	m_ui->pbOpenFolder->setIcon( QIcon( ":/folder.png" ) ) ;
	m_ui->pushButtonFile->setIcon( QIcon( ":/file.png" ) ) ;

	connect( m_ui->pbCreate,SIGNAL( clicked() ),this,SLOT( pbCreate() ) ) ;
	connect( m_ui->pushButtonFile,SIGNAL( clicked() ),this,SLOT( pbOpenFile() ) ) ;
	connect( m_ui->pbOpenFolder,SIGNAL( clicked() ),this,SLOT( pbOpenFolder() ) ) ;
	connect( m_ui->comboBox,SIGNAL( activated( int ) ),this,SLOT( cbChanged( int ) ) ) ;
	connect( m_ui->lineEditSourcePath,SIGNAL( textChanged( QString ) ),this,SLOT( sourceTextChanged( QString ) ) ) ;
	connect( m_ui->pushButtonKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	connect( this,SIGNAL( progressUpdate( int ) ),this,SLOT( progressBarUpdate( int ) ) ) ;

	this->cbChanged( 0 ) ;

	m_OperationInProgress = false ;

	m_ui->lineEditPass_2->setEchoMode( QLineEdit::Password ) ;

	m_ui->lineEditSourcePath->setFocus() ;

	m_ui->lineEditDestinationPath->setText( QString() ) ;

	m_ui->lineEditDestinationPath->setEnabled( false ) ;
	m_ui->lineEditSourcePath->setEnabled( false ) ;

	this->installEventFilter( this ) ;
}

bool cryptfiles::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void cryptfiles::sourceTextChanged( QString source )
{
	QString dest ;

	if( m_operation == "-E" ){

		dest = source.split( "/" ).last() + ".zC" ;
	}else{
		dest = source.split( "/" ).last() ;
	}

	auto p = m_ui->lineEditDestinationPath->text().split( "/" ) ;

	auto size = p.size() ;

	QString path ;

	for( int i = 0 ; i < size - 1 ; i++ ){

		path += p.at( i ) + "/" ;
	}

	path += dest ;

	if( m_operation == "-D" ){

		if( path.endsWith( ".zc" ) || path.endsWith( ".zC" ) ){

			path = path.mid( 0,path.size() - 3 ) ;
		}
	}

	m_ui->lineEditDestinationPath->setText( path ) ;
}

QString cryptfiles::destinationPath( const QString& e )
{
	if( e.isEmpty() ){

		return utility::homePath() + "/" ;
	}else{
		int r = e.lastIndexOf( '/' ) ;

		if( r == -1 ){

			return utility::homePath() + "/" ;
		}else{
			auto x = e ;
			x.truncate( r ) ;
			return x + "/" ;
		}
	}
}

void cryptfiles::encrypt()
{
	m_operation = "-E" ;

	this->setWindowTitle( tr( "Create An Encrypted Version Of A File" ) ) ;

	this->show() ;
}

void cryptfiles::decrypt()
{
	m_operation = "-D" ;
	m_ui->labelKey2->setEnabled( false ) ;
	m_ui->lineEditPass_2->setEnabled( false ) ;

	this->setWindowTitle( tr( "Create A Decrypted Version Of An encrypted File" ) ) ;
	this->show() ;
}

void cryptfiles::decrypt( const QString& e )
{
	m_ui->lineEditSourcePath->setText( e ) ;
	m_operation = "-D" ;
	m_ui->labelKey2->setEnabled( false ) ;
	m_ui->lineEditPass_2->setEnabled( false ) ;
	m_ui->lineEditDestinationPath->setText( this->destinationPath( e )  ) ;

	this->sourceTextChanged( e ) ;
	this->setWindowTitle( tr( "Create A Decrypted Version Of An encrypted File" ) ) ;
	this->show() ;
}

void cryptfiles::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

void cryptfiles::pbCancel()
{
	this->HideUI() ;
}

void cryptfiles::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void cryptfiles::enableAll()
{
	if( m_operation == "-E" ){

		m_ui->labelKey2->setEnabled( true ) ;
		m_ui->lineEditPass_2->setEnabled( true ) ;
	}

	m_ui->labelKey->setEnabled( true ) ;
	m_ui->lineEditPass_1->setEnabled( true ) ;

	m_ui->labelDestinationPath->setEnabled( true ) ;
	m_ui->labelSourcePath->setEnabled( true ) ;
	m_ui->pbCreate->setEnabled( true ) ;
	m_ui->pbOpenFolder->setEnabled( true ) ;
	m_ui->pushButtonFile->setEnabled( true ) ;
	m_ui->pushButtonCancel->setEnabled( true ) ;
	m_ui->comboBox->setEnabled( true ) ;
	m_ui->labelProgressBar->setEnabled( true ) ;
}

void cryptfiles::disableAll()
{
	m_ui->lineEditPass_1->setEnabled( false ) ;
	m_ui->lineEditPass_2->setEnabled( false ) ;
	m_ui->labelKey2->setEnabled( false ) ;
	m_ui->labelKey->setEnabled( false ) ;
	m_ui->labelDestinationPath->setEnabled( false ) ;
	m_ui->labelSourcePath->setEnabled( false ) ;
	m_ui->lineEditDestinationPath->setEnabled( false ) ;
	m_ui->lineEditSourcePath->setEnabled( false ) ;
	m_ui->pbCreate->setEnabled( false ) ;
	m_ui->pbOpenFolder->setEnabled( false ) ;
	m_ui->pushButtonFile->setEnabled( false ) ;
	m_ui->comboBox->setEnabled( false ) ;
	m_ui->labelProgressBar->setEnabled( false ) ;
	m_ui->pushButtonCancel->setEnabled( false ) ;
}

void cryptfiles::pbCreate()
{
	DialogMsg msg( this ) ;

	auto source = utility::resolvePath( m_ui->lineEditSourcePath->text() ) ;

	if( source.isEmpty() ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Path to source field is empty" ) ) ;
	}

	auto dest = utility::resolvePath( m_ui->lineEditDestinationPath->text() ) ;

	if( !utility::pathExists( source ) ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Invalid path to source file" ) ) ;
	}
	if( utility::pathExists( dest ) ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Destination path already taken" ) ) ;
	}
	if( !utility::canCreateFile( dest ) ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "You dont seem to have writing access to the destination folder" ) ) ;
	}

	auto key_1 = m_ui->lineEditPass_1->text() ;
	auto key_2 = m_ui->lineEditPass_2->text() ;

	QString keySource ;

	if( m_ui->comboBox->currentIndex() == 1 ){

		if( utility::pathExists( key_1 ) ){

			keySource = "-f" ;
		}else{
			return msg.ShowUIOK( tr( "ERROR!" ),tr( "Invalid path to key file" ) ) ;
		}

	}else{
		keySource = "-p" ;

		if( key_1.isEmpty() ){

			return msg.ShowUIOK( tr( "ERROR!" ),tr( "First key field is empty" ) ) ;
		}
		if( m_operation == "-E" ){

			if( key_2.isEmpty() ){

				return msg.ShowUIOK( tr( "ERROR!" ),tr( "Second key field is empty" ) ) ;
			}
			if( key_1 != key_2 ){

				return msg.ShowUIOK( tr( "ERROR!" ),tr( "Keys do not match" ) ) ;
			}
		}
	}

	this->disableAll() ;

	if( m_operation == "-D" && source.endsWith( ".zc" ) ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "These very old encrypted files are no longer supported" ) ) ;
	}else{
		#define _constPtr toLatin1().constData()

		bool e = m_operation == "-E" ;

		if( keySource == "-f" ){

			QFile f( key_1 ) ;
			f.open( QIODevice::ReadOnly ) ;
			auto r = f.readAll() ;

			this->cryptFile( source._constPtr,dest._constPtr,r.constData(),r.size(),e ) ;
		}else{
			this->cryptFile( source._constPtr,dest._constPtr,key_1._constPtr,key_1.size(),e ) ;
		}
	}
}

void cryptfiles::cryptFile( const char * s,const char * d,const char * k,unsigned long l,bool encrypt )
{
	using function_t = std::function< int( int ) > ;

	function_t foo = [ this ]( int e ){ emit progressUpdate( e ) ; return 0 ; } ;

	auto f = reinterpret_cast< void * >( &foo ) ;

	auto u = []( int e,void * f )
	{
		auto function = reinterpret_cast< function_t * >( f ) ;

		return ( *function )( e ) ;
	} ;

	auto r = Task::await< lxqt_wallet_error >( [ & ]{

		auto r = u_int32_t( l ) ;

		if( encrypt ){

			return lxqt_wallet_create_encrypted_file( k,r,s,d,u,f ) ;
		}else{
			return lxqt_wallet_create_decrypted_file( k,r,s,d,u,f ) ;
		}
	} ) ;

	if( r == lxqt_wallet_wrong_password ){

		this->taskFinished( cryptfiles::wrongKey ) ;

	}else if( r == lxqt_wallet_no_error ){

		if( m_ui->progressBar->value() == 100 ){

			utility::changeFileOwner( d ) ;

			if( encrypt ){

				this->taskFinished( cryptfiles::encryptSuccess ) ;
			}else{
				this->taskFinished( cryptfiles::decryptSuccess ) ;
			}
		}else{
			this->taskFinished( cryptfiles::quit ) ;
		}
	}else{
		/*
		 * we shouldnt get here and we return a bogus return value for lack of better
		 * alternative
		 */
		this->taskFinished( cryptfiles::openMapperReadFail ) ;
	}
}

void cryptfiles::disableCancel()
{
	m_ui->pushButtonCancel->setEnabled( false ) ;
}

void cryptfiles::enableCancel()
{
	m_ui->pushButtonCancel->setEnabled( true ) ;
}

void cryptfiles::titleUpdate( QString title )
{
	this->setWindowTitle( title ) ;
}

void cryptfiles::progressBarUpdate( int i )
{
	m_ui->progressBar->setValue( i ) ;
}

void cryptfiles::pbOpenFolder( void )
{
	QString p = tr( "Select Path to put destination file" ) ;
	QString Z = QFileDialog::getExistingDirectory( this,p,utility::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( Z.isEmpty() ){
		Z = utility::homePath() ;
	}
	QString path ;

	if( m_operation == "-E" ){

		path = Z + "/" + m_ui->lineEditSourcePath->text().split( "/" ).last() + ".zC" ;
	}else{
		path = Z + "/" + m_ui->lineEditSourcePath->text().split( "/" ).last() ;
		path.chop( 3 ) ;
	}

	m_ui->lineEditDestinationPath->setText( path ) ;

	if( m_ui->lineEditSourcePath->text().isEmpty() ){

		m_ui->lineEditSourcePath->setFocus() ;
	}else{
		m_ui->pbCreate->setFocus() ;
	}
}

void cryptfiles::cbChanged( int r )
{
	auto _key_ui = [ this ](){

		m_ui->lineEditPass_1->setToolTip( tr( "Enter A Key" ) ) ;
		m_ui->pushButtonKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;
		m_ui->pushButtonKeyFile->setEnabled( false ) ;
		m_ui->lineEditPass_1->clear() ;
		m_ui->lineEditPass_2->clear() ;
		m_ui->lineEditPass_1->setEchoMode( QLineEdit::Password ) ;
		m_ui->lineEditPass_1->setFocus() ;
		m_ui->labelKey->setText( tr( "Key" ) ) ;
		m_ui->labelKey2->setText( tr( "Repeat Key" ) ) ;

		m_ui->lineEditPass_1->setEnabled( true ) ;

		if( m_operation == "-E" ){

			m_ui->labelKey2->setEnabled( true ) ;
			m_ui->lineEditPass_2->setEnabled( true ) ;
		}
	} ;

	if( r == 0 ){

		_key_ui() ;

	}else if( r == 1 ){

		m_ui->lineEditPass_1->setToolTip( tr( "Enter A Path To A Keyfile Location" ) ) ;
		m_ui->labelKey->setText( tr( "keyfile path" ) ) ;
		m_ui->pushButtonKeyFile->setIcon( QIcon( ":/keyfile.png" ) ) ;
		m_ui->lineEditPass_1->setEnabled( true ) ;
		m_ui->lineEditPass_2->setEnabled( false ) ;
		m_ui->pushButtonKeyFile->setEnabled( true ) ;
		m_ui->labelKey2->setEnabled( false ) ;
		m_ui->lineEditPass_1->clear() ;
		m_ui->lineEditPass_2->clear() ;
		m_ui->lineEditPass_1->setEchoMode( QLineEdit::Normal ) ;
		m_ui->lineEditPass_1->setFocus() ;
	}else{
		_key_ui() ;

		m_ui->lineEditPass_1->setEnabled( false ) ;
		m_ui->lineEditPass_2->setEnabled( false ) ;

		plugin::instance( this,plugins::plugin::hmac_key,[ this ]( const QString& key ){

			m_ui->lineEditPass_1->setText( key ) ;
			m_ui->lineEditPass_2->setText( key ) ;

			if( key.isEmpty() ){

				m_ui->comboBox->setCurrentIndex( 0 ) ;
				this->cbChanged( 0 ) ;
			}

		},tr( "Generate a key made up of a passphrase and a keyfile" ) ) ;
	}
}

void cryptfiles::pbOpenFile()
{
	QString Z ;

	if( m_operation == "-E" ){

		Z = QFileDialog::getOpenFileName( this,tr( "Select A File You Want To Encrypt" ),utility::homePath(),0 ) ;
	}else{
		auto x = tr( "zuluCrypt encrypted files ( *.zc ) ;; All Files ( * )" ) ;
		Z = QFileDialog::getOpenFileName( this,tr( "Select A File You Want To Decrypt" ),utility::homePath(),x ) ;
	}
	m_ui->lineEditSourcePath->setText( Z ) ;
	m_ui->lineEditDestinationPath->setText( this->destinationPath( Z ) ) ;
	this->sourceTextChanged( Z ) ;
	m_ui->lineEditPass_1->setFocus() ;
}

void cryptfiles::pbKeyFile()
{
	auto e = QFileDialog::getOpenFileName( this,tr( "Select A Keyfile" ),utility::homePath(),0 ) ;

	m_ui->lineEditPass_1->setText( e ) ;

	if( m_ui->lineEditSourcePath->text().isEmpty() ){

		m_ui->lineEditSourcePath->setFocus() ;
	}else{
		m_ui->pbCreate->setFocus() ;
	}
}

void cryptfiles::taskFinished( int st )
{
	DialogMsg msg( this ) ;

	m_OperationInProgress = false ;

	cryptfiles::status status = cryptfiles::status( st ) ;

	switch( status ){
	case cryptfiles::encryptSuccess        : msg.ShowUIOK( tr( "SUCCESS" ),tr( "Encrypted file created successfully" ) )     ;
		 return this->HideUI() ;
	case cryptfiles::md5Pass               :
	case cryptfiles::decryptSuccess        : msg.ShowUIOK( tr( "SUCCESS" ),tr( "Decrypted file created successfully" ) )	;
		 return this->HideUI() ;
	case cryptfiles::openKeyFileReadFail   : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not open keyfile for reading" ) )	;
		 break ;
	case cryptfiles::openMapperFail : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not open encryption routines" ) )	;
		break ;
	case cryptfiles::destinationFileExists : msg.ShowUIOK( tr( "ERROR!" ),tr( "File or folder already exist at destination address" ) )    ;
		break ;
	case cryptfiles::OpenDestinationFail   :
	case cryptfiles::createFileFail        : msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to create destination file" ) )      ;
		break ;
	case cryptfiles::wrongKey              : msg.ShowUIOK( tr( "ERROR!" ),tr( "Presented key did not match the encryption key" ) )         ;
		break ;
	case cryptfiles::quit                  : msg.ShowUIOK( tr( "INFO!" ),tr( "Operation terminated per user request" ) )                   ;
		return this->HideUI() ;
	case cryptfiles::OpenSourceFail        : msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to open source file for reading" ) ) ;
		break ;
	case cryptfiles::md5Fail               : msg.ShowUIOK( tr( "WARNING"),tr( "Decrypted file created successfully but md5 checksum failed,file maybe corrupted" ) ) ;
		return this->HideUI() ;
	case cryptfiles::openMapperReadFail    : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not open reading encryption routines" ) )	;
		break ;
	case cryptfiles::openMapperWriteFail   : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not open writing encryption routines" ) )	;
		break ;
	case cryptfiles::closeMapperFail       : msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to close encryption routine" ) )		;
		break ;
	case cryptfiles::unset                 :
	case cryptfiles::success               :
	case cryptfiles::QProcessFail          :
		break ;
	}

	this->enableAll() ;

	if( status == cryptfiles::wrongKey  ){

		m_ui->lineEditPass_1->clear() ;
		m_ui->lineEditPass_1->setFocus() ;
	}
}

cryptfiles::~cryptfiles()
{
	delete m_ui ;
}

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
#include "crypttask.h"
#include "dialogmsg.h"
#include "lxqt_wallet/backend/lxqtwallet.h"

cryptfiles::cryptfiles( QWidget * parent ) :QDialog( parent ),m_ui( new Ui::cryptfiles )
{
	m_ui->setupUi( this ) ;
	this->setFont( parent->font() ) ;
	this->setFixedSize( this->size() ) ;

	m_ui->progressBar->setMinimum( 0 ) ;
	m_ui->progressBar->setMaximum( 100 ) ;
	m_ui->progressBar->setValue( 0 ) ;

	m_ui->pbOpenFolder->setIcon( QIcon( QString( ":/folder.png" ) ) ) ;
	m_ui->pushButtonFile->setIcon( QIcon( QString( ":/file.png" ) ) ) ;

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
	if( utility::eventFilter( this,watched,event ) ){
		this->HideUI() ;
		return true ;
	}else{
		return false ;
	}
}

void cryptfiles::sourceTextChanged( QString source )
{
	QString dest ;

	if( m_operation == QString( "-E" ) ){
		dest = source.split( "/" ).last() + QString( ".zC" ) ;
	}else{
		dest = source.split( "/" ).last() ;
	}

	QStringList p = m_ui->lineEditDestinationPath->text().split( "/" ) ;

	int size = p.size() ;
	QString path = QString( "" ) ;
	for( int i = 0 ; i < size - 1 ; i++ ){
		path += p.at( i ) + QString( "/" ) ;
	}
	path += dest ;

	if( m_operation == QString( "-D" ) ){
		if( path.endsWith( QString( ".zc" ) ) || path.endsWith( QString( ".zC" ) ) ){
			path = path.mid( 0,path.size() - 3 ) ;
		}
	}

	m_ui->lineEditDestinationPath->setText( path ) ;
}

QString cryptfiles::destinationPath( const QString& e )
{
	if( e.isEmpty() ){

		return QDir::homePath() + "/" ;
	}else{
		int r = e.lastIndexOf( '/' ) ;

		if( r == -1 ){
			return QDir::homePath() + "/" ;
		}else{
			QString x = e ;
			x.truncate( r ) ;
			return x + "/" ;
		}
		return QDir::homePath() + "/" ;
	}
}

void cryptfiles::encrypt()
{
	m_operation = QString( "-E" ) ;
	this->setWindowTitle( tr( "create encrypted version of a file" ) ) ;
	this->show() ;
}

void cryptfiles::decrypt()
{
	m_operation = QString( "-D" ) ;
	m_ui->labelKey2->setEnabled( false ) ;
	m_ui->lineEditPass_2->setEnabled( false ) ;
	this->setWindowTitle( tr( "create decrypted version of an encrypted file" ) ) ;
	this->show() ;
}

void cryptfiles::decrypt( const QString& e )
{
	m_ui->lineEditSourcePath->setText( e ) ;
	m_operation = QString( "-D" ) ;
	m_ui->labelKey2->setEnabled( false ) ;
	m_ui->lineEditPass_2->setEnabled( false ) ;
	m_ui->lineEditDestinationPath->setText( this->destinationPath( e )  ) ;
	this->sourceTextChanged( e ) ;
	this->setWindowTitle( tr( "create decrypted version of an encrypted file" ) ) ;
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
	if( m_OperationInProgress ){
		m_task->terminate() ;
	}else{
		emit this->HideUISignal() ;
		this->hide() ;
	}
}

void cryptfiles::enableAll()
{
	if( m_operation == QString( "-E" ) ){
		m_ui->labelKey2->setEnabled( true ) ;
		m_ui->lineEditPass_2->setEnabled( true ) ;
	}

	m_ui->labelKey->setEnabled( true ) ;
	m_ui->lineEditPass_1->setEnabled( true ) ;

	m_ui->labelDestinationPath->setEnabled( true ) ;
	m_ui->labelSourcePath->setEnabled( true ) ;
	m_ui->lineEditDestinationPath->setEnabled( true ) ;
	m_ui->lineEditSourcePath->setEnabled( true ) ;
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

	QString source = utility::resolvePath( m_ui->lineEditSourcePath->text() ) ;

	if( source.isEmpty() ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "path to source field is empty" ) ) ;
	}
	QString dest = utility::resolvePath( m_ui->lineEditDestinationPath->text() ) ;

	if( !utility::pathExists( source ) ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "invalid path to source file" ) ) ;
	}
	if( utility::pathExists( dest ) ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "destination path already taken" ) ) ;
	}
	QString key_1 = m_ui->lineEditPass_1->text() ;
	QString key_2 = m_ui->lineEditPass_2->text() ;

	QString keySource ;
	if( m_ui->comboBox->currentIndex() == 0 ){
		keySource = "-p" ;
		if( key_1.isEmpty() ){
			return msg.ShowUIOK( tr( "ERROR!" ),tr( "first key field is empty" ) ) ;
		}
		if( m_operation == "-E" ){
			if( key_2.isEmpty() ){
				return msg.ShowUIOK( tr( "ERROR!" ),tr( "second key field is empty" ) ) ;
			}
			if( key_1 != key_2 ){
				return msg.ShowUIOK( tr( "ERROR!" ),tr( "keys do not match" ) ) ;
			}
		}
	}else{
		keySource = "-f" ;
		if( !utility::pathExists( key_1 ) ){
			return msg.ShowUIOK( tr( "ERROR!" ),tr( "invalid path to key file" ) ) ;
		}
	}

	this->disableAll() ;

	if( m_operation == "-D" && source.endsWith( ".zc" ) ){
		/*
		 * deprecated code path used to decrypt volumes created with zuluCrypt < 4.6.9
		 */
		m_task = new CryptTask( source,dest,keySource,key_1,m_operation ) ;
		this->cryptFileDeprecatedFunctionality() ;
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

void cryptfiles::cryptFile( const char * s,const char * d,const char * key,unsigned long l,bool encrypt )
{
	struct _progress
	{
		_progress( std::function< int( int ) > function )
		{
			update = std::move( function ) ;
		}

		std::function< int( int ) > update ;
	} ;

	_progress progress( [ this ]( int e ){ emit progressUpdate( e ) ; return 0 ; } ) ;

	auto f = reinterpret_cast< void * >( &progress ) ;

	auto _update = []( int e,void * f )
	{
		auto r = reinterpret_cast< _progress * >( f ) ;
		return r->update( e ) ;
	} ;

	auto r = Task::await< lxqt_wallet_error >( [ & ]{

		auto r = u_int32_t( l ) ;

		if( encrypt ){

			return lxqt_wallet_create_encrypted_file( key,r,s,d,_update,f ) ;
		}else{
			return lxqt_wallet_create_decrypted_file( key,r,s,d,_update,f ) ;
		}
	} ) ;

	if( r == lxqt_wallet_wrong_password ){

		this->taskFinished( CryptTask::wrongKey ) ;

	}else if( r == lxqt_wallet_no_error ){

		if( m_ui->progressBar->value() == 100 ){

			if( encrypt ){

				this->taskFinished( CryptTask::encryptSuccess ) ;
			}else{
				this->taskFinished( CryptTask::decryptSuccess ) ;
			}
		}else{
			this->taskFinished( CryptTask::quit ) ;
		}
	}else{
		/*
		 * we shouldnt get here and we return a bogus return value for lack of better
		 * alternative
		 */
		this->taskFinished( CryptTask::openMapperReadFail ) ;
	}
}

void cryptfiles::cryptFileDeprecatedFunctionality()
{
	m_OperationInProgress = true ;

	connect( m_task,SIGNAL( complete( int ) ),this,SLOT( taskFinished( int ) ) ) ;
	connect( m_task,SIGNAL( progressUpdate( int ) ),this,SLOT( progressBarUpdate( int ) ) ) ;
	connect( m_task,SIGNAL( titleUpdate( QString ) ),this,SLOT( titleUpdate( QString ) ) ) ;
	connect( m_task,SIGNAL( enableCancel() ),this,SLOT( enableCancel() ) ) ;
	connect( m_task,SIGNAL( disableCancel() ),this,SLOT( disableCancel() ) ) ;

	m_task->start() ;
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
	QString Z = QFileDialog::getExistingDirectory( this,p,QDir::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( Z.isEmpty() ){
		Z = QDir::homePath() ;
	}
	QString path ;
	if( m_operation == QString( "-E" ) ){
		path = Z + QString( "/" ) + m_ui->lineEditSourcePath->text().split( "/" ).last() + QString( ".zC" ) ;
	}else{
		path = Z + QString( "/" ) + m_ui->lineEditSourcePath->text().split( "/" ).last() ;
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
	if( r == 0 ){

		m_ui->lineEditPass_1->setToolTip( tr( "enter a key" ) ) ;
		m_ui->pushButtonKeyFile->setIcon( QIcon( QString( ":/passphrase.png" ) ) ) ;
		m_ui->pushButtonKeyFile->setEnabled( false ) ;
		m_ui->lineEditPass_1->clear() ;
		m_ui->lineEditPass_2->clear() ;
		m_ui->lineEditPass_1->setEchoMode( QLineEdit::Password ) ;
		m_ui->lineEditPass_1->setFocus() ;
		m_ui->labelKey->setText( tr( "key" ) ) ;
		m_ui->labelKey2->setText( tr( "repeat key" ) ) ;

		if( m_operation == QString( "-E" ) ){
			m_ui->labelKey2->setEnabled( true ) ;
			m_ui->lineEditPass_2->setEnabled( true ) ;
		}
	}else{

		m_ui->lineEditPass_1->setToolTip( tr( "enter a path to a keyfile location" ) ) ;
		m_ui->labelKey->setText( tr( "keyfile path" ) ) ;
		m_ui->pushButtonKeyFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) ) ;
		m_ui->lineEditPass_2->setEnabled( false ) ;
		m_ui->pushButtonKeyFile->setEnabled( true ) ;
		m_ui->labelKey2->setEnabled( false ) ;
		m_ui->lineEditPass_1->clear() ;
		m_ui->lineEditPass_2->clear() ;
		m_ui->lineEditPass_1->setEchoMode( QLineEdit::Normal ) ;
		m_ui->lineEditPass_1->setFocus() ;
	}
}

void cryptfiles::pbOpenFile()
{
	QString Z ;
	if( m_operation == QString( "-E" ) ){
		Z = QFileDialog::getOpenFileName( this,tr( "select a file you want to encrypt" ),QDir::homePath(),0 ) ;
	}else{
		QString x = tr( "zuluCrypt encrypted files ( *.zc ) ;; All Files ( * )" ) ;
		Z = QFileDialog::getOpenFileName( this,tr( "select a file you want to decrypt" ),QDir::homePath(),x ) ;
	}
	m_ui->lineEditSourcePath->setText( Z ) ;
	m_ui->lineEditDestinationPath->setText( this->destinationPath( Z ) ) ;
	this->sourceTextChanged( Z ) ;
	m_ui->lineEditPass_1->setFocus() ;
}

void cryptfiles::pbKeyFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "select a keyfile" ),QDir::homePath(),0 ) ;

	m_ui->lineEditPass_1->setText( Z ) ;
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

	CryptTask::status status = CryptTask::status( st ) ;

	switch( status ){
	case CryptTask::encryptSuccess        : msg.ShowUIOK( tr( "SUCCESS" ),tr( "encrypted file created successfully" ) )     ;
		 return this->HideUI() ;
	case CryptTask::md5Pass               :
	case CryptTask::decryptSuccess        : msg.ShowUIOK( tr( "SUCCESS" ),tr( "decrypted file created successfully" ) )	;
		 return this->HideUI() ;
	case CryptTask::openKeyFileReadFail   : msg.ShowUIOK( tr( "ERROR!" ),tr( "could not open keyfile for reading" ) )	;
		 break ;
	case CryptTask::openMapperFail : msg.ShowUIOK( tr( "ERROR!" ),tr( "could not open encryption routines" ) )	;
		break ;
	case CryptTask::destinationFileExists : msg.ShowUIOK( tr( "ERROR!" ),tr( "file or folder already exist at destination address" ) )    ;
		break ;
	case CryptTask::OpenDestinationFail   :
	case CryptTask::createFileFail        : msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to create destination file" ) )      ;
		break ;
	case CryptTask::wrongKey              : msg.ShowUIOK( tr( "ERROR!" ),tr( "presented key did not match the encryption key" ) )         ;
		break ;
	case CryptTask::quit                  : msg.ShowUIOK( tr( "INFO!" ),tr( "operation terminated per user request" ) )                   ;
		return this->HideUI() ;
	case CryptTask::OpenSourceFail        : msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open source file for reading" ) ) ;
		break ;
	case CryptTask::md5Fail               : msg.ShowUIOK( tr( "WARNING"),tr( "decrypted file created successfully but md5 checksum failed,file maybe corrupted" ) ) ;
		return this->HideUI() ;
	case CryptTask::openMapperReadFail    : msg.ShowUIOK( tr( "ERROR!" ),tr( "could not open reading encryption routines" ) )	;
		break ;
	case CryptTask::openMapperWriteFail   : msg.ShowUIOK( tr( "ERROR!" ),tr( "could not open writing encryption routines" ) )	;
		break ;
	case CryptTask::closeMapperFail       : msg.ShowUIOK( tr( "ERROR!" ),tr( "failed to close encryption routine" ) )		;
		break ;
	case CryptTask::unset                 :
	case CryptTask::success               :
	case CryptTask::QProcessFail          :
		break ;
	}

	this->enableAll() ;

	if( status == CryptTask::wrongKey  ){
		m_ui->lineEditPass_1->clear() ;
		m_ui->lineEditPass_1->setFocus() ;
	}
}

cryptfiles::~cryptfiles()
{
	delete m_ui ;
}

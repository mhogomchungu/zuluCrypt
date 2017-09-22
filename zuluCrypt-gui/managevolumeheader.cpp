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

#include "managevolumeheader.h"
#include "truecrypt_support_1.h"
#include "utility.h"
#include "../zuluCrypt-cli/constants.h"

#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QKeyEvent>

#include <QDebug>
#include <QFileDialog>

#include "ui_managevolumeheader.h"
#include "task.h"
#include "utility.h"
#include "openvolume.h"
#include "dialogmsg.h"

managevolumeheader::managevolumeheader( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::managevolumeheader )
{
	m_ui->setupUi( this ) ;
	this->setFont( parent->font() ) ;
	this->setFixedSize( this->size() ) ;

	//m_ui->pbOpenFolder->setIcon( QIcon( QString( ":/folder.png" ) ) ) ;
	m_ui->pushButtonPartition->setIcon( QIcon( ":/partition.png" ) ) ;
	m_ui->pushButtonFile->setIcon( QIcon( ":/file.png" ) ) ;

	m_ui->lineEditDevicePath->setFocus() ;

	this->cbVolumeHeader( 0 ) ;

	connect( m_ui->cbVolumeType,SIGNAL( activated( int ) ),this,SLOT( cbVolumeType( int ) ) );
	connect( m_ui->cbVolumeHeader,SIGNAL( activated( int ) ),this,SLOT( cbVolumeHeader( int ) ) );
	connect( m_ui->pbCreate,SIGNAL( clicked() ),this,SLOT( pbCreate() ) ) ;
	connect( m_ui->pbOpenFolder,SIGNAL( clicked() ),this,SLOT( pbOpenLuksHeaderBackUp() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pushButtonFile,SIGNAL( clicked() ),this,SLOT( pbOpenFile() ) ) ;
	connect( m_ui->pushButtonPartition,SIGNAL( clicked() ),this,SLOT( pbOpenPartition() ) ) ;
	connect( m_ui->lineEditDevicePath,SIGNAL( textChanged( QString ) ),this,SLOT( backUpHeaderNameChange( QString ) ) ) ;
	connect( m_ui->cbKeySource,SIGNAL( activated( int ) ),this,SLOT( cbKeyType( int ) ) ) ;
	connect( m_ui->pBKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;

	m_OperationInProgress = false ;

	m_veraCryptWarning.setWarningLabel( m_ui->labelVeraCryptWarning ) ;

	this->installEventFilter( this ) ;
}

bool managevolumeheader::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void managevolumeheader::cbKeyType( int e )
{
	if( e == 0 ){

		m_ui->pBKeyFile->setEnabled( false ) ;
		m_ui->lineEditPassWord->setEchoMode( QLineEdit::Password ) ;
	}else{
		m_ui->pBKeyFile->setEnabled( true ) ;
		m_ui->lineEditPassWord->setEchoMode( QLineEdit::Normal ) ;
	}

	m_ui->lineEditPassWord->clear() ;
}

void managevolumeheader::cbVolumeHeader( int e )
{
	bool enable = ( e != 0 ) ;

	m_ui->labelKeyType->setEnabled( enable ) ;
	m_ui->lineEditPassWord->clear() ;
	m_ui->pBKeyFile->setEnabled( enable ) ;
	m_ui->lineEditPassWord->setEnabled( enable ) ;
	m_ui->labelBackUpHeader->setEnabled( true ) ;
	m_ui->labelDevicePath->setEnabled( true ) ;
	m_ui->lineEditBackUpName->setEnabled( true ) ;
	m_ui->lineEditDevicePath->setEnabled( true ) ;
	m_ui->labelVolumeType->setEnabled( enable ) ;
	m_ui->cbVolumeType->setEnabled( enable ) ;
	m_ui->labelKeySource->setEnabled( enable ) ;
	m_ui->cbKeySource->setEnabled( enable ) ;

	auto index = m_ui->cbVolumeHeader->currentIndex() ;

	m_ui->labelPIM->setEnabled( index == 2 ) ;
	m_ui->lineEditPIM->setEnabled( index == 2 ) ;

	m_ui->cbKeySource->setEnabled( e != 0 ) ;
	m_ui->labelKeySource->setEnabled( e != 0 ) ;

	this->backUpHeaderNameChange( m_ui->lineEditDevicePath->text() ) ;
}

void managevolumeheader::cbVolumeType( int e )
{
	Q_UNUSED( e ) ;
}

void managevolumeheader::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void managevolumeheader::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}

void managevolumeheader::ShowUI()
{
	if( m_ui->lineEditDevicePath->text().isEmpty() ){

		m_ui->lineEditDevicePath->setFocus() ;

	}else if( m_ui->lineEditBackUpName->text().isEmpty() ){

		m_ui->lineEditBackUpName->setFocus() ;
	}else{
		m_ui->pbCreate->setFocus() ;
	}

	this->show() ;
}

void managevolumeheader::restoreHeader()
{
	m_operation = "restore" ;
//	m_ui->label->setText( tr( "Enter an existing key in the back up header file" ) ) ;

	this->setWindowTitle( tr( "Restore volume header" ) ) ;

	m_ui->labelBackUpHeader->setText( "Backup path" ) ;
	m_ui->pbCreate->setText( tr( "&Restore" ) ) ;

	this->ShowUI() ;
}

void managevolumeheader::headerBackUp()
{
	m_operation = "backup" ;
//	m_ui->label->setText( tr( "Enter an existing key in the volume" ) ) ;

	this->setWindowTitle( tr( "Back up volume header" ) ) ;

	m_ui->labelBackUpHeader->setText( "Backup path" ) ;
	m_ui->pbCreate->setText( tr( "&Backup" ) ) ;

	this->ShowUI() ;
}

void managevolumeheader::backUpHeader()
{
	this->headerBackUp() ;
}

void managevolumeheader::backUpHeader( QString device )
{
	m_ui->lineEditDevicePath->setText( device ) ;
	this->headerBackUp() ;
}

void managevolumeheader::backUpHeaderNameChange( QString name )
{
	if( m_operation == "restore" ){
		return ;
	}
	if( !m_ui->lineEditDevicePath->text().isEmpty() ){

		auto p = name.split( "/" ).last() ;

		if( p.isEmpty() ){

			m_ui->lineEditBackUpName->clear() ;
		}else{
			auto path = m_ui->lineEditBackUpName->text() ;

			if( path.isEmpty() ){

				path = utility::homePath() + "/" ;
			}

			auto q = path.split( "/" ) ;

			path.clear() ;

			int j = q.size() - 1 ;

			for(  int i = 0 ; i < j ; i++ ){

				path += q.at( i ) +  "/" ;
			}

			auto index = m_ui->cbVolumeHeader->currentIndex() ;

			if( index == 0 ){

				path += p + ".luksVolumeHeaderBackUp" ;

			}else if( index == 1 ){

				path += p + ".tcryptVolumeHeaderBackUp" ;
			}else{
				path += p + ".vcryptVolumeHeaderBackUp" ;
			}

			m_ui->lineEditBackUpName->setText( path ) ;
		}
	}
}

void managevolumeheader::pbOpenLuksHeaderBackUp()
{
	QString Z ;

	if( m_operation == "restore" ){

		Z = QFileDialog::getOpenFileName( this,tr( "Select A File With A LUKS Backup Header" ),utility::homePath() ) ;

		if( Z.isEmpty() ){

			return ;
		}
	}else{
		Z = QFileDialog::getExistingDirectory( this,tr( "Select A Folder To Store The Header" ),utility::homePath(),QFileDialog::ShowDirsOnly ) ;

		if( Z.isEmpty() ){

			return ;
		}

		while( true ){

			if( Z.endsWith( '/' ) ){

				Z.truncate( Z.length() - 1 ) ;
			}else{
				break ;
			}
		}

		QString p = m_ui->lineEditDevicePath->text().split( "/" ).last() ;

		if( !p.isEmpty() ){

			QString q = m_ui->lineEditBackUpName->text() ;

			if( q.isEmpty() ){

				Z += "/" + p + ".volumeHeaderBackUp" ;
			}else{
				q = q.split( "/" ).last() ;

				if( q.isEmpty() ){

					Z += "/" + p + ".volumeHeaderBackUp" ;
				}else{
					Z += "/" + q ;
				}
			}
		}
	}

	m_ui->lineEditBackUpName->setText( Z ) ;

	if( m_ui->lineEditDevicePath->text().isEmpty() ){

		m_ui->lineEditDevicePath->setFocus() ;
	}else{
		m_ui->pbCreate->setFocus() ;
	}
}

void managevolumeheader::pbCancel()
{
	if(  !m_OperationInProgress ){

		this->HideUI() ;
	}
}

void managevolumeheader::enableAll()
{
	m_ui->labelBackUpHeader->setEnabled( true ) ;
	m_ui->labelDevicePath->setEnabled( true ) ;
	m_ui->lineEditBackUpName->setEnabled( true ) ;
	m_ui->lineEditDevicePath->setEnabled( true ) ;
	m_ui->pbCancel->setEnabled( true ) ;
	m_ui->pbCreate->setEnabled( true ) ;
	m_ui->pbOpenFolder->setEnabled( true ) ;
	m_ui->pushButtonFile->setEnabled( true ) ;
	m_ui->pushButtonPartition->setEnabled( true ) ;
	m_ui->cbVolumeHeader->setEnabled( true ) ;

	auto index = m_ui->cbVolumeHeader->currentIndex() ;

	m_ui->labelPIM->setEnabled( index == 2 ) ;
	m_ui->lineEditPIM->setEnabled( index == 2 ) ;
	m_ui->labelKeyType->setEnabled( index != 0 ) ;
	m_ui->labelKeySource->setEnabled( index != 0 ) ;
	m_ui->cbKeySource->setEnabled( index != 0 ) ;
	m_ui->lineEditPassWord->setEnabled( index != 0 ) ;

	m_ui->labelVolumeType->setEnabled( index != 0 ) ;
	m_ui->cbVolumeType->setEnabled( index != 0 ) ;

	if( index != 0 ){

		//m_ui->label->setEnabled( true ) ;

		if( m_ui->cbKeySource->currentIndex() == 1 ){

			m_ui->pBKeyFile->setEnabled( true ) ;
		}
	}
}

void managevolumeheader::disableAll()
{
	m_ui->labelKeyType->setEnabled( false ) ;
	m_ui->labelKeySource->setEnabled( false ) ;
	m_ui->cbKeySource->setEnabled( false ) ;
	m_ui->lineEditPassWord->setEnabled( false ) ;
	m_ui->labelVolumeType->setEnabled( false ) ;
	m_ui->cbVolumeType->setEnabled( false ) ;
	m_ui->labelBackUpHeader->setEnabled( false ) ;
	m_ui->labelDevicePath->setEnabled( false ) ;
	m_ui->lineEditBackUpName->setEnabled( false ) ;
	m_ui->lineEditDevicePath->setEnabled( false ) ;
	m_ui->pbCancel->setEnabled( false ) ;
	m_ui->pbCreate->setEnabled( false ) ;
	m_ui->pbOpenFolder->setEnabled( false ) ;
	m_ui->pushButtonFile->setEnabled( false ) ;
	m_ui->pushButtonPartition->setEnabled( false ) ;
	//m_ui->label->setEnabled( false ) ;
	m_ui->pBKeyFile->setEnabled( false ) ;
	m_ui->cbVolumeHeader->setEnabled( false ) ;
	m_ui->labelPIM->setEnabled( false ) ;
	m_ui->lineEditPIM->setEnabled( false ) ;
}

void managevolumeheader::pbCreate()
{
	DialogMsg msg( this ) ;

	this->disableAll() ;

	utility::raii raii( [ this ](){ this->enableAll() ; } ) ;

	if( m_ui->lineEditBackUpName->text().isEmpty() || m_ui->lineEditDevicePath->text().isEmpty() ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
	}
	if( m_ui->lineEditPassWord->text().isEmpty() && m_ui->cbVolumeHeader->currentIndex() != 0 ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
	}

	auto device = utility::resolvePath( m_ui->lineEditDevicePath->text() ) ;

	auto backUp = m_ui->lineEditBackUpName->text().replace( "\"","\"\"\"" ) ;

	QString exe ;

	if( utility::requireSystemPermissions( device ) ){

		if( !utility::enablePolkit( utility::background_thread::False ) ){

			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to enable polkit support" ) ) ;
		}
	}

	device.replace( "\"","\"\"\"" ) ;

	if( m_operation == "backup" ){


		m_saveHeader = 1 ;

		if( m_ui->cbVolumeHeader->currentIndex() != 0 ){

			if( m_ui->cbKeySource->currentIndex() == 0 ){

				auto path = utility::keyPath() ;
				auto key  = m_ui->lineEditPassWord->text() ;

				utility::keySend( path,key ) ;

				exe = QString( "%1 -B -d \"%2\" -z \"%3\" -f \"%4\"" ).arg( ZULUCRYPTzuluCrypt,device,backUp,path ) ;
			}else{
				auto path = m_ui->lineEditPassWord->text() ;

				exe = QString( "%1 -B -d \"%2\" -z \"%3\" -f \"%4\"" ).arg( ZULUCRYPTzuluCrypt,device,backUp,path ) ;
			}

			if( m_ui->cbVolumeHeader->currentIndex() == 2 ){

				auto e = m_ui->lineEditPIM->text() ;

				if( e.isEmpty() ){

					exe += " -t vcrypt" ;
				}else{
					exe += " -t vcrypt." + e ;
				}
			}
		}else{
			exe = QString( "%1 -B -d \"%2\" -z \"%3\"" ).arg( ZULUCRYPTzuluCrypt,device,backUp ) ;
		}
	}else{
		m_saveHeader = 0 ;
		auto x = m_ui->lineEditDevicePath->text() ;
		auto y = m_ui->lineEditBackUpName->text() ;

		auto warn = tr( "Are you sure you want to replace a header on device \"%1\" with a backup copy at \"%2\"?" ).arg( x ).arg( y ) ;

		if( msg.ShowUIYesNoDefaultNo( tr( "WARNING!" ),warn ) == QMessageBox::No ){

			return ;
		}
		if( m_ui->cbVolumeHeader->currentIndex() != 0 ){

			if( m_ui->cbKeySource->currentIndex() == 0 ){

				auto path = utility::keyPath() ;
				auto key  = m_ui->lineEditPassWord->text() ;

				utility::keySend( path,key ) ;

				exe = QString( "%1 -kR -d \"%2\" -z \"%3\" -f \"%4\"" ).arg( ZULUCRYPTzuluCrypt,device,backUp,path ) ;
			}else{
				QString path = m_ui->lineEditPassWord->text() ;
				exe = QString( "%1 -kR -d \"%2\" -z \"%3\" -f \"%4\"" ).arg( ZULUCRYPTzuluCrypt,device,backUp,path ) ;
			}

			if( m_ui->cbVolumeHeader->currentIndex() == 2 ){

				auto e = m_ui->lineEditPIM->text() ;

				if( e.isEmpty() ){

					exe += " -t vcrypt" ;
				}else{
					exe += " -t vcrypt." + e ;
				}
			}
		}else{
			exe = QString( "%1 -kR -d \"%2\" -z \"%3\"" ).arg( ZULUCRYPTzuluCrypt,device,backUp ) ;
		}
	}

	/*
	 * default to /dev/random as source of random data when managing a truecrypt header
	 */
	auto index = m_ui->cbVolumeType->currentIndex() ;

	if( index == 2 ){

		exe += " -g /dev/urandom -e fde" ;

	}else if( index == 1 ){

		exe += " -g /dev/urandom -e sys" ;
	}

	m_OperationInProgress = true ;

	m_veraCryptWarning.show( m_ui->cbVolumeHeader->currentIndex() == 2 ) ;

	raii.cancel() ;

	this->taskFinished( utility::exec( utility::appendUserUID( exe ) ).await() ) ;
}

void managevolumeheader::pbOpenPartition()
{
	openvolume::instance( this ).ShowNonSystemPartitions( [ this ]( const QString& e ){

		m_ui->lineEditDevicePath->setText( e ) ;

		if( m_ui->lineEditBackUpName->text().isEmpty() ){

			m_ui->lineEditBackUpName->setFocus() ;
		}else{
			m_ui->pbCreate->setFocus() ;
		}
	} ) ;
}

void managevolumeheader::pbOpenFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "Select luks container you want to backup its header" ),utility::homePath() ) ;

	m_ui->lineEditDevicePath->setText( Z ) ;

	if( m_ui->lineEditBackUpName->text().isEmpty() ){

		m_ui->lineEditBackUpName->setFocus() ;
	}else{
		m_ui->pbCreate->setFocus() ;
	}
}

void managevolumeheader::pbKeyFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "" ),utility::homePath() ) ;

	if( !Z.isEmpty() ){

		m_ui->lineEditPassWord->setText( Z ) ;
	}
}

void managevolumeheader::success()
{
	DialogMsg msg( this ) ;

	if( m_saveHeader ){

		msg.ShowUIOK( tr( "SUCCESS" ),tr( "Header saved successfully.\nIf possible,store it securely." ) ) ;
	}else{
		msg.ShowUIOK( tr( "SUCCESS" ),tr( "Header restored successfully" ) )	;
	}

	return this->HideUI() ;
}

void managevolumeheader::taskFinished( int st )
{
	m_veraCryptWarning.stopTimer() ;

	m_OperationInProgress = false ;

	DialogMsg msg( this ) ;

	switch( st ){

		case 0 : return this->success() ;
		case 1 : return this->success() ;
		case 2 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Presented device is not a LUKS device" ) )					; break ;
		case 3 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to perform requested operation" ) )					; break ;
		case 4 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to perform requested operation" ) )					; break ;
		case 5 : msg.ShowUIOK( tr( "INFO!"  ),tr( "Operation terminater per user request" ) )					; break ;
		case 6 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Path to be used to create a back up file is occupied" ) ) 			; break ;
		case 7 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to perform requested operation" ) )					; break ;
		case 8 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to open backup header file for reading" ) ) 		; break ;
		case 9 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Invalid path to back up header file" ) )					; break ;
		case 10: msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to create a backup header in a destination folder" )) ; break ;
		case 11: msg.ShowUIOK( tr( "ERROR!" ),tr( "Invalid path to device" ) )							; break ;
		case 12: msg.ShowUIOK( tr( "ERROR!" ),tr( "Argument for path to a backup  header file is missing" ) ) 			; break ;
		case 13: msg.ShowUIOK( tr( "ERROR!" ),tr( "Argument for path to a backup  header file is missing" ) ) 			; break ;
		case 14: msg.ShowUIOK( tr( "ERROR!" ),tr( "Only root user and \"zulucrypt\" members can restore and back up luks headers on system devices" ) )	; break ;
		case 15: msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to open device for writing" ) ) 			; break ;
		case 16: msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not resolve path to device" ) )					; break ;
		case 17: msg.ShowUIOK( tr( "ERROR!" ),tr( "Backup file does not appear to contain luks header" ) )			; break ;
		case 18: msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to open device for reading" ) )			; break ;
		case 20:{
				if( m_ui->cbVolumeHeader->currentIndex() != 0 ){

					msg.ShowUIOK( tr( "ERROR!" ),tr( "Wrong password entered or volume is not a truecrypt volume" ) ) ;
				}else{
					msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to perform requested operation on the LUKS volume" ) ) ;
				}
			}
			break ;
		case 21:{
			if( m_ui->cbVolumeHeader->currentIndex() != 0 ){

				msg.ShowUIOK( tr( "ERROR!" ),tr( "Wrong password entered or volume is not a veracrypt volume" ) ) ;
			}else{
				msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to perform requested operation on the LUKS volume" ) )	;
			}
			break ;
		}
	default: msg.ShowUIOK( tr( "ERROR!" ),tr( "Unrecognized ERROR! with status number %1 encountered" ).arg( st ) ) ;
	}

	m_veraCryptWarning.hide() ;
	this->enableAll() ;
}

managevolumeheader::~managevolumeheader()
{
	delete m_ui ;
}

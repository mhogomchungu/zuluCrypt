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

#include "managevolumeheader.h"
#include "utility.h"
#include "../zuluCrypt-cli/constants.h"

#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QKeyEvent>

#include <QFileDialog>

#include "ui_managevolumeheader.h"
#include "task.h"
#include "utility.h"
#include "openvolume.h"
#include "dialogmsg.h"

managevolumeheader::managevolumeheader( QWidget * parent ) :
    QDialog( parent ),
    m_ui( new Ui::managevolumeheader )
{
	m_ui->setupUi( this ) ;
	this->setFont( parent->font() ) ;
	this->setFixedSize( this->size() ) ;

	m_ui->pbOpenFolder->setIcon( QIcon( QString( ":/folder.png" ) ) ) ;
	m_ui->pushButtonPartition->setIcon( QIcon( QString( ":/partition.png" ) ) ) ;
	m_ui->pushButtonFile->setIcon( QIcon( QString( ":/file.png" ) ) ) ;

	m_ui->lineEditDevicePath->setFocus() ;

	connect( m_ui->pbCreate,SIGNAL( clicked() ),this,SLOT( pbCreate() ) ) ;
	connect( m_ui->pbOpenFolder,SIGNAL( clicked() ),this,SLOT( pbOpenLuksHeaderBackUp() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pushButtonFile,SIGNAL( clicked() ),this,SLOT( pbOpenFile() ) ) ;
	connect( m_ui->pushButtonPartition,SIGNAL( clicked() ),this,SLOT( pbOpenPartition() ) ) ;
	connect( m_ui->lineEditDevicePath,SIGNAL( textChanged( QString ) ),this,SLOT( backUpHeaderNameChange( QString ) ) ) ;

	m_OperationInProgress = false ;
}

void managevolumeheader::HideUI()
{
	this->hide() ;
	emit HideUISignal() ;
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
	m_operation = QString( "restore" ) ;
	this->setWindowTitle( tr( "restore volume header" ) ) ;
	m_ui->labelBackUpHeader->setText( QString( "backup path" ) ) ;
	m_ui->pbCreate->setText( tr( "&restore" ) ) ;
	this->ShowUI() ;
}

void managevolumeheader::headerBackUp()
{
	m_operation = QString( "backup" ) ;
	this->setWindowTitle( tr( "back up volume header" ) ) ;
	m_ui->labelBackUpHeader->setText( QString( "backup path" ) ) ;
	m_ui->pbCreate->setText( tr( "&backup" ) ) ;
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
	if( m_operation == QString( "restore" ) ){
		return ;
	}
	if( m_ui->lineEditDevicePath->text().isEmpty() ){
		;
	}else{
		QString p = name.split( "/" ).last() ;

		if( p.isEmpty() ){
			m_ui->lineEditBackUpName->clear() ;
		}else{
			QString path = m_ui->lineEditBackUpName->text() ;

			if( path.isEmpty() ){
				path = QDir::homePath() + QString( "/" ) ;
			}
			QStringList q = path.split( "/" ) ;
			path.clear() ;
			int j = q.size() - 1 ;
			for(  int i = 0 ; i < j ; i++ ){
				path += q.at( i ) +  QString( "/" ) ;
			}
			path += p + QString( ".volumeHeaderBackUp" ) ;
			m_ui->lineEditBackUpName->setText( path ) ;
		}
	}
}

void managevolumeheader::pbOpenLuksHeaderBackUp()
{
	QString Z ;
	QString Y ;
	if( m_operation == QString( "restore" ) ){
		Z = QFileDialog::getOpenFileName( this,tr( "select a file with a luks backup header" ),QDir::homePath(),0 ) ;
		if( Z.isEmpty() ){
			return ;
		}
	}else{
		Z = QFileDialog::getExistingDirectory( this,tr( "select a folder to store the header" ),QDir::homePath(),0 ) ;

		if( Z.isEmpty() ){
			return ;
		}
		QString p = m_ui->lineEditDevicePath->text().split( "/" ).last() ;

		if( !p.isEmpty() ){
			QString q = m_ui->lineEditBackUpName->text() ;
			if( q.isEmpty() ){
				Z += QString( "/" ) + p + QString( ".volumeHeaderBackUp" ) ;
			}else{
				q = q.split( "/" ).last() ;
				if( q.isEmpty() ){
					Z += QString( "/" ) + p + QString( ".volumeHeaderBackUp" ) ;
				}else{
					Z += QString( "/" ) + q ;
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
}

void managevolumeheader::disableAll()
{
	m_ui->labelBackUpHeader->setEnabled( false ) ;
	m_ui->labelDevicePath->setEnabled( false ) ;
	m_ui->lineEditBackUpName->setEnabled( false ) ;
	m_ui->lineEditDevicePath->setEnabled( false ) ;
	m_ui->pbCancel->setEnabled( false ) ;
	m_ui->pbCreate->setEnabled( false ) ;
	m_ui->pbOpenFolder->setEnabled( false ) ;
	m_ui->pushButtonFile->setEnabled( false ) ;
	m_ui->pushButtonPartition->setEnabled( false ) ;
}

void managevolumeheader::pbCreate()
{
	DialogMsg msg( this ) ;

	if( m_ui->lineEditBackUpName->text().isEmpty() || m_ui->lineEditDevicePath->text().isEmpty() ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "atleast one required field is empty" ) ) ;
	}
	QString device = utility::resolvePath( m_ui->lineEditDevicePath->text() ) ;

	device.replace( "\"","\"\"\"" ) ;

	QString backUp = m_ui->lineEditBackUpName->text().replace( "\"","\"\"\"" ) ;

	QString exe ;
	if(  m_operation == QString( "backup" ) ){
		m_saveHeader = 1 ;
		exe = QString( "%1 -B -d \"%2\" -f \"%3\"" ).arg( ZULUCRYPTzuluCrypt ).arg( device ).arg( backUp ) ;
	}else{
		m_saveHeader = 0 ;
		QString x = m_ui->lineEditDevicePath->text() ;
		QString y = m_ui->lineEditBackUpName->text() ;

		QString warn = tr( "Are you sure you want to replace a header on device \"%1\" with a backup copy at \"%2\"?" ).arg( x ).arg( y ) ;

		if( msg.ShowUIYesNoDefaultNo( tr( "WARNING!" ),warn ) == QMessageBox::No ){
			return ;
		}
		exe = QString( "%1 -kR -d \"%2\" -f \"%3\"" ).arg( ZULUCRYPTzuluCrypt ).arg( device ).arg( backUp ) ;
	}

	this->disableAll() ;

	m_OperationInProgress = true ;

	Task * t = new Task( exe ) ;
	connect( t,SIGNAL( finished( int ) ),this,SLOT( taskFinished( int ) ) ) ;
	t->start() ;
	disableAll() ;
}

void managevolumeheader::pbOpenPartition()
{
	openvolume * op = new openvolume( this ) ;
	connect( op,SIGNAL( clickedPartition( QString ) ),this,SLOT( selectedPartition( QString ) ) ) ;
	connect( op,SIGNAL( HideUISignal() ),op,SLOT( deleteLater() ) ) ;
	op->ShowNonSystemPartitions() ;
}

void managevolumeheader::selectedPartition( QString p )
{
	m_ui->lineEditDevicePath->setText( p ) ;
	if( m_ui->lineEditBackUpName->text().isEmpty() ){
		m_ui->lineEditBackUpName->setFocus() ;
	}else{
		m_ui->pbCreate->setFocus() ;
	}
}

void managevolumeheader::pbOpenFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "select luks container you want to backup its header" ),QDir::homePath(),0 ) ;
	m_ui->lineEditDevicePath->setText( Z ) ;
	if( m_ui->lineEditBackUpName->text().isEmpty() ){
		m_ui->lineEditBackUpName->setFocus() ;
	}else{
		m_ui->pbCreate->setFocus() ;
	}
}

void managevolumeheader::success()
{
	DialogMsg msg( this ) ;
	if( m_saveHeader ){
		msg.ShowUIOK( tr( "SUCCESS" ),tr( "header saved successfully.\nif possible,store it securely." ) ) ;
	}else{
		msg.ShowUIOK(  tr( "SUCCESS" ),tr( "header restored successfully" ) )	;
	}
	return this->HideUI() ;
}

void managevolumeheader::taskFinished( int st )
{
	m_OperationInProgress = false ;
	DialogMsg msg( this ) ;
	switch( st ){
		case 0 : return this->success() ;
		case 1 : return this->success() ;
		case 2 : msg.ShowUIOK( tr( "ERROR!" ),tr( "presented device is not a LUKS device" ) )					; break ;
		case 3 : msg.ShowUIOK( tr( "ERROR!" ),tr( "failed to perform requested operation" ) )					; break ;
		case 4 : msg.ShowUIOK( tr( "ERROR!" ),tr( "failed to perform requested operation" ) )					; break ;
		case 5 : msg.ShowUIOK( tr( "INFO!"  ),tr( "operation terminater per user request" ) )					; break ;
		case 6 : msg.ShowUIOK( tr( "ERROR!" ),tr( "path to be used to create a back up file is occupied" ) ) 			; break ;
		case 7 : msg.ShowUIOK( tr( "ERROR!" ),tr( "failed to perform requested operation" ) )					; break ;
		case 8 : msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open backup header file for reading" ) ) 		; break ;
		case 9 : msg.ShowUIOK( tr( "ERROR!" ),tr( "invalid path to back up header file" ) )					; break ;
		case 10: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to create a backup header in a destination folder" )) ; break ;
		case 11: msg.ShowUIOK( tr( "ERROR!" ),tr( "invalid path to device" ) )							; break ;
		case 12: msg.ShowUIOK( tr( "ERROR!" ),tr( "argument for path to a backup  header file is missing" ) ) 			; break ;
		case 13: msg.ShowUIOK( tr( "ERROR!" ),tr( "argument for path to a backup  header file is missing" ) ) 			; break ;
		case 14: msg.ShowUIOK( tr( "ERROR!" ),tr( "only root user and \"zulucrypt\" members can restore and back up luks headers on system devices" ) )	; break ;
		case 15: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open device for writing" ) ) 			; break ;
		case 16: msg.ShowUIOK( tr( "ERROR!" ),tr( "could not resolve path to device" ) )					; break ;
		case 17: msg.ShowUIOK( tr( "ERROR!" ),tr( "backup file does not appear to contain luks header" ) )			; break ;
		case 18: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open device for reading" ) )			; break ;
		case 113: msg.ShowUIOK( tr( "ERROR!" ),tr( "device is not a luks device" ) ) 						; break ;
		default : msg.ShowUIOK( tr( "ERROR!" ),tr( "unrecognized ERROR! with status number %1 encountered" ).arg( st ) ) ;
	}
	this->enableAll() ;
}

managevolumeheader::~managevolumeheader()
{
	delete m_ui ;
}

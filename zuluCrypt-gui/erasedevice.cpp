/*
 *  Copyright ( c ) 2012
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

#include "erasedevice.h"
#include "ui_erasedevice.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>

#include "utility.h"
#include "openvolume.h"
#include "dialogmsg.h"
#include "../zuluCrypt-cli/constants.h"

erasedevice::erasedevice( QWidget * parent ) :
	QDialog( parent ),
	m_ui( new Ui::erasedevice )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_ui->progressBar->setValue( 0 ) ;
	m_ui->progressBar->setMaximum( 100 ) ;
	m_ui->progressBar->setMinimum( 0 ) ;

	this->setWindowTitle( tr( "write random data over existing data" ) ) ;

	connect( m_ui->pushButtonFile,SIGNAL( clicked() ),this,SLOT( pbFile() ) ) ;
	connect( m_ui->pushButtonPartition,SIGNAL( clicked() ),this,SLOT( pbPartition() ) ) ;
	connect( m_ui->pushButtonStart,SIGNAL( clicked() ),this,SLOT( pbStart() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	connect( this,SIGNAL( sendProgress( int ) ),this,SLOT( setProgress( int ) ) ) ;

	m_ui->pushButtonFile->setIcon( QIcon( QString( ":/file.png" ) ) ) ;
	m_ui->pushButtonPartition->setIcon( QIcon( QString( ":/partition.png" ) ) ) ;

	m_ui->lineEdit->setFocus() ;

	this->installEventFilter( this ) ;
}

bool erasedevice::eventFilter( QObject * watched,QEvent * event )
{
	if( utility::eventFilter( this,watched,event ) ){
		this->HideUI() ;
		return true ;
	}else{
		return false ;
	}
}

void erasedevice::ShowUI()
{
	m_option = 0 ;

	DialogMsg msg( this ) ;
	QString x = tr ( "\
The next dialog will write random data to a device leading to permanent loss of all contents on the device.\n\n\
Are you sure you want to continue? " ) ;

	if( msg.ShowUIYesNoDefaultNo( tr( "WARNING" ),x ) == QMessageBox::Yes ){
		this->show() ;
	}else{
		this->HideUI() ;
	}
}

void erasedevice::ShowUI( const QString& path )
{
	m_option = 1 ;
	m_ui->lineEdit->setText( path ) ;
	this->show() ;
	this->pbStart() ;
}

void erasedevice::taskResult( int st )
{
	this->setWindowTitle( tr( "write random data over existing data" ) ) ;

	DialogMsg msg( this ) ;

	switch( st ){
		case 0 : m_ui->progressBar->setValue( 100 ) ;
			 msg.ShowUIOK( tr( "SUCCESS!" ),tr( "data on the device successfully erased" ) )			;break ;
		case 1: msg.ShowUIOK( tr( "ERROR!" ),tr( "could not create mapper" ) )						;break ;
		case 2: msg.ShowUIOK( tr( "ERROR!" ),tr( "could not resolve device path" ) )					;break ;
		case 3: msg.ShowUIOK( tr( "ERROR!" ),tr( "random data successfully written" ) )					;break ;
		//case 4: msg.ShowUIOK( tr( "ERROR!" ),tr( "user chose not to proceed" ) )					;break ;
		case 5: msg.ShowUIOK( tr( "ERROR!" ),tr( "operation terminated per user choice" ) )				;break ;
		case 6: msg.ShowUIOK( tr( "ERROR!" ),tr( "can not write on a device with opened mapper" ) )			;break ;
		case 7: msg.ShowUIOK( tr( "ERROR!" ),tr( "policy prevents non root user opening mapper on system partition" ) ) ;break;
		case 8: msg.ShowPermissionProblem( QString( "writing" ),m_ui->lineEdit->text() )				;break ;
		case 9: msg.ShowUIOK( tr( "ERROR!" ),tr( "device path is invalid" ) )						;break ;
		case 10:msg.ShowUIOK( tr( "ERROR!" ),tr( "passphrase file does not exist" ) )					;break ;
		case 11:msg.ShowUIOK( tr( "ERROR!" ),tr( "could not get enought memory to hold the key file" ) )		;break ;
		case 12:msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open key file for reading" ) )		;break ;
		case 13:msg.ShowUIOK( tr( "ERROR!" ),tr( "this device appear to already be in use" ) )				;break ;
		case 14:msg.ShowUIOK( tr( "ERROR!" ),tr( "can not open a mapper on a mounted device" ) )			;break ;
		default:msg.ShowUIOK( tr( "ERROR!" ),tr( "could not write to the device" ) ) ;
	}

	this->HideUI() ;
}

void erasedevice::HideUI()
{
	emit HideUISignal() ;
	this->hide() ;
}

void erasedevice::pbStart()
{
	QString path = m_ui->lineEdit->text() ;

	DialogMsg msg( this ) ;

	if( path.isEmpty() ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "device path field is empty" ) ) ;
	}

	path = utility::resolvePath( path ) ;

	if( !utility::pathExists( path ) ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "invalid path to device" ) ) ;
	}
	if( m_option == 0 ){
		QString x = tr( "\
Are you really sure you want to write random data to \"%1\" effectively destroying all contents in it?" ).arg( path ) ;

		if( msg.ShowUIYesNoDefaultNo( tr( "WARNING!" ),x ) == QMessageBox::No ){
			return ;
		}
	}

	this->setWindowTitle( tr( "writing random data over existing data" ) ) ;

	this->disableAll() ;

	m_exit = false ;
	m_running = true ;

	int r = utility::clearVolume( path,&m_exit,[ this ]( int i ){ emit sendProgress( i ) ; } ).await() ;

	m_running = false ;

	this->taskResult( r ) ;
}

void erasedevice::enableAll()
{
	m_ui->label->setEnabled( true ) ;
	m_ui->labelPath->setEnabled( true ) ;
	m_ui->lineEdit->setEnabled( true ) ;
	m_ui->pushButtonCancel->setEnabled( true ) ;
	m_ui->pushButtonFile->setEnabled( true ) ;
	m_ui->pushButtonPartition->setEnabled( true ) ;
	m_ui->pushButtonStart->setEnabled( true ) ;
}

void erasedevice::disableAll()
{
	m_ui->label->setEnabled( false ) ;
	m_ui->labelPath->setEnabled( false ) ;
	m_ui->lineEdit->setEnabled( false ) ;
	//m_ui->pushButtonCancel->setEnabled( false ) ; //need this one enabled
	m_ui->pushButtonFile->setEnabled( false ) ;
	m_ui->pushButtonPartition->setEnabled( false ) ;
	m_ui->pushButtonStart->setEnabled( false ) ;
}

void erasedevice::setProgress( int st )
{
	m_ui->progressBar->setValue( st ) ;
}

void erasedevice::pbCancel()
{
	if( m_running ){
		m_exit = true ;
	}else{
		this->HideUI() ;
	}
}

void erasedevice::pbFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "enter path to volume to be erased" ),QDir::homePath(),0 ) ;
	m_ui->lineEdit->setText( Z ) ;
}

void erasedevice::pbPartition()
{
	openvolume * op = new openvolume( this ) ;
	connect( op,SIGNAL( clickedPartition( QString ) ),this,SLOT( setPath( QString ) ) ) ;
	connect( op,SIGNAL( HideUISignal() ),op,SLOT( deleteLater() ) ) ;
	op->partitionList( tr( "select a non system partition to erase its contents" ),QString( " -N" ) ) ;
}

void erasedevice::setPath( QString p )
{
	m_ui->lineEdit->setText( p ) ;
}

void erasedevice::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}

erasedevice::~erasedevice()
{
	delete m_ui ;
}

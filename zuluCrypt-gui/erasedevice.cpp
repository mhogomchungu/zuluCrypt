/*
 *  Copyright ( c ) 2012-2015
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
#include <QDateTime>

#include "utility.h"
#include "openvolume.h"
#include "dialogmsg.h"
#include "../zuluCrypt-cli/constants.h"

erasedevice::erasedevice( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::erasedevice )
{
	m_ui->setupUi( this ) ;

	m_label.setOptions( m_ui->label_2,m_ui->pushButton ) ;

	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_ui->progressBar->setValue( 0 ) ;
	m_ui->progressBar->setMaximum( 100 ) ;
	m_ui->progressBar->setMinimum( 0 ) ;

	this->setWindowTitle( tr( "Write Random Data Over Existing Data" ) ) ;

	connect( m_ui->pushButtonFile,SIGNAL( clicked() ),this,SLOT( pbFile() ) ) ;
	connect( m_ui->pushButtonPartition,SIGNAL( clicked() ),this,SLOT( pbPartition() ) ) ;
	connect( m_ui->pushButtonStart,SIGNAL( clicked() ),this,SLOT( pbStart() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	connect( this,SIGNAL( sendProgress( QString,QString,QString,QString,int ) ),
		 this,SLOT( setProgress( QString,QString,QString,QString,int ) ) ) ;

	m_ui->pushButtonFile->setIcon( QIcon( ":/file.png" ) ) ;
	m_ui->pushButtonPartition->setIcon( QIcon( ":/partition.png" ) ) ;

	m_ui->lineEdit->setFocus() ;

	this->installEventFilter( this ) ;
}

bool erasedevice::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void erasedevice::ShowUI()
{
	m_option = 0 ;

	DialogMsg msg( this ) ;

	auto x = tr ( "\
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
	this->setWindowTitle( tr( "Write Random Data Over Existing Data" ) ) ;

	DialogMsg msg( this ) ;

	switch( st ){

		case 0: m_ui->progressBar->setValue( 100 ) ;
		{
			QString a = tr( "Average Speed:" ) + " " + m_average_speed + "\n" ;
			QString b = tr( "Total Time:" ) + " " + m_total_time + "\n\n" ;
			m_label.show( a + b + tr( "Data on the device successfully erased" ) ) ;
		}
		break ;
		case 1: m_label.show( tr( "Could not create mapper" ) )						;break ;
		case 2: m_label.show( tr( "Could not resolve device path" ) )					;break ;
		case 3: m_label.show( tr( "Random data successfully written" ) )				;break ;
		//case 4: msg.ShowUIOK( tr( "ERROR!" ),tr( "User chose not to proceed" ) )			;break ;
		case 5: m_label.show( tr( "Operation terminated per user choice" ) )				;break ;
		case 6: m_label.show( tr( "Can not write on a device with opened mapper" ) )			;break ;
		case 7: m_label.show( tr( "Policy prevents non root user opening mapper on system partition" ) );break;
		case 8: msg.ShowPermissionProblem( "Writing",m_ui->lineEdit->text() )				;break ;
		case 9: m_label.show( tr( "Device path is invalid" ) )						;break ;
		case 10:m_label.show( tr( "Passphrase file does not exist" ) )					;break ;
		case 11:m_label.show( tr( "Could not get enought memory to hold the key file" ) )		;break ;
		case 12:m_label.show( tr( "Insufficient privilege to open key file for reading" ) )		;break ;
		case 13:m_label.show( tr( "This device appear to already be in use" ) )				;break ;
		case 14:m_label.show( tr( "Can not open a mapper on a mounted device" ) )			;break ;
		default:m_label.show( tr( "Could not write to the device" ) ) ;
	}

	this->HideUI() ;
}

void erasedevice::HideUI()
{	
	this->hide() ;
	this->deleteLater() ;
}

void erasedevice::pbStart()
{
	auto path = m_ui->lineEdit->text() ;

	DialogMsg msg( this ) ;

	if( path.isEmpty() ){

		return m_label.show( tr( "Device path field is empty" ) ) ;
	}

	path = utility::resolvePath( path ) ;

	if( !utility::pathExists( path ) ){

		return m_label.show( tr( "Invalid path to device" ) ) ;
	}

	if( m_option == 0 ){

		QString x = tr( "\
Are you really sure you want to write random data to \"%1\" effectively destroying all contents in it?" ).arg( path ) ;

		if( msg.ShowUIYesNoDefaultNo( tr( "WARNING!" ),x ) == QMessageBox::No ){

			return ;
		}
	}

	if( utility::requireSystemPermissions( path ) ){

		if( !utility::enablePolkit( utility::background_thread::False ) ){

			return m_label.show( tr( "Failed to enable polkit support" ) ) ;
		}
	}

	this->setWindowTitle( tr( "Writing Random Data Over Existing Data" ) ) ;

	this->disableAll() ;

	m_exit = false ;
	m_running = true ;

	utility::progress update( 1500,[ this ]( const utility::progress::result& m ){

		emit sendProgress( m.current_speed,
				   m.average_speed,
				   m.eta,
				   m.total_time,
				   m.percentage_done ) ;
	} ) ;

	auto r = utility::clearVolume( path,&m_exit,0,update.updater_quint() ).await() ;

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
	m_ui->label_3->setEnabled( true ) ;
	m_ui->label_4->setEnabled( true ) ;
	m_ui->labelAverageSpeed->setEnabled( true ) ;
	m_ui->labelETA->setEnabled( true ) ;
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
	m_ui->label_3->setEnabled( false ) ;
	m_ui->label_4->setEnabled( false ) ;
	m_ui->labelAverageSpeed->setEnabled( false ) ;
	m_ui->labelETA->setEnabled( false ) ;
}

void erasedevice::setProgress( QString cs,QString av,QString eta,QString tt,int st )
{
	Q_UNUSED( cs )
	m_ui->labelAverageSpeed->setText( av ) ;
	m_ui->labelETA->setText( eta ) ;
	m_ui->progressBar->setValue( st ) ;

	m_average_speed = av ;
	m_total_time = tt ;
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
	auto Z = QFileDialog::getOpenFileName( this,tr( "Enter Path To Volume To Be Erased" ),utility::homePath() ) ;
	m_ui->lineEdit->setText( Z ) ;
}

void erasedevice::pbPartition()
{
	auto e = tr( "Select A Non System Partition To Erase Its Contents" ) ;

	openvolume::instance( this,false ).partitionList( e," -N",[ this ]( const QString& e ){

		this->setPath( e ) ;
	} ) ;
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

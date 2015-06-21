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

#include "openvolume.h"

#include <QCloseEvent>
#include <QDialog>
#include "ui_openvolume.h"
#include "task.h"
#include <QProcess>
#include <iostream>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QObject>
#include <QHeaderView>
#include <QFile>
#include <QThreadPool>
#include <QKeySequence>
#include "../zuluCrypt-cli/constants.h"
#include "dialogmsg.h"
#include "tablewidget.h"

#include <unistd.h>
#include <QDebug>

#define ALL_PARTITIONS 1
#define NON_SYSTEM_PARTITIONS 3

openvolume::openvolume( QWidget * parent ) :
	QDialog( parent )
{
	m_ui = new Ui::openvolume() ;
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	connect( m_ui->tableWidget,SIGNAL( itemDoubleClicked( QTableWidgetItem * ) ),this,SLOT( tableEntryDoubleClicked( QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),this,
		 SLOT( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->pbHelp,SIGNAL( clicked() ),this,SLOT( pbHelp() ) ) ;
	connect( m_ui->pbUUID,SIGNAL( clicked() ),this,SLOT( pbUUID() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	m_action = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter ) ;
	keys.append( Qt::Key_Return ) ;
	m_action->setShortcuts( keys ) ;

	connect( m_action,SIGNAL( triggered() ),this,SLOT( EnterKeyPressed() ) ) ;

	this->addAction( m_action ) ;

	QTableWidget * tw = m_ui->tableWidget ;
	tw->setColumnWidth( 0,183 ) ;
	tw->setColumnWidth( 1,90 ) ;
	//tw->setColumnWidth( 2,110 ) ;
	tw->hideColumn( 2 ) ;
	tw->setColumnWidth( 3,90 ) ;
	tw->setColumnWidth( 4,309 ) ;

	for( int i = 0 ; i < 5 ; i++ ){
		tw->horizontalHeaderItem( i )->setFont( this->font() ) ;
	}
	tw->horizontalHeader()->setVisible( true ) ;

	m_ui->checkBoxUUID->setVisible( false ) ;

	m_diableNonLUKS = false ;
	//m_ui->pbUUID->setVisible( false ) ;
	m_showEncryptedOnly = false ;
	m_showLuksOnly = false ;

	this->installEventFilter( this ) ;
}

bool openvolume::eventFilter( QObject * watched,QEvent * event )
{
	if( utility::eventFilter( this,watched,event ) ){
		this->HideUI() ;
		return true ;
	}else{
		return false ;
	}
}

void openvolume::showEncryptedOnly()
{
	m_showEncryptedOnly = true ;
}

void openvolume::showLuksOnly()
{
	m_showEncryptedOnly = true ;
}

void openvolume::pbHelp()
{
	DialogMsg msg( this ) ;

	QString m ;

	if( m_option == 2 ){
		m = tr( "A list of all partitions on this system are displayed here.\nDouble click an entry to use it" ) ;
	}else{
		if( getuid() != 0 ) {
			m = tr( "You are not root user and hence only non system partition are displayed on this list.\
\nPlease read documentation for more information on system/non system partitions.\nDouble click an entry to use it" ) ;
		}else{
			m = tr( "You are a root user and all partitions are displayed.\nDouble click an entry to use it" )	;
		}
	}
	msg.ShowUIOK( tr( "INFO" ),m ) ;
}

void openvolume::pbUUID()
{
	if( m_ui->pbUUID->isFlat() ){
		m_ui->pbUUID->setFlat( false ) ;
	}else{
		m_ui->pbUUID->setFlat( true ) ;
	}
}

void openvolume::pbCancel()
{
	this->HideUI() ;
}

void openvolume::EnterKeyPressed()
{
	auto tw = m_ui->tableWidget ;
	auto it = tw->currentItem() ;

	if( it != nullptr ){
		tableEntryDoubleClicked( tw->item( it->row(),0 ) ) ;
	}
}

void openvolume::currentItemChanged( QTableWidgetItem * current, QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;
}

void openvolume::ShowNonSystemPartitions()
{
	m_option = 1 ;
	this->partitionList( tr( "Select A Partition To Create An Encrypted Volume In" )," -N" ) ;
}

void openvolume::ShowAllPartitions()
{
	m_option = 2 ;
	this->partitionList( tr( "Select An Encrypted Partition To Open" )," -A" ) ;
}

void openvolume::ShowPartitionList( QString x,QString y )
{
	this->partitionList( x,y ) ;
}

void openvolume::allowLUKSOnly()
{
	m_diableNonLUKS = true ;
	this->ShowAllPartitions() ;
}

void openvolume::partitionList( QString title,QString volumeType )
{
	this->setWindowTitle( title ) ;

	while( m_ui->tableWidget->rowCount() > 0 ){
		m_ui->tableWidget->removeRow( 0 ) ;
	}

	m_ui->tableWidget->setEnabled( false ) ;
	this->show() ;

	QStringList l = Task::await<QStringList>( [ & ](){

		/*
		 * Root user can create encrypted volumes in all partitions including system partitions.
		 * Show all partitions, not only non system.
		 */
		if( volumeType == " -N" && utility::userIsRoot() ){
			volumeType = " -A" ;
		}

		QString exe = QString( "%1 %2 -Z" ).arg( ZULUCRYPTzuluCrypt,volumeType ) ;

		return utility::Task( exe ).splitOutput( '\n' ) ;
	} ) ;

	m_ui->tableWidget->setEnabled( true ) ;
	m_ui->tableWidget->setFocus() ;

	this->partitionProperties( l ) ;
}

void openvolume::partitionProperties( const QStringList& l )
{
	QStringList z ;

	for( const auto& it : l ){

		z = utility::split( it,'\t' ) ;

		if( z.size() >= 4 ){

			const QString& fs = z.at( 3 ) ;

			if( !fs.contains( "member" ) ){

				const QString& size = z.at( 1 ) ;

				if( size == "1.0 KB" || size == "Nil" ){
					continue ;
				}
				if( m_showEncryptedOnly ){
					if( fs.startsWith( "crypto" ) || fs.contains( "Nil" ) ){
						tablewidget::addRowToTable( m_ui->tableWidget,z ) ;
					}
				}else if( m_showLuksOnly ){
					if( fs.startsWith( "crypto" ) ){
						tablewidget::addRowToTable( m_ui->tableWidget,z ) ;
					}
				}else{
					tablewidget::addRowToTable( m_ui->tableWidget,z ) ;
				}
			}
		}
	}
}

void openvolume::HideUI()
{
	this->hide() ;
	emit HideUISignal() ;
}

void openvolume::tableEntryDoubleClicked( QTableWidgetItem * item )
{
	QString dev ;
	QTableWidget * tw = m_ui->tableWidget ;

	if( m_diableNonLUKS ){
		if( tw->item( item->row(),3 )->text() != "crypto_LUKS" ){
			DialogMsg m( this ) ;

			return m.ShowUIOK( tr( "ERROR" ),tr( "Only crypto_LUKS volumes can be selected" ) ) ;
		}else{
			m_ui->pbUUID->setFlat( true ) ;
		}

	}
	if( m_ui->pbUUID->isFlat() ){
		dev = "UUID=\"" + tw->item( item->row(),4 )->text() + "\"" ;
	}else{
		dev = tw->item( item->row(),0 )->text() ;
	}
	emit clickedPartition( dev ) ;
	this->HideUI() ;
}

void openvolume::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

openvolume::~openvolume()
{
	delete m_ui ;
}

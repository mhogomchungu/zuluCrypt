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

openvolume::openvolume( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::openvolume() )
{
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

	auto tw = m_ui->tableWidget ;

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

	m_ui->pbHelp->setVisible( false ) ;

	this->installEventFilter( this ) ;
}

bool openvolume::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

openvolume& openvolume::showEncryptedOnly()
{
	m_showEncryptedOnly = true ;
	return *this ;
}

openvolume& openvolume::showLuksOnly()
{
	m_showLuksOnly = true ;
	return *this ;
}

void openvolume::pbHelp()
{
	DialogMsg msg( this ) ;

	QString m ;

	if( m_option == openvolume::allVolumes ){

		m = tr( "A list of all partitions on this system are displayed here.\nDouble click an entry to use it" ) ;
	}else{
		if( !utility::userIsRoot() ){

			m = tr( "Restart the tool from root's account or after you have created and added yourself to group \"zulucrypt\" if the volume you want to use is not on the list." ) ;
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

	if( it ){

		this->tableEntryDoubleClicked( tw->item( it->row(),0 ) ) ;
	}
}

void openvolume::currentItemChanged( QTableWidgetItem * current, QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;
}

void openvolume::ShowNonSystemPartitions( std::function< void( const QString& ) > f )
{
	m_function = std::move( f ) ;

	this->partitionList( tr( "Select A Partition To Create An Encrypted Volume In" )," -N" ) ;
}

void openvolume::partitionList( const QString& p, const QString& q,std::function< void( const QString& )> f )
{
	m_function = std::move( f ) ;

	this->partitionList( p,q ) ;
}

void openvolume::ShowAllPartitions( std::function< void( const QString& ) > f )
{
	m_function = std::move( f ) ;

	this->partitionList( tr( "Select An Encrypted Partition To Open" )," -A" ) ;
}

void openvolume::allowLUKSOnly()
{
	m_diableNonLUKS = true ;
}

void openvolume::partitionList( const QString& title,const QString& volumeType )
{
	this->setWindowTitle( title ) ;

	tablewidget::clearTable( m_ui->tableWidget ) ;

	m_ui->tableWidget->setEnabled( false ) ;

	this->show() ;

	if( volumeType == " -N" ){

		m_option = openvolume::nonSystemVolumes ;

	}else if( volumeType == " -A" ){

		m_option = openvolume::allVolumes ;

	}else if( volumeType == " -S" ){

		m_option = openvolume::systemVolumes ;
	}

	QStringList r ;
	QStringList l ;

	Task::await( [ & ](){

		l = utility::Task( QString( "%1 -AZ" ).arg( ZULUCRYPTzuluCrypt ) ).splitOutput( '\n' ) ;

		r = utility::Task( QString( "%1 -S" ).arg( ZULUCRYPTzuluCrypt ) ).splitOutput( '\n' ) ;
	} ) ;

	for( const auto& it : l ){

		QFont nonSystem = this->font() ;

		QFont system = this->font() ;
		system.setItalic( !system.italic() ) ;
		system.setBold( !system.bold() ) ;

		std::reference_wrapper<QFont> font = system ;

		auto z = utility::split( it,'\t' ) ;

		if( z.size() >= 4 ){

			const auto& fs = z.at( 3 ) ;

			if( !fs.contains( "member" ) ){

				if( r.contains( z.first() ) ){

					font = system ;
				}else{
					font = nonSystem ;
				}

				const QString& size = z.at( 1 ) ;

				if( size == "1.0 KB" || size == "Nil" ){

					continue ;
				}
				if( m_showLuksOnly ){

					if( fs.startsWith( "crypto_LUKS" ) ){

						tablewidget::addRowToTable( m_ui->tableWidget,z,font ) ;
					}
				}else if( m_showEncryptedOnly ){

					if( fs.startsWith( "crypto" ) || fs.contains( "Nil" ) ){

						tablewidget::addRowToTable( m_ui->tableWidget,z,font ) ;
					}
				}else{
					tablewidget::addRowToTable( m_ui->tableWidget,z,font ) ;
				}
			}
		}
	}

	m_ui->tableWidget->setEnabled( true ) ;
	m_ui->tableWidget->setFocus() ;
}

void openvolume::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void openvolume::tableEntryDoubleClicked( QTableWidgetItem * item )
{
	auto tw = m_ui->tableWidget ;

	if( m_diableNonLUKS ){

		if( !tw->item( item->row(),3 )->text().startsWith( "crypto_LUKS" ) ){

			DialogMsg m( this ) ;

			return m.ShowUIOK( tr( "ERROR" ),tr( "Only crypto_LUKS volumes can be selected" ) ) ;
		}else{
			m_ui->pbUUID->setFlat( true ) ;
		}
	}

	if( m_option == openvolume::nonSystemVolumes ) {

		if( !( utility::userIsRoot() || utility::userBelongsToGroup( "zulucrypt" ) ) ){

			if( item->font().italic() != this->font().italic() ){

				DialogMsg m( this ) ;

				auto e = tr( "Insufficient privileges to operate on a system volume.\nRestart zuluCrypt from root's account to proceed" ) ;

				return m.ShowUIOK( tr( "ERROR" ),e ) ;
			}
		}
	}

	if( m_ui->pbUUID->isFlat() ){

		m_function( "UUID=\"" + tw->item( item->row(),4 )->text() + "\"" ) ;
	}else{
		m_function( tw->item( item->row(),0 )->text() ) ;
	}

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

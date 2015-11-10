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

#include "managesystemvolumes.h"
#include "ui_managesystemvolumes.h"

#include <QFileDialog>
#include <QFile>
#include <QTableWidgetItem>
#include <QStringList>
#include <QMessageBox>
#include <QList>
#include <QKeySequence>
#include <QAction>
#include <QCloseEvent>
#include <QCursor>
#include <QMenu>

#include "openvolume.h"
#include "dialogmsg.h"
#include "tablewidget.h"

manageSystemVolumes::manageSystemVolumes( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::manageSystemVolumes )
{
	m_ui->setupUi( this ) ;

	this->setFont( parent->font() ) ;
	this->setFixedSize( this->size() ) ;

	connect( m_ui->pbDone,SIGNAL( clicked() ),this,SLOT( pbDone() ) ) ;
	connect( m_ui->pbFile,SIGNAL( clicked() ),this,SLOT( pbFile() ) ) ;
	connect( m_ui->pbPartition,SIGNAL( clicked() ),this,SLOT( pbPartition() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),this,
		SLOT( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,
		SLOT( itemClicked( QTableWidgetItem * ) ) ) ;

	m_ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter ) ;
	keys.append( Qt::Key_Return ) ;
	keys.append( Qt::Key_Menu ) ;
	m_ac->setShortcuts( keys ) ;
	connect( m_ac,SIGNAL( triggered() ),this,SLOT( defaultButton() ) ) ;
	this->addAction( m_ac ) ;

	this->installEventFilter( this ) ;
}

bool manageSystemVolumes::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void manageSystemVolumes::defaultButton()
{
	if( m_ui->pbDone->hasFocus() ){

		this->pbDone() ;

	}else if( m_ui->pbFile->hasFocus() ){

		this->pbFile() ;

	}else if( m_ui->pbPartition->hasFocus() ){

		this->pbPartition() ;
	}else{
		this->contextMenu() ;
	}
}

void manageSystemVolumes::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;
}

void manageSystemVolumes::readSystemPartitions()
{
	QFile file( m_path ) ;

	if( file.open( QIODevice::ReadOnly ) ){

		auto l = utility::split( file.readAll() ) ;

		l.removeDuplicates() ;

		for( const auto& it : l ){

			this->addItemsToTable( it ) ;
		}
	}
}

void manageSystemVolumes::writeSystemPartitions()
{
	QFile file( m_path ) ;

	if( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "Could not open \"%1\" for writing" ).arg( m_path ) ) ;
	}else{
		auto table = m_ui->tableWidget ;

		int j = m_ui->tableWidget->rowCount() ;

		if( j > 0 ){

			for( int i = 0 ; i < j ; i++ ){

				auto it = table->item( i,0 ) ;

				file.write( it->text().toLatin1() ) ;

				file.putChar( '\n' ) ;
			}
		}

		file.setPermissions( QFile::ReadOwner|QFile::WriteOwner ) ;		
	}
}

void manageSystemVolumes::itemClicked( QTableWidgetItem * current )
{
	this->itemClicked( current,true ) ;
}

void manageSystemVolumes::contextMenu()
{
	this->itemClicked( m_ui->tableWidget->currentItem(),false ) ;
}

void manageSystemVolumes::itemClicked( QTableWidgetItem * current,bool clicked )
{
	if( current ){

		QMenu m ;
		m.setFont( this->font() ) ;
		connect( m.addAction( tr( "Remove Selected Entry" ) ),SIGNAL( triggered() ),this,SLOT( removeCurrentRow() ) ) ;

		m.addSeparator() ;
		m.addAction( tr( "Cancel" ) ) ;

		if( clicked ){

			m.exec( QCursor::pos() ) ;
		}else{
			int x = m_ui->tableWidget->columnWidth( 0 ) / 2 ;
			int y = m_ui->tableWidget->rowHeight( current->row() ) * current->row() + 20 ;
			m.exec( m_ui->tableWidget->mapToGlobal( QPoint( x,y ) ) ) ;
		}
	}
}

void manageSystemVolumes::removeCurrentRow()
{
	auto it = m_ui->tableWidget->currentItem() ;

	auto m = tr( "Are you sure you want to remove \n\"%1\"\n from the list?" ).arg( it->text() ) ;

	DialogMsg msg( this ) ;

	if( msg.ShowUIYesNoDefaultNo( tr( "WARNING"),m ) == QMessageBox::Yes ){

		tablewidget::deleteRowFromTable( m_ui->tableWidget,it->row() ) ;
	}
}

void manageSystemVolumes::addItemsToTable( QString path )
{
	if( path.isEmpty() ){

		m_ui->tableWidget->setFocus() ;
	}else{		 
		this->addItemsToTable( QStringList( path ) ) ;
	}
}

void manageSystemVolumes::addItemsToTable( QStringList paths )
{
	tablewidget::addRowToTable( m_ui->tableWidget,paths ) ;
}

void manageSystemVolumes::pbDone()
{
	this->writeSystemPartitions() ;
	this->HideUI() ;
}

void manageSystemVolumes::pbFile()
{
	auto Z = QFileDialog::getOpenFileName( this,tr( "Select Path To System Volume" ),utility::homePath(),0 ) ;

	this->addItemsToTable( Z ) ;
}

void manageSystemVolumes::pbPartition()
{
	openvolume::instance( this ).ShowAllPartitions( [ this ]( const QString& e ){

		m_ui->tableWidget->setFocus() ;

		this->addItemsToTable( e ) ;
	} ) ;
}

void manageSystemVolumes::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbDone() ;
}

void manageSystemVolumes::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void manageSystemVolumes::ShowUI( const QString& path )
{
	m_path = path ;
	this->readSystemPartitions() ;
	m_ui->tableWidget->setColumnWidth( 0,580 ) ;
	this->show() ;
}

manageSystemVolumes::~manageSystemVolumes()
{
	delete m_ui ;
}

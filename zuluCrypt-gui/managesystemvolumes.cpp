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

manageSystemVolumes::manageSystemVolumes( QWidget * parent,const char * s ) :
	QDialog( parent ),m_ui( new Ui::manageSystemVolumes ),m_path( s )
{
	m_ui->setupUi( this ) ;

	m_ui->tableWidget->setColumnWidth( 0,580 ) ;

	this->setFont( parent->font() ) ;
	this->setFixedSize( this->size() ) ;

	connect( m_ui->pbDone,SIGNAL( clicked() ),this,SLOT( pbDone() ) ) ;
	connect( m_ui->pbFile,SIGNAL( clicked() ),this,SLOT( pbFile() ) ) ;
	connect( m_ui->pbPartition,SIGNAL( clicked() ),this,SLOT( pbPartition() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),this,
		SLOT( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,
		SLOT( itemClicked( QTableWidgetItem * ) ) ) ;

	this->addAction( [ this ](){

		auto ac = new QAction( this ) ;

		QList<QKeySequence> keys ;

		keys.append( Qt::Key_Enter ) ;
		keys.append( Qt::Key_Return ) ;
		keys.append( Qt::Key_Menu ) ;

		ac->setShortcuts( keys ) ;

		connect( ac,SIGNAL( triggered() ),this,SLOT( defaultButton() ) ) ;

		return ac ;
	}() ) ;

	this->installEventFilter( this ) ;

	this->readSystemPartitions() ;
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
	tablewidget::selectRow( current,previous ) ;
}

void manageSystemVolumes::readSystemPartitions()
{
	auto e = utility::privilegedReadConfigFile( m_path ) ;

	m_read = e.first ;

	if( m_read ){

		auto s = utility::split( e.second ) ;

		s.removeDuplicates() ;

		for( const auto& it : s ){

			this->addItemsToTable( { it } ) ;
		}

		this->show() ;
	}else{
		m_ui->pbFile->setEnabled( false ) ;
		m_ui->pbPartition->setEnabled( false ) ;
		m_ui->tableWidget->setEnabled( false ) ;
	}
}

void manageSystemVolumes::writeSystemPartitions()
{
	if( m_read ){

		auto table = m_ui->tableWidget ;

		int j = m_ui->tableWidget->rowCount() ;

		QByteArray s ;

		for( int i = 0 ; i < j ; i++ ){

			s += table->item( i,0 )->text().toLatin1() + "\n" ;
		}

		utility::privilegedWriteConfigFile( s,m_path ) ;
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

		tablewidget::selectRow( m_ui->tableWidget,current->row() ) ;

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

		tablewidget::deleteRow( m_ui->tableWidget,it->row() ) ;
	}
}

void manageSystemVolumes::addItemsToTable( const QStringList& paths )
{
	if( !paths.isEmpty() ){

		tablewidget::addRow( m_ui->tableWidget,paths ) ;
	}

	m_ui->tableWidget->setFocus() ;
}

void manageSystemVolumes::pbDone()
{
	this->writeSystemPartitions() ;
	this->HideUI() ;
}

void manageSystemVolumes::pbFile()
{
	auto e = QFileDialog::getOpenFileName( this,tr( "Select Path To System Volume" ),utility::homePath() ) ;

	this->addItemsToTable( { e } ) ;
}

void manageSystemVolumes::pbPartition()
{
	openvolume::instance( this ).ShowAllPartitions( [ this ]( const QString& e ){

		m_ui->tableWidget->setFocus() ;

		this->addItemsToTable( { e } ) ;
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

manageSystemVolumes::~manageSystemVolumes()
{
	delete m_ui ;
}

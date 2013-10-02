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

#include "managedevicenames.h"
#include "ui_managedevicenames.h"
#include <iostream>

#include <QTableWidgetItem>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QMenu>

#include "openvolume.h"
#include "utility.h"
#include "dialogmsg.h"
#include "tablewidget.h"

managedevicenames::managedevicenames( QWidget * parent ) :
	QDialog( parent ),
	m_ui( new Ui::managedevicenames )
{
	m_ui->setupUi( this ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;

	connect( m_ui->pbDeviceAddress,SIGNAL( clicked() ),this,SLOT( deviceAddress() ) ) ;
	connect( m_ui->pbAdd,SIGNAL( clicked() ),this,SLOT( add() ) ) ;
	connect( m_ui->pbFileAddress,SIGNAL( clicked() ),this,SLOT( fileAddress() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( cancel() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),this,
		SLOT( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,
		SLOT( itemClicked( QTableWidgetItem * ) ) ) ;
	connect( m_ui->lineEditDeviceAddress,SIGNAL( textChanged( QString ) ),this,SLOT( devicePathTextChange( QString ) ) ) ;

	m_ui->pbFileAddress->setIcon( QIcon( QString( ":/keyfile.png" ) ) ) ;
	m_ui->pbDeviceAddress->setIcon( QIcon( QString( ":/partition.png" ) ) ) ;

	m_ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter ) ;
	keys.append( Qt::Key_Return ) ;
	keys.append( Qt::Key_Menu ) ;
	m_ac->setShortcuts( keys ) ;
	connect( m_ac,SIGNAL( triggered() ),this,SLOT( shortcutPressed() ) ) ;
	this->addAction( m_ac ) ;
}

void managedevicenames::devicePathTextChange( QString txt )
{
	if( txt.isEmpty() ){
		m_ui->lineEditMountPath->clear() ; ;
	}else{
		QString ed = QString( "/run/media/private/%1/%2" ).arg( utility::userName() ).arg( txt.split( "/" ).last() ) ;
		m_ui->lineEditMountPath->setText( ed ) ;
	}
}

void managedevicenames::shortcutPressed()
{
	QTableWidgetItem * it = m_ui->tableWidget->currentItem() ;
	itemClicked( it,false ) ;
}

void managedevicenames::deviceAddress()
{
	openvolume * op = new openvolume( this ) ;
	connect( op,SIGNAL( clickedPartition( QString ) ),this,SLOT( PartitionEntry( QString ) ) ) ;
	connect( op,SIGNAL( HideUISignal() ),op,SLOT( deleteLater() ) ) ;
	op->ShowAllPartitions() ;
}

void managedevicenames::ShowUI()
{
	m_ui->tableWidget->setColumnWidth( 0,285 ) ;
	m_ui->tableWidget->setColumnWidth( 1,285 ) ;

	while( m_ui->tableWidget->rowCount() > 0 ){
		m_ui->tableWidget->removeRow( 0 ) ;
	}
	QStringList entries = utility::readFavorites() ;
	QStringList line ;
	int j = entries.size() - 1 ;

	for( int i = 0 ; i < j ; i++ ){
		line = entries.at( i ).split( "\t" ) ;
		addEntries( line.at( 0 ),line.at( 1 ) ) ;
	}

	m_ui->lineEditDeviceAddress->clear() ;
	m_ui->lineEditMountPath->clear() ;
	m_ui->tableWidget->setFocus() ;
	this->show() ;
}

void managedevicenames::HideUI()
{
	this->hide() ;
	emit HideUISignal() ;
}

void managedevicenames::addEntries( QString dev,QString m_point )
{
	QStringList s ;

	s.append( dev ) ;
	s.append( m_point ) ;

	tablewidget::addRowToTable( m_ui->tableWidget,s ) ;
}

void managedevicenames::itemClicked( QTableWidgetItem * current )
{
	this->itemClicked( current,true ) ;
}

void managedevicenames::itemClicked( QTableWidgetItem * current,bool clicked )
{
	QMenu m ;
	m.setFont( this->font() ) ;
	connect( m.addAction( tr( "remove selected entry" ) ),SIGNAL( triggered() ),this,SLOT( removeEntryFromFavoriteList() ) ) ;

	m.addSeparator() ;
	m.addAction( tr( "cancel" ) ) ;

	if( clicked ){
		m.exec( QCursor::pos() ) ;
	}else{
		int x = m_ui->tableWidget->columnWidth( 0 ) ;
		int y = m_ui->tableWidget->rowHeight( current->row() ) * current->row() + 20 ;
		m.exec( m_ui->tableWidget->mapToGlobal( QPoint( x,y ) ) ) ;
	}
}

void managedevicenames::removeEntryFromFavoriteList()
{
	QTableWidget * table = m_ui->tableWidget ;

	table->setEnabled( false ) ;

	int row = table->currentRow() ;

	QString txt1 = table->item( row,0 )->text() ;
	QString txt2 = table->item( row,1 )->text() ;

	QString entry = QString( "%1\t%2\n" ).arg( txt1 ).arg( txt2 ) ;

	utility::removeFavoriteEntry( entry ) ;

	tablewidget::deleteRowFromTable( table,row ) ;

	table->setEnabled( true ) ;
}

void managedevicenames::cancel()
{
	this->HideUI() ;
}

void managedevicenames::add()
{
	DialogMsg msg( this ) ;

	QString dev = m_ui->lineEditDeviceAddress->text() ;
	QString mount_point = m_ui->lineEditMountPath->text() ;

	if( dev.isEmpty() ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "device address field is empty" ) ) ;
	}
	if( mount_point.isEmpty() ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "mount point path field is empty" ) ) ;
	}
	m_ui->tableWidget->setEnabled( false ) ;
	this->addEntries( dev,mount_point ) ;

	utility::addToFavorite( dev,mount_point ) ;

	m_ui->lineEditDeviceAddress->clear() ; ;
	m_ui->lineEditMountPath->clear() ;

	m_ui->tableWidget->setEnabled( true ) ;
}

void managedevicenames::fileAddress()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "path to an encrypted file" ),QDir::homePath(),0 ) ;
	m_ui->lineEditDeviceAddress->setText( Z ) ;
}

void managedevicenames::PartitionEntry( QString device )
{
	m_ui->lineEditDeviceAddress->setText( device ) ;
}

managedevicenames::~managedevicenames()
{
	delete m_ui ;
}

void managedevicenames::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

void managedevicenames::currentItemChanged( QTableWidgetItem * current, QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;
}

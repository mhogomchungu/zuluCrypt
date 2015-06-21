/*
 *
 *  Copyright (c) 2014-2015
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

#include "dialogmsg.h"
#include "tcrypt.h"
#include "ui_tcrypt.h"

#include "tablewidget.h"

#include <QCloseEvent>
#include <QDropEvent>
#include <QUrl>
#include <QDebug>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QMimeData>

tcrypt::tcrypt( QWidget * parent) : QDialog( parent ),m_ui( new Ui::tcrypt )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;

	connect( m_ui->pbSend,SIGNAL( clicked() ),this,SLOT( pbSend() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbAddKeyFile,SIGNAL( clicked() ),this,SLOT( pbAddKeyFIle() ) ) ;

	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT(currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),
		 this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;

	this->installEventFilter( this ) ;
	m_ui->tableWidget->setColumnWidth( 0,426 ) ;
}

void tcrypt::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;
}

void tcrypt::itemClicked( QTableWidgetItem * item )
{
	Q_UNUSED( item ) ;
}

bool tcrypt::eventFilter( QObject * watched,QEvent * event )
{
	if( utility::eventFilter( this,watched,event ) ){
		this->HideUI() ;
		return true ;
	}else{
		return false ;
	}
}

void tcrypt::ShowUI()
{
	this->show() ;
}

void tcrypt::ShowUI_1()
{
	m_ui->pbSend->setText( tr( "set" ) ) ;
	this->show() ;
}

void tcrypt::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void tcrypt::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

void tcrypt::dragEnterEvent( QDragEnterEvent * e )
{
	e->accept() ;
}

void tcrypt::dropEvent( QDropEvent * e )
{
	const QMimeData * m = e->mimeData() ;
	QList<QUrl> l = m->urls() ;

	QTableWidget * table = m_ui->tableWidget ;

	QStringList k ;
	for( const auto& it : l ){
		k.clear() ;
		k.append( it.path() ) ;
		tablewidget::addRowToTable( table,k ) ;
	}
}

void tcrypt::pbSend()
{
	QStringList l = tablewidget::tableColumnEntries( m_ui->tableWidget ) ;

	if( l.isEmpty() ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "At least one keyfile is required" ) ) ;
	}else{
		/*
		 * we call hide() because the GUI doesnt seem to hide when called later on in HideUI()
		 */
		this->hide() ;

		emit Keys( m_ui->lineEdit->text(),l ) ;
		this->HideUI() ;
	}
}

void tcrypt::pbCancel()
{
	emit cancelled() ;
	this->HideUI() ;
}

void tcrypt::pbAddKeyFIle()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "Select A Keyfile" ),utility::homePath(),0 ) ;
	if( !Z.isEmpty() ){
		QTableWidget * table = m_ui->tableWidget ;
		QStringList l ;
		l.append( Z ) ;
		tablewidget::addRowToTable( table,l ) ;
	}
}

tcrypt::~tcrypt()
{
	delete m_ui ;
}

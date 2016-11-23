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

tcrypt::tcrypt( QWidget * parent,bool e,
		std::function< void( const QString&,const QStringList& ) > p,
		std::function< void() > q ) :
	QDialog( parent ),m_ui( new Ui::tcrypt ),m_success( std::move( p ) ),m_cancelled( std::move( q ) )
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

	if( e ){

		m_ui->pbSend->setText( tr( "&Set" ) ) ;
	}

	this->setWindowTitle( tr( "TrueCrypt/VeraCrypt Keys" ) ) ;
	this->show() ;
}

void tcrypt::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectRow( current,previous ) ;
}

void tcrypt::itemClicked( QTableWidgetItem * item )
{
	Q_UNUSED( item ) ;
}

bool tcrypt::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
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
	for( const auto& it : e->mimeData()->urls() ){

		tablewidget::addRow( m_ui->tableWidget,{ it.path() } ) ;
	}
}

void tcrypt::pbSend()
{
	auto l = tablewidget::columnEntries( m_ui->tableWidget ) ;

	if( l.isEmpty() ){

		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "At least one keyfile is required" ) ) ;
	}else{
		/*
		 * we call hide() because the GUI doesnt seem to hide when called later on in HideUI()
		 */
		this->hide() ;

		m_success( m_ui->lineEdit->text(),l ) ;

		this->HideUI() ;
	}
}

void tcrypt::pbCancel()
{
	m_cancelled() ;

	this->HideUI() ;
}

void tcrypt::pbAddKeyFIle()
{
	auto e = QFileDialog::getOpenFileName( this,tr( "Select A KeyFile" ),utility::homePath() ) ;

	if( !e.isEmpty() ){

		tablewidget::addRow( m_ui->tableWidget,{ e } ) ;
	}
}

tcrypt::~tcrypt()
{
	delete m_ui ;
}

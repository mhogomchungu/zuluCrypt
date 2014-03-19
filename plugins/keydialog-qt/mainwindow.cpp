/*
 *
 *  Copyright (c) 2012
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow( QWidget * parent ) : QWidget( parent ),m_ui( new Ui::MainWindow )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;

	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;

	this->setWindowIcon( QIcon( QString( ":/key.png" ) ) ) ;

	QAction * ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter ) ;
	keys.append( Qt::Key_Return ) ;
	ac->setShortcuts( keys ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( defaultButton() ) ) ;
	this->addAction( ac ) ;

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;

	this->SetFocus() ;
}

void MainWindow::defaultButton()
{
	if( m_ui->pbCancel->hasFocus() ){
		this->pbCancel() ;
	}else{
		this->pbOpen() ;
	}
}

void MainWindow::SetAddr( QString addr )
{
	m_addr = addr ;
	m_handle = socketSendKey::zuluCryptPluginManagerOpenConnection( m_addr ) ;
}

void MainWindow::SetFocus()
{
	if( m_ui->lineEditKey->text().isEmpty() ){
		m_ui->lineEditKey->setFocus() ;
	}else{
		m_ui->pbOpen->setFocus() ;
	}
}

void MainWindow::pbCancel()
{
	/*
	 * Send a single NULL character to inform the backend that we were cancelled
	 */
	char c = '\0' ;
	QByteArray key( &c,1 ) ;
	socketSendKey::zuluCryptPluginManagerSendKey( m_handle,key ) ;
	this->done() ;
}

void MainWindow::done()
{
	this->hide() ;
	QCoreApplication::exit() ;
}
void MainWindow::pbOpen()
{
	QByteArray key = m_ui->lineEditKey->text().toLatin1() ;

	socketSendKey::zuluCryptPluginManagerSendKey( m_handle,key ) ;

	this->done() ;
}

MainWindow::~MainWindow()
{
	socketSendKey::zuluCryptPluginManagerCloseConnection( m_handle ) ;
	delete m_ui ;
}

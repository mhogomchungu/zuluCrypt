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

	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;

	this->setWindowIcon( QIcon( QString( ":/keyfile.png" ) ) ) ;
	m_ui->pbKeyFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) ) ;

	QAction * ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter ) ;
	keys.append( Qt::Key_Return ) ;
	ac->setShortcuts( keys ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( defaultButton() ) ) ;
	this->addAction( ac ) ;

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;

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
	}else if( m_ui->lineEditKeyFile->text().isEmpty() ){
		m_ui->lineEditKeyFile->setFocus() ;
	}else{
		m_ui->pbOpen->setFocus() ;
	}
}

void MainWindow::pbCancel()
{
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

	m_keyFile = m_ui->lineEditKeyFile->text() ;

	if( m_keyFile.isEmpty() || key.isEmpty() ){
		DialogMsg msg( this ) ;
		return msg.ShowUIOK( tr( "ERROR" ),tr( "atleast one required field is empty" ) ) ;
	}

	QString kar = m_keyFile.mid( 0,1 ) ;

	if( kar == QString( "~" ) )
		m_keyFile = QDir::homePath() + QString( "/" ) + m_keyFile.mid( 1 ) ;

	QFile file( m_keyFile ) ;

	if( file.open( QIODevice::ReadOnly ) ){
		key = key + file.readAll() ;
	}else{
		DialogMsg msg( this ) ;
		return msg.ShowUIOK( tr( "ERROR" ),tr( "could not open keyfile for reading" ) ) ;
	}

	socketSendKey::zuluCryptPluginManagerSendKey( m_handle,key ) ;

	this->done() ;
}

void MainWindow::pbKeyFile()
{
	QString Z = QFileDialog::getOpenFileName( this,QString( "select a key file" ),QDir::homePath() ) ;

	if( !Z.isEmpty() ){
		m_ui->lineEditKeyFile->setText( Z ) ;
	}
	this->SetFocus() ;
}

MainWindow::~MainWindow()
{
	socketSendKey::zuluCryptPluginManagerCloseConnection( m_handle ) ;
	delete m_ui;
}

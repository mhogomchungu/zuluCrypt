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

MainWindow::MainWindow( QWidget * parent ) : QMainWindow( parent ),m_ui( new Ui::MainWindow )
{
	m_ui->setupUi( this );

	m_ui->lineEditKey->setEchoMode( QLineEdit::Password );

	this->setWindowIcon( QIcon( QString( ":/keyfile.png" ) ) );
	m_ui->pbKeyFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) );

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;

	this->SetFocus();
	m_ui->pbOpen->setEnabled( false );

	m_zuluSocket = 0 ;
}

void MainWindow::SetAddr( QString addr )
{
	m_addr = addr ;
	m_zuluSocket = new zuluSocket() ;
	m_zuluSocket->startServer( addr );
	connect( m_zuluSocket,SIGNAL( gotConnected() ),this,SLOT( gotConnected() ) ) ;
}

void MainWindow::gotConnected()
{
	m_ui->pbOpen->setEnabled( true );
}

void MainWindow::SetFocus()
{
	if( m_ui->lineEditKey->text().isEmpty() )
		m_ui->lineEditKey->setFocus();
	else if( m_ui->lineEditKeyFile->text().isEmpty() )
		m_ui->lineEditKeyFile->setFocus();
	else
		m_ui->pbOpen->setFocus();
}

void MainWindow::pbCancel()
{
	this->Exit();
}

void MainWindow::Exit()
{
	this->hide();
	QCoreApplication::quit() ;
}

void MainWindow::pbOpen()
{
	QByteArray key = m_ui->lineEditKey->text().toAscii() ;

	m_keyFile = m_ui->lineEditKeyFile->text() ;

	if( !m_keyFile.isEmpty() ){

		QString kar = m_keyFile.mid( 0,1 ) ;

		if( kar == QString( "~" ) )
			m_keyFile = QDir::homePath() + QString( "/" ) + m_keyFile.mid( 1 ) ;

		if( kar != QString( "/" ) )
			m_keyFile = QDir::homePath() + QString( "/" ) + m_keyFile ;

		QFile file( m_keyFile ) ;

		if( file.open( QIODevice::ReadOnly ) ){
			if( key.isEmpty() )
				key = file.readAll() ;
			else
				key = key + file.readAll() ;
			file.close();
		}
	}

	m_zuluSocket->sendData( key );
	this->Exit();
}

void MainWindow::pbKeyFile()
{
	QString Z = QFileDialog::getOpenFileName( this,QString( "select a key file" ),QDir::homePath() ) ;

	if( !Z.isEmpty() )
		m_ui->lineEditKeyFile->setText( Z );
	this->SetFocus();
}

MainWindow::~MainWindow()
{
	if( m_zuluSocket )
		m_zuluSocket->deleteLater();

	QFile::remove( m_addr ) ;

	delete m_ui;
}

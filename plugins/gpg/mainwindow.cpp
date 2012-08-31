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

	m_client = 0 ;
	m_server = 0 ;

	this->setWindowIcon( QIcon( QString( ":/keyfile.png" ) ) );
	m_ui->pbKeyFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) );

	m_server = new QLocalServer( this ) ;
	connect( m_server,SIGNAL( newConnection() ),this,SLOT( acceptConnection() ) ) ;

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;

	this->SetFocus();
	m_ui->pbOpen->setEnabled( false );
	m_ui->lineEditKeyFile->setText( "/home/ink/gpg.gpg" );
}

void MainWindow::SetAddr( QString addr )
{
	m_addr = addr ;
	m_server->listen( m_addr ) ;
}

void MainWindow::acceptConnection()
{
	m_client = m_server->nextPendingConnection() ;
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
	if( m_client ){
		if( m_client->isOpen() )
			m_client->close();

		m_client->deleteLater();
	}

	if( m_server ){
		m_server->close();
		m_server->deleteLater();
	}

	QFile::remove( m_addr ) ;

	this->hide();

	QCoreApplication::quit() ;
}

#include <QDebug>
void MainWindow::pbOpen()
{
	QProcess p ;
	m_ui->lineEditKeyFile->setText( "/home/ink/gpg.gpg" );
	QString EXE = QString( "/usr/bin/gpg" ) ;
	QString path = m_ui->lineEditKeyFile->text().replace( "\"","\"\"\"" ) ;
	QString key = m_ui->lineEditKey->text() ;

	QString exe = QString( "%1 --batch --passphrase-fd 0 -d \"%2\"" ).arg( EXE ).arg( path ) ;

	p.start( exe );
	p.waitForStarted() ;
	p.write( key.toAscii() ) ;
	p.waitForFinished() ;

	QByteArray data = p.readAll() ;

	if( !data.isEmpty() ){
		qDebug() << data;
		qDebug() << m_client->write( data ) ;
		m_client->flush() ;
		m_client->close();
	}else
		qDebug() << "data is empty";

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
	delete m_ui;
}

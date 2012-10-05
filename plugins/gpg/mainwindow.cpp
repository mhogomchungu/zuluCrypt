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
#include <QDebug>

MainWindow::MainWindow( QWidget * parent ) : QMainWindow( parent ),m_ui( new Ui::MainWindow )
{
	m_ui->setupUi( this );
	this->setFixedSize( this->size() );

	m_ui->lineEditKey->setEchoMode( QLineEdit::Password );

	this->setWindowIcon( QIcon( QString( ":/gpg.png" ) ) );
	m_ui->pbKeyFile->setIcon( QIcon( QString( ":/gpg.png" ) ) );

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;

	m_ui->lineEditKey->setFocus();

	m_working = false ;
	m_closeBackEnd = true ;

	this->setWindowTitle( QString( "gpg key module" ) );
}

void MainWindow::SetAddr( QString addr )
{
	m_addr = addr ;
}

void MainWindow::gotConnected()
{
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
	if( m_working ){
		DialogMsg msg( this ) ;
		int st = msg.ShowUIYesNoDefaultNo( QString( "warning"),QString( "are you sure you want to terminate this operation prematurely?" )) ;

		if( st == QMessageBox::Yes ){
			this->enableAlll();
			emit cancel();
		}
	}else{
		this->Exit( 1 );
	}
}

void MainWindow::Exit( int st )
{
	this->hide();
	QCoreApplication::exit( st );
}

QString MainWindow::FindGPG()
{
	if( QFile::exists( QString( "/usr/local/bin/gpg" ) ) )
		return QString( "/usr/local/bin/gpg" ) ;
	else if( QFile::exists( QString( "/usr/bin/gpg" ) ) )
		return QString( "/usr/bin/gpg" ) ;
	else if( QFile::exists( QString( "/usr/sbin/gpg") ) )
		return QString( "/usr/sbin/gpg" ) ;
	else{
		QString m ;
		return m ;
	}
}

void MainWindow::startingToreadData()
{
}

void MainWindow::bytesRead( int bytes )
{
	QString msg = QString( "number of bytes read from gpg keyfile: " ) + QString::number( bytes ) ;
	this->setWindowTitle( msg );
}

void MainWindow::doneReading()
{
}

void MainWindow::getGPGKey( bool cancelled,QByteArray data )
{
	if( cancelled ){
		m_working = false ;
		return ;
	}

	if( !data.isEmpty() ){
		m_ui->pbCancel->setEnabled( false );
		socketSendKey * s = new socketSendKey( this,m_addr,data ) ;
		connect( s,SIGNAL( keySent() ),this,SLOT( doneWritingData() ) ) ;
		s->sendKey();
		m_closeBackEnd = false ;
	}else{
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr("could not decrept the gpg keyfile" ) );
		this->Exit( 1 );
		m_working = false ;
		//msg.ShowUIOK( tr( "ERROR" ),tr("could not decrept the gpg keyfile,wrong key?" ) );
		//this->enableAlll();
		//m_ui->lineEditKey->setFocus();
		//this->setWindowTitle( QString( "gpg key module" ) );
	}
}

void MainWindow::pbOpen()
{
	QString path = m_ui->lineEditKeyFile->text() ;

	DialogMsg msg( this ) ;

	if( path.isEmpty() )
		return msg.ShowUIOK( tr( "ERROR" ),tr( "path to gpg keyfile is empty" ) ) ;

	if( !QFile::exists( path ) )
		return msg.ShowUIOK( tr( "ERROR" ),tr( "invalid path to gpg keyfile" ) ) ;

	QString exe = this->FindGPG() ;

	if( exe.isEmpty() )
		return msg.ShowUIOK( tr( "ERROR" ),tr( "could not find \"gpg\" executable in \"/usr/local\",\"/usr/bin\" and \"/usr/sbin\"" ) ) ;

	this->disableAll();
	m_working = true ;

	getgpgkey * gpg = new getgpgkey( exe,m_ui->lineEditKey->text(),m_ui->lineEditKeyFile->text() ) ;
	connect( gpg,SIGNAL( key( bool,QByteArray ) ),this,SLOT( getGPGKey( bool,QByteArray ) ) ) ;
	connect( this,SIGNAL( cancel() ),gpg,SLOT( cancel() ) ) ;
	connect( gpg,SIGNAL( bytesRead( int ) ),this,SLOT( bytesRead( int ) ) ) ;
	connect( gpg,SIGNAL( doneReadingFromgpg() ),this,SLOT( doneReading() ) ) ;
	gpg->start();
}

void MainWindow::doneWritingData()
{
	m_working = false ;
	this->Exit( 0 );
}

void MainWindow::pbKeyFile()
{
	QString Z = QFileDialog::getOpenFileName( this,QString( "select a key file" ),QDir::homePath() ) ;

	if( !Z.isEmpty() )
		m_ui->lineEditKeyFile->setText( Z );
	this->SetFocus();
}

void MainWindow::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->pbCancel();
}

void MainWindow::disableAll()
{
	m_ui->label->setEnabled( false );
	m_ui->label_2->setEnabled( false );
	m_ui->lineEditKey->setEnabled( false );
	m_ui->lineEditKeyFile->setEnabled( false );
	m_ui->pbKeyFile->setEnabled( false );
	m_ui->pbOpen->setEnabled( false );
}

void MainWindow::enableAlll()
{
	m_ui->label->setEnabled( true );
	m_ui->label_2->setEnabled( true );
	m_ui->lineEditKey->setEnabled( true );
	m_ui->lineEditKeyFile->setEnabled( true );
	m_ui->pbKeyFile->setEnabled( true );
	m_ui->pbOpen->setEnabled( true );
	m_ui->pbCancel->setEnabled( true );
}

MainWindow::~MainWindow()
{
	if( m_closeBackEnd )
		socketSendKey::openAndCloseConnection( m_addr ) ;
	delete m_ui;
}

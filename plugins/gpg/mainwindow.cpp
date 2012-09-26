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
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <QDebug>
MainWindow::MainWindow( QWidget * parent ) : QMainWindow( parent ),m_ui( new Ui::MainWindow )
{
	m_ui->setupUi( this );

	m_ui->lineEditKey->setEchoMode( QLineEdit::Password );

	this->setWindowIcon( QIcon( QString( ":/gpg.png" ) ) );
	m_ui->pbKeyFile->setIcon( QIcon( QString( ":/gpg.png" ) ) );

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;

	this->SetFocus();
}

void MainWindow::SetAddr( QString addr )
{
	m_addr = addr ;
	m_handle = socketSendKey::zuluCryptPluginManagerOpenConnection( m_addr ) ;
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
	this->Exit( 1 );
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

QByteArray MainWindow::getGPGKey( QString EXE,QString key,QString path )
{
	int fd_write_to_gpg[2] ;
	int fd_read_from_gpg[2] ;

	pipe( fd_write_to_gpg );

	pipe( fd_read_from_gpg ) ;

	int pid = fork() ;

	if( pid == -1 ){
		QByteArray nnggrr ;
		return nnggrr ;
	}

	if( pid == 0 ){
		::close( fd_write_to_gpg[ 1 ] );
		dup2( fd_read_from_gpg[ 1 ],1 ) ;
		::close( 2 ) ;

		QByteArray fdb = QString::number( fd_write_to_gpg[ 0 ] ).toAscii() ;
		const char * fd = fdb.constData() ;

		QDir dir( path ) ;
		QByteArray patharray = dir.canonicalPath().toAscii() ;
		const char * pathChar = patharray.constData() ;

		QByteArray exe = EXE.toAscii() ;
		execl( exe.constData(),
		       "--bash",
			"--no-tty",
			"--yes",
			"--no-mdc-warning",
			"--no-verbose",
			"--passphrase-fd",
			fd,
			"-d",
			pathChar,
			( void *)0 ) ;
		_Exit( 1 ); // shouldnt get here
	}else{
		::close( 2 ) ;
		::close( fd_write_to_gpg[ 0 ] );
		::close( fd_read_from_gpg[ 1 ] ) ;

		QByteArray keyData = key.toAscii() ;
		write( fd_write_to_gpg[ 1 ],keyData.constData(),keyData.size() ) ;
		::close( fd_write_to_gpg[ 1 ] ) ;

		char * buffer = NULL ;
		int i = 0 ;
		char c ;

		while( read( fd_read_from_gpg[ 0 ],&c,1 ) ){
			buffer = ( char * )realloc( buffer,i + 1 ) ;
			buffer[ i ] = c ;
			i++ ;
		}

		waitpid( pid,NULL,0 ) ;

		QByteArray data = QByteArray( buffer,i ) ;

		free( buffer ) ;

		return data ;
	}

	QByteArray shouldntGetHere ;
	return shouldntGetHere ;
}

void MainWindow::pbOpen()
{
	QString path = m_ui->lineEditKeyFile->text() ;

	DialogMsg msg( this ) ;

	if( path.isEmpty() )
		return msg.ShowUIOK( tr( "ERROR" ),tr( "path to gpg keyfile is empty" ) ) ;

	if( !QFile::exists( path ) )
		return msg.ShowUIOK( tr( "ERROR" ),tr( "invalid path to gpg keyfile" ) ) ;

	QString key = m_ui->lineEditKey->text() ;

	this->disableAll();
	QString EXE = this->FindGPG() ;

	if( EXE.isEmpty() )
		return msg.ShowUIOK( tr( "ERROR" ),tr( "could not find \"gpg\" executable in \"/usr/local\",\"/usr/bin\" and \"/usr/sbin\"" ) ) ;

	QByteArray data ;
	if( key.isEmpty() ){
		QProcess p ;
		QString exe = QString( "%1 --batch -d \"%2\"" ).arg( EXE ).arg( path ) ;
		p.start( exe );
		p.waitForFinished() ;
		data = p.readAll() ;
		p.close();
	}else{
		data = this->getGPGKey( EXE,m_ui->lineEditKey->text(),m_ui->lineEditKeyFile->text() ) ;
	}

	if( !data.isEmpty() ){
		socketSendKey::zuluCryptPluginManagerSendKey( m_handle,data ) ;
		this->doneWritingData();
	}else{
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr("could not decrept the gpg keyfile,wrong key?" ) );
		this->enableAlll();
	}
}

void MainWindow::doneWritingData()
{
	this->enableAlll();
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
	this->doneWritingData();
}

void MainWindow::disableAll()
{
	m_ui->label->setEnabled( false );
	m_ui->label_2->setEnabled( false );
	m_ui->lineEditKey->setEnabled( false );
	m_ui->lineEditKeyFile->setEnabled( false );
	m_ui->pbKeyFile->setEnabled( false );
	m_ui->pbOpen->setEnabled( false );
	m_ui->pbCancel->setEnabled( false );
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
	socketSendKey::zuluCryptPluginManagerCloseConnection( m_handle ) ;
	delete m_ui;
}

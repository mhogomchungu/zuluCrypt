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

#include <sys/types.h>
#include <unistd.h>

MainWindow::MainWindow( QWidget * parent ) : QWidget( parent ),m_ui( new Ui::MainWindow )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;

	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;

	this->setWindowIcon( QIcon( QString( ":/gpg.png" ) ) ) ;
	m_ui->pbKeyFile->setIcon( QIcon( QString( ":/gpg.png" ) ) );

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;

	m_ui->lineEditKey->setFocus() ;

	m_working = false ;

	this->setWindowTitle( tr( "%1 key module" ).arg( m_appName ) ) ;

	QAction * ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter );
	keys.append( Qt::Key_Return );
	ac->setShortcuts( keys ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( defaultButton() ) ) ;
	this->addAction( ac ) ;

	m_sendKey = new socketSendKey() ;
	connect( m_sendKey,SIGNAL( keySent() ),this,SLOT( doneWritingData() ) ) ;
}

void MainWindow::defaultButton()
{
	if( m_ui->pbCancel->hasFocus() ){
		this->pbCancel() ;
	}else{
		this->pbOpen() ;
	}
}

void MainWindow::setAddr( const QString& addr )
{
	m_addr = addr ;
	m_sendKey->setAddr( m_addr ) ;
	if( !m_sendKey->openConnection() ){
		this->Exit( 1 ) ;
	}
}

void MainWindow::setApplicationName( const QString& appName )
{
	m_appName = appName ;
}

void MainWindow::setKeyRoutine( std::function<QByteArray( const QString& exe,const QString& keyFile,const QString& password )> function )
{
	m_function = function ;
}

void MainWindow::gotConnected()
{
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
	if( m_working ){
		DialogMsg msg( this ) ;
		int st = msg.ShowUIYesNoDefaultNo( tr( "warning"),tr( "are you sure you want to terminate this operation prematurely?" )) ;

		if( st == QMessageBox::Yes ){
			this->enableAlll() ;
			m_working = false ;
			emit cancel() ;
		}
	}else{
		this->Exit( 1 ) ;
	}
}

void MainWindow::Exit( int st )
{
	char * e = m_key.data() ;

	memset( e,'\0',m_key.size() ) ;

	QCoreApplication::exit( st ) ;
}

QString MainWindow::fullApplicationPath()
{
	QString exe = QString( "/usr/local/bin/" ) + m_appName ;
	if( QFile::exists( exe ) ){
		return exe ;
	}
	exe = QString( "/usr/bin/" ) + m_appName ;
	if( QFile::exists( exe ) ){
		return exe ;
	}
	exe = QString( "/usr/sbin/" ) + m_appName ;
	if( QFile::exists( exe ) ){
		return exe ;
	}
	return QString() ;
}

void MainWindow::startingToreadData()
{
}

void MainWindow::bytesRead( int bytes )
{
	QString msg = tr( "number of bytes read from gpg keyfile: " ) + QString::number( bytes ) ;
	this->setWindowTitle( msg ) ;
}

void MainWindow::doneReading( bool cancelled )
{
	if( cancelled ){
		return this->Exit( 1 ) ;
	}
	if( !m_key.isEmpty() ){
		this->hide() ;
		m_sendKey->sendKey( m_key ) ;
	}else{
		DialogMsg msg( this ) ;
		m_working = false ;
		msg.ShowUIOK( tr( "ERROR" ),tr("could not decrypt the %1 keyfile,wrong key?" ).arg( m_appName ) ) ;
		this->enableAlll() ;
		m_ui->lineEditKey->setFocus() ;
		this->setWindowTitle( tr( "%1 key module" ).arg( m_appName ) ) ;
	}
}

void MainWindow::pbOpen()
{
	QString path = m_ui->lineEditKeyFile->text() ;

	DialogMsg msg( this ) ;

	if( path.isEmpty() ){
		return msg.ShowUIOK( tr( "ERROR" ),tr( "path to %1 keyfile is empty" ).arg( m_appName ) ) ;
	}
	if( !QFile::exists( path ) ){
		return msg.ShowUIOK( tr( "ERROR" ),tr( "invalid path to %1 keyfile" ).arg( m_appName ) ) ;
	}

	QString exe = this->fullApplicationPath() ;

	if( exe.isEmpty() ){
		return msg.ShowUIOK( tr( "ERROR" ),tr( "could not find \"%1\" executable in \"/usr/local\",\"/usr/bin\" and \"/usr/sbin\"" ).arg( m_appName ) ) ;
	}

	this->disableAll() ;
	m_working = true ;

	m_key = m_ui->lineEditKey->text().toLatin1() ;
	getKey * k = new getKey( exe,&m_key,m_ui->lineEditKeyFile->text() ) ;
	k->setKeyRoutine( m_function ) ;
	connect( this,SIGNAL( cancel() ),k,SLOT( cancel() ) ) ;
	connect( k,SIGNAL( bytesRead( int ) ),this,SLOT( bytesRead( int ) ) ) ;
	connect( k,SIGNAL( doneReadingKey( bool ) ),this,SLOT( doneReading( bool ) ) ) ;
	k->start() ;
}

void MainWindow::doneWritingData()
{
	m_working = false ;
	this->Exit( 0 ) ;
}

void MainWindow::pbKeyFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "select a key file" ),QDir::homePath() ) ;

	if( !Z.isEmpty() ){
		m_ui->lineEditKeyFile->setText( Z ) ;
	}
	this->SetFocus() ;
}

void MainWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}

void MainWindow::disableAll()
{
	m_ui->label->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->lineEditKey->setEnabled( false ) ;
	m_ui->lineEditKeyFile->setEnabled( false ) ;
	m_ui->pbKeyFile->setEnabled( false ) ;
	m_ui->pbOpen->setEnabled( false ) ;
	m_ui->pbCancel->setEnabled( false ) ;
}

void MainWindow::enableAlll()
{
	m_ui->label->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->lineEditKey->setEnabled( true ) ;
	m_ui->lineEditKeyFile->setEnabled( true ) ;
	m_ui->pbKeyFile->setEnabled( true ) ;
	m_ui->pbOpen->setEnabled( true ) ;
	m_ui->pbCancel->setEnabled( true ) ;
}

MainWindow::~MainWindow()
{
	delete m_ui ;
}

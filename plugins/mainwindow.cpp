/*
 *
 *  Copyright (c) 2012
 *  name : Francis Banyikwa
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

#include "../zuluCrypt-gui/utility.h"

#include "../zuluCrypt-cli/pluginManager/libzuluCryptPluginManager.h"

#include "../zuluCrypt-gui/lxqt_wallet/frontend/task.h"

namespace Task = LxQt::Wallet::Task ;

MainWindow::MainWindow( QWidget * parent ) : QWidget( parent ),m_ui( new Ui::MainWindow ),m_handle( nullptr )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;

	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;

	this->setWindowIcon( QIcon( ":/default.png" ) ) ;
	//m_ui->pbKeyFile->setIcon( QIcon( QString( ":/default.png" ) ) );

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;

	m_ui->lineEditKey->setFocus() ;

	m_working = false ;

	m_requireKey = false ;
	m_requireKeyFile = true ;

	QAction * ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter ) ;
	keys.append( Qt::Key_Return ) ;
	ac->setShortcuts( keys ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( defaultButton() ) ) ;
	this->addAction( ac ) ;

	m_findExecutable = []( QVector<QString>& exe ){

		if( exe.isEmpty() ){
			return QString() ;
		}

		QString e ;

		for( auto& it : exe ){
			auto _not_found = [&]( const char * path ){
				e = path + it ;
				bool r = QFile::exists( e ) ;
				if( r ){
					it = e ;
				}
				return r == false ;
			} ;

			if( _not_found( "/usr/local/bin/" ) ){
				if( _not_found( "/usr/bin/" ) ){
					if( _not_found( "/usr/sbin/" ) ){
						return it ;
					}
				}
			}
		}
		return QString() ;
	} ;
}

void MainWindow::Show()
{
	if( m_appName.endsWith( " Key" ) ){
		this->setWindowTitle( tr( "%1 Module" ).arg( m_appName ) ) ;
	}else{
		this->setWindowTitle( tr( "%1 Key Module" ).arg( m_appName ) ) ;
	}

	this->show() ;
}

void MainWindow::setButtonIcon( const QString& icon )
{
	QString x( ":/" + icon ) ;
	this->setWindowIcon( QIcon( x ) ) ;
	m_ui->pbKeyFile->setIcon( QIcon( x ) ) ;
}

void MainWindow::setRequireKey( bool k )
{
	m_requireKey = k ;
}

void MainWindow::setRequireKeyFile( bool k )
{
	m_requireKeyFile = k ;
}

void MainWindow::defaultButton()
{
	if( m_ui->pbCancel->hasFocus() ){
		this->pbCancel() ;
	}else{
		this->pbOpen() ;
	}
}

void MainWindow::setToken( const QString& token )
{
	m_handle = zuluCryptPluginManagerOpenConnection( token.toLatin1().constData() ) ;
}

void MainWindow::setApplicationName( const QString& appName )
{
	m_appName = appName ;
}

void MainWindow::setkeyLabel( const QString& keyLabel )
{
	m_ui->label_2->setText( keyLabel ) ;
}

void MainWindow::setkeyFileLabel( const QString& keyFileLabel )
{
	m_ui->label->setText( keyFileLabel ) ;
}

void MainWindow::setKeyFunction( function_t function )
{
	m_function = function ;
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
		int st = msg.ShowUIYesNoDefaultNo( tr( "WARNING"),
						   tr( "Are you sure you want to terminate this operation prematurely?" ) ) ;

		if( st == QMessageBox::Yes ){
			this->enableAlll() ;
			m_working = false ;
			this->cancelled() ;
		}
	}else{
		this->cancelled() ;
	}
}

void MainWindow::cancelled()
{
	this->Exit( 1 ) ;
}

void MainWindow::Exit( int st )
{
	QCoreApplication::exit( st ) ;
}

void MainWindow::setfindExeFunction( std::function<const QString&( QVector<QString>& )> f )
{
	m_findExecutable = f ;
}

void MainWindow::setExe( const QVector<QString>& exe )
{
	m_exe = exe ;
}

void MainWindow::setKeyFileAsKey( void )
{
	m_keyfileAsKey = true ;
	m_ui->lineEditKeyFile->setEchoMode( QLineEdit::Password ) ;
	m_ui->pbKeyFile->setVisible( false ) ;
}

void MainWindow::pbOpen()
{
	DialogMsg msg( this ) ;

	QString key = m_ui->lineEditKey->text().toLatin1() ;
	if( m_requireKey ){
		if( key.isEmpty() ){
			return msg.ShowUIOK( tr( "ERROR" ),tr( "Key field is empty" ) ) ;
		}
	}

	QString keyFile = m_ui->lineEditKeyFile->text() ;

	keyFile.replace( "file://","" ) ;

	if( m_requireKeyFile ){
		if( m_keyfileAsKey ){
			;
		}else{
			if( keyFile.isEmpty() ){
				return msg.ShowUIOK( tr( "ERROR" ),tr( "Path to %1 keyfile is empty" ).arg( m_appName ) ) ;
			}
			if( !QFile::exists( keyFile ) ){
				return msg.ShowUIOK( tr( "ERROR" ),tr( "Invalid path to %1 keyfile" ).arg( m_appName ) ) ;
			}
		}
	}

	m_exe_1 = m_exe ;
	QString e = m_findExecutable( m_exe_1 ) ;
	if( !e.isEmpty() ){
		return msg.ShowUIOK( tr( "ERROR" ),
				     tr( "Could not find \"%1\" executable in \"/usr/local\",\"/usr/bin\" and \"/usr/sbin\"" ).arg( e ) ) ;
	}

	this->disableAll() ;
	m_working = true ;

	Task::run< bool >( [ &,keyFile,key ](){

		QByteArray s = m_function( m_exe_1,keyFile,key ) ;

		if( s.isEmpty() ){

			return false ;
		}else{
			if( m_handle ){

				zuluCryptPluginManagerSendKey( m_handle,s.constData(),s.size() ) ;

				return true ;
			}else{
				return false ;
			}
		}

	} ).then( [ this ]( bool passed ){

		if( passed ){
			this->Exit( 0 ) ;
		}else{
			DialogMsg msg( this ) ;
			m_working = false ;

			if( m_appName.endsWith( " key" ) ){
				msg.ShowUIOK( tr( "ERROR" ),tr("Could not decrypt the %1,wrong key?" ).arg( m_appName ) ) ;
			}else{
				msg.ShowUIOK( tr( "ERROR" ),tr("Could not decrypt the %1 key,wrong key?" ).arg( m_appName ) ) ;
			}

			this->enableAlll() ;
			m_ui->lineEditKey->setFocus() ;
		}
	} ) ;
}

void MainWindow::pbKeyFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "Select A Keyfile" ),utility::homePath() ) ;

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
	zuluCryptPluginManagerCloseConnection( m_handle ) ;
	delete m_ui ;
}

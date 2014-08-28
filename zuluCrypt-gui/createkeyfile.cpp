/*
 *
 *  Copyright ( c ) 2011
 *  name : mhogo mchungu
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

#include "createkeyfile.h"
#include "ui_createkeyfile.h"
#include "utility.h"
#include "../zuluCrypt-cli/constants.h"

#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <iostream>
#include <QFile>
#include <QKeyEvent>

#include <QFile>

#include "filetask.h"
#include "utility.h"
#include "dialogmsg.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

createkeyfile::createkeyfile( QWidget * parent ) :
    QDialog( parent ),
    m_ui( new Ui::createkeyfile )
{
	m_ui->setupUi( this ) ;
	this->setFont( parent->font() ) ;

	m_ui->pbOpenFolder->setIcon( QIcon( QString( ":/folder.png" ) ) ) ;
	connect( m_ui->pbCreate,SIGNAL( clicked() ),this,SLOT( pbCreate() ) ) ;
	connect( m_ui->pbOpenFolder,SIGNAL( clicked() ),this,SLOT( pbOpenFolder() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->lineEditFileName,SIGNAL( textChanged( QString ) ),this,SLOT( keyTextChange( QString ) ) ) ;

	this->installEventFilter( this ) ;
}

bool createkeyfile::eventFilter( QObject * watched,QEvent * event )
{
	if( utility::eventFilter( this,watched,event ) ){
		this->HideUI() ;
		return true ;
	}else{
		return false ;
	}
}

void createkeyfile::keyTextChange( QString txt )
{
	QString p = m_ui->lineEditPath->text() ;

	if( p.isEmpty() ){
		QString x = QDir::homePath() + QString( "/" ) + txt.split( "/" ).last() ;
		m_ui->lineEditPath->setText( x ) ;
	}else{
		int i = p.lastIndexOf( "/" ) ;
		if( i != -1 ){
			p = p.mid( 0,i ) + QString( "/" ) + txt.split( "/" ).last() ;
			m_ui->lineEditPath->setText( p ) ;
		}
	}
}

void createkeyfile::HideUI()
{
	this->hide() ;
	emit HideUISignal() ;
}

void createkeyfile::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}

void createkeyfile::ShowUI()
{
	m_ui->lineEditFileName->clear() ;
	m_ui->lineEditPath->setText( QDir::homePath() + QString( "/" ) ) ;
	m_ui->comboBoxRNG->setCurrentIndex( 0 ) ;
	this->show() ;
}

void createkeyfile::pbCancel()
{
	if( m_running ){
		m_stop = true ;
	}else{
		this->HideUI() ;
	}
}

void createkeyfile::enableAll()
{
	m_ui->label->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->lineEditFileName->setEnabled( true ) ;
	m_ui->lineEditPath->setEnabled( true ) ;
	m_ui->pbCreate->setEnabled( true ) ;
	m_ui->pbOpenFolder->setEnabled( true ) ;
	m_ui->labelRNG->setEnabled( true ) ;
	m_ui->comboBoxRNG->setEnabled( true ) ;
}

void createkeyfile::disableAll()
{
	m_ui->label->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->lineEditFileName->setEnabled( false ) ;
	m_ui->lineEditPath->setEnabled( false ) ;
	m_ui->pbCreate->setEnabled( false ) ;
	m_ui->pbOpenFolder->setEnabled( false ) ;
	m_ui->labelRNG->setEnabled( false ) ;
	m_ui->comboBoxRNG->setEnabled( false ) ;
}

void createkeyfile::pbCreate()
{
	DialogMsg msg( this ) ;

	QString fileName = m_ui->lineEditFileName->text() ;
	QString path = m_ui->lineEditPath->text() ;

	if( fileName.isEmpty() ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "the key name field is empth" ) ) ;
	}
	if( path.isEmpty() ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "folder path to where the key will be created is empty" ) ) ;
	}
	if( utility::pathExists( path ) ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "file with the same name and at the destination folder already exist" ) ) ;
	}
	if( !utility::canCreateFile( path ) ){
		msg.ShowUIOK( tr( "ERROR!" ),tr( "you dont seem to have writing access to the destination folder" ) ) ;
		m_ui->lineEditPath->setFocus() ;
		return ;
	}

	this->disableAll() ;

	int rng = m_ui->comboBoxRNG->currentIndex() ;

	m_stop = false ;
	m_running = true ;

	Task::await( [ & ](){

		char data ;
		int r ;
		int e ;

		if( rng == 0 ){
			r = ::open( "/dev/urandom",O_RDONLY ) ;
		}else{
			r = ::open( "/dev/random",O_RDONLY ) ;
		}

		if( r == -1 ){
			m_stop = true ;
			return ;
		}

		QByteArray z = path.toLatin1() ;
		const char * p = z.constData() ;

		e = ::open( p,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) ;

		if( e == -1 ){
			::close( r ) ;
			m_stop = true ;
			return ;
		}

		for( int i = 0 ; i < 64 ; i++ ){

			if( m_stop ){
				break ;
			}else{
				do{
					::read( r,&data,1 ) ;
				}while( data < 32 || data > 126 ) ;

				::write( e,&data,1 ) ;
			}
		}

		::close( e ) ;
		::close( r ) ;
	} ) ;

	m_running = false ;

	if( m_stop ){
		msg.ShowUIOK( tr( "WARNING!" ),tr( "process interrupted,key not fully generated" ) ) ;
		this->enableAll() ;
	}else{
		msg.ShowUIOK( tr( "SUCCESS!" ),tr( "key file successfully created" ) ) ;
		this->HideUI() ;
	}
}

void createkeyfile::pbOpenFolder()
{
	QString p = tr( "Select a folder to create a key file in" ) ;
	QString q = QDir::homePath() ;
	QString Z = QFileDialog::getExistingDirectory( this,p,q,QFileDialog::ShowDirsOnly ) ;

	if( !Z.isEmpty() ){
		Z = Z + QString( "/" ) + m_ui->lineEditPath->text().split( "/" ).last() ;
		m_ui->lineEditPath->setText( Z ) ;
	}
}

createkeyfile::~createkeyfile()
{
	delete m_ui ;
}

/*
 *
 *  Copyright ( c ) 2015
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


#include "hmac.h"
#include "task.h"

#include "../plugins/hmac/hmac_key.h"

#include "ui_hmac.h"

#include "utility.h"
#include "dialogmsg.h"

#include <QCloseEvent>
#include <QEvent>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QIcon>

#include <memory>

hmac::hmac( QDialog * parent,std::function< void( const QString& ) > function,const QString& e ) :
	QDialog( parent ),m_ui( new Ui::hmac ),m_function( std::move( function ) )
{
	m_ui->setupUi( this ) ;

	if( !e.isEmpty() ){

		m_ui->label->setText( e ) ;
	}

	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	connect( m_ui->pbSetKey,SIGNAL( clicked() ),this,SLOT( pbSetKey() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbClose() ) ) ;
	connect( m_ui->pbKeyFile,SIGNAL( clicked() ),this,SLOT( pbSelectKeyFile() ) ) ;

	m_ui->pbKeyFile->setIcon( QIcon( ":/file.png" ) ) ;

	this->ShowUI() ;
}

bool hmac::eventFilter( QObject * watched,QEvent * event )
{
	if( utility::eventFilter( this,watched,event ) ){

		this->HideUI() ;
		return true ;
	}else{
		return false ;
	}
}

void hmac::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

hmac::~hmac()
{
	delete m_ui ;
}

void hmac::ShowUI()
{
	this->show() ;
}

void hmac::HideUI()
{
	m_function( m_key ) ;
	this->hide() ;
	this->deleteLater() ;
}

void hmac::pbSetKey()
{
	m_passphrase = m_ui->lineEdit->text() ;

	DialogMsg msg( this ) ;

	if( m_passphrase.isEmpty() ){

		return msg.ShowUIOK( tr( "ERROR" ),tr( "Passphrase Not Set" ) ) ;
	}

	if( m_keyFile.isEmpty() ){

		return msg.ShowUIOK( tr( "ERROR" ),tr( "KeyFile Not Set" ) ) ;
	}

	this->disableAll() ;

	Task::run< QByteArray >( [ this ](){

		return hmac_key_0( m_keyFile,m_passphrase ) ;

	} ).then( [ this ]( const QByteArray& e ){

		m_key = e ;

		if( m_key.isEmpty() ){

			DialogMsg msg( this ) ;

			msg.ShowUIOK( tr( "ERROR" ),tr( "Failed To Generate Key" ) ) ;

			this->enableAll() ;
		}else{
			this->HideUI() ;
		}
	} ) ;
}

void hmac::pbSelectKeyFile()
{
	m_keyFile = QFileDialog::getOpenFileName( this,tr( "Key File" ),utility::homePath(),0 ) ;
}

void hmac::pbClose()
{
	this->HideUI() ;
}

void hmac::enableAll()
{
	m_ui->pbCancel->setEnabled( true ) ;
	m_ui->pbKeyFile->setEnabled( true ) ;
	m_ui->pbSetKey->setEnabled( true ) ;
	m_ui->groupBox->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->lineEdit->setEnabled( true ) ;
}

void hmac::disableAll()
{
	m_ui->pbCancel->setEnabled( false ) ;
	m_ui->pbKeyFile->setEnabled( false ) ;
	m_ui->pbSetKey->setEnabled( false ) ;
	m_ui->groupBox->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->lineEdit->setEnabled( false ) ;
}

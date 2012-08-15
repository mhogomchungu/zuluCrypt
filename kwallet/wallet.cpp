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

#include "wallet.h"
#include <iostream>
#include <QFile>

wallet::wallet( int argc,char * argv[] )
{
	m_argc = argc ;
	m_device = QString( argv[ 1 ] ) ;
	m_uuid = QString( argv[ 2 ] ) ;
	m_sockpath = QString( argv[ 3 ] ) ;
	m_bufferSize = QString( argv[ 4 ] ).toInt() ;
}

void wallet::openWallet()
{
	this->openConnection();

	m_wallet = Wallet::openWallet( "zuluCrypt",0,KWallet::Wallet::Asynchronous ) ;

	if( m_wallet ){
		connect( m_wallet,SIGNAL( walletOpened( bool ) ),this,SLOT( walletOpened( bool ) ) ) ;
	}else{
		qDebug() << "failed to open wallet";
		this->Exit( 1 );
	}
}

void wallet::gotConnected()
{
	m_socket = m_server->nextPendingConnection() ;
}

void wallet::walletOpened( bool e )
{
	m_walletOpened = e ;
	this->sendKey();
}

void wallet::openConnection()
{
	m_server = new QLocalServer( this ) ;
	connect( m_server,SIGNAL( newConnection() ),this,SLOT( gotConnected() ) ) ;
	m_server->listen( m_sockpath ) ;
}

void wallet::sendKey()
{
	this->getKey();

	if( m_key.isEmpty() )
		this->Exit( 1 );
	else{
		m_socket->write( m_key.toAscii() ) ;
		m_socket->flush() ;
		this->Exit( 0 );
	}
}

void wallet::getKey()
{
	if( !m_walletOpened ){
		//qDebug() << "openWallet() failed ";
		m_key.clear();
		return ;
	}

	if( !m_wallet->setFolder( "Form Data" ) ){
		//qDebug() << "setFolder() failed ";
		m_key.clear();
		return ;
	}

	QMap <QString,QString> map ;

	if( m_wallet->readMap( "LUKS",map ) ){
		//qDebug() << "readMap() failed";
		m_key.clear();
		return;
	}

	if( m_uuid == QString( "Nil" ) ){
		//qDebug() << "device has no UUID";
		m_key.clear();
		return  ;
	}

	m_key = map.value( m_uuid ) ;

	if( m_key.isEmpty() ){
		m_uuid.remove( QChar( '\"' ) ) ;
		m_key = map.value( m_uuid ) ;
	}
}

void wallet::Exit( int st )
{
	if( m_socket->isOpen() )
		m_socket->close();
	m_socket->deleteLater();

	m_server->close();
	m_server->deleteLater();

	QFile::remove( m_sockpath ) ;

	QCoreApplication::exit( st ) ;
}

wallet::~wallet()
{
	m_wallet->deleteLater();
}

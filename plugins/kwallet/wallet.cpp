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

wallet::wallet( QString uuid,QString sockAddr )
{
	m_uuid = QString( "UUID=\"%1\"" ).arg( uuid ) ;
	m_sockAddr = sockAddr ;
}

void wallet::start()
{
	m_zuluSocket = new zuluSocket( this ) ;
	connect( m_zuluSocket,SIGNAL( gotConnected() ),this,SLOT( openWallet() ) ) ;
	m_zuluSocket->startServer( m_sockAddr );
}

void wallet::openWallet()
{
	m_wallet = Wallet::openWallet( zuluOptions::wallet(),0,KWallet::Wallet::Synchronous ) ;

	if( m_wallet )
		this->readKwallet();
	else
		this->Exit( 1 );
}

void wallet::SendKey()
{
	m_zuluSocket->sendData( &m_key );
	this->Exit( 0 );
}

void wallet::readKwallet()
{
	QString formData = zuluOptions::formData() ;
	if( !m_wallet->hasFolder( formData ) )
		m_wallet->createFolder( formData ) ;

	m_wallet->setFolder( formData ) ;

	QMap <QString,QString> map ;

	m_wallet->readMap( zuluOptions::key(),map ) ;

	m_key = map.value( m_uuid ).toAscii() ;
	if( m_key.isEmpty() )
		return this->Exit( 4 );
	else
		this->SendKey();
}

void wallet::Exit( int st )
{
	QCoreApplication::exit( st ) ;
}

wallet::~wallet()
{
	m_zuluSocket->deleteLater();
	m_wallet->deleteLater();
}

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
#include <QDebug>

wallet::wallet( QString uuid,QString sockAddr )
{
	m_uuid = QString( "UUID=\"%1\"" ).arg( uuid ) ;
	m_sockAddr = sockAddr ;
	m_handle = socketSendKey::zuluCryptPluginManagerOpenConnection( m_sockAddr ) ;
}

void wallet::openWallet()
{
	m_wallet = Wallet::openWallet( kwalletplugin::wallet(),0,KWallet::Wallet::Synchronous ) ;

	if( m_wallet ){
		this->readKwallet();
	}else{
		QCoreApplication::exit( 1 ) ;
	}
}

void wallet::SendKey()
{
	socketSendKey::zuluCryptPluginManagerSendKey( m_handle,m_key ) ;
	this->Exit();
}

void wallet::readKwallet()
{
	QString formData = kwalletplugin::formData() ;

	m_wallet->setFolder( formData ) ;

	QMap <QString,QString> map ;

	m_wallet->readMap( kwalletplugin::key(),map ) ;

	m_key = map.value( m_uuid ).toAscii() ;

	this->SendKey();
}

void wallet::Exit( void )
{
	QCoreApplication::exit( 0 ) ;
}

wallet::~wallet()
{
	socketSendKey::zuluCryptPluginManagerCloseConnection( m_handle ) ;
	if( m_wallet ){
		delete m_wallet ;
	}
}

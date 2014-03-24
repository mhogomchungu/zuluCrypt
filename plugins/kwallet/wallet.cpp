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
#include "../../zuluCrypt-gui/utility.h"

wallet::wallet( const QString& path,const QString& uuid,const QString& sockAddr )
{
	if( uuid == QString( "Nil" ) ){
		m_keyID = path ;
	}else{
		m_keyID = QString( "UUID=\"%1\"" ).arg( uuid ) ;
	}

	m_sockAddr = sockAddr ;
	m_handle = socketSendKey::zuluCryptPluginManagerOpenConnection( m_sockAddr ) ;
}

void wallet::openWallet()
{
	m_wallet = Wallet::openWallet( KWallet::Wallet::LocalWallet(),0,KWallet::Wallet::Synchronous ) ;

	if( m_wallet ){
		m_wallet->setFolder( utility::applicationName() ) ;
		QString key ;
		m_wallet->readPassword( m_keyID,key ) ;
		if( key.isEmpty() && m_keyID.startsWith( QString( "UUID=" ) ) ){
			m_wallet->readPassword( m_keyID.replace( "\"","" ),key ) ;
		}
		if( key.isEmpty() ){
			QCoreApplication::exit( 1 ) ;
		}else{
			socketSendKey::zuluCryptPluginManagerSendKey( m_handle,key.toLatin1() ) ;
			QCoreApplication::exit( 0 ) ;
		}
	}else{
		QCoreApplication::exit( 1 ) ;
	}
}

wallet::~wallet()
{
	socketSendKey::zuluCryptPluginManagerCloseConnection( m_handle ) ;
	delete m_wallet ;

}

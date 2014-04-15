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

#include "getKey.h"

#include <sys/types.h>
#include <signal.h>
#include <QDebug>

#include "../../zuluCrypt-cli/pluginManager/libzuluCryptPluginManager.h"

getKey::getKey( const QString& token )
{
	m_token = token ;
}

void getKey::setOptions( const QVector<QString>& exe,const QString& key,const QString& keyFile,function_t function )
{
	m_function = function ;
	m_exe = exe ;
	m_key = key ;
	m_keyFile = keyFile ;
}

void getKey::start()
{
	QThreadPool::globalInstance()->start( this ) ;
}

void getKey::run()
{
	QByteArray key = m_function( m_exe,m_keyFile,m_key ) ;

	if( key.isEmpty() ){
		m_status = getKey::wrongKey ;
	}else{
		void * handle = zuluCryptPluginManagerOpenConnection( m_token.toLatin1().constData() ) ;

		if( handle ){

			zuluCryptPluginManagerSendKey( handle,key.constData(),key.size() ) ;
			zuluCryptPluginManagerCloseConnection( handle ) ;

			m_status = getKey::complete ;
		}else{
			m_status = getKey::wrongKey ;
		}
	}
}

void getKey::cancel( const QString& token )
{
	void * handle = zuluCryptPluginManagerOpenConnection( token.toLatin1().constData() ) ;
	if( handle ){
		zuluCryptPluginManagerCloseConnection( handle ) ;
	}
}

getKey::~getKey()
{
	emit done( int( m_status ) ) ;
}

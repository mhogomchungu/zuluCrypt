/*
 *  Copyright ( c ) 2012
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

#include "socketsendkey.h"
#include <QDebug>
socketSendKey::socketSendKey( QObject * parent,QString sockpath,QByteArray key )
{
	Q_UNUSED( parent ) ;
	m_connectionHandle = 0 ;
	//this->setParent( parent );
	m_sockpath = sockpath ;
	m_key = key ;
	connect( this,SIGNAL( finished() ),this,SLOT( deleteLater() ) ) ;
}

void socketSendKey::sendKey( void )
{
	this->start();
}

void socketSendKey::run()
{
	m_connectionHandle = ::zuluCryptPluginManagerOpenConnection( m_sockpath.toAscii().constData() ) ;

	if( !m_connectionHandle ){
		//qDebug() << "failed to connect";
		emit keyNotSent();
	}else{
		//sleep( 1 ) ;
		//qDebug() << "got coonnected";

		emit gotConnected() ;

		::zuluCryptPluginManagerSendKey( m_connectionHandle,m_key.constData(),m_key.size() ) ;

		::zuluCryptPluginManagerCloseConnection( m_connectionHandle );

		m_connectionHandle = 0 ;
		//qDebug() << "connection closed";

		emit keySent() ;
	}
}

void * socketSendKey::zuluCryptPluginManagerOpenConnection( QString sockpath )
{
	return ::zuluCryptPluginManagerOpenConnection( sockpath.toAscii().constData() ) ;
}

ssize_t socketSendKey::zuluCryptPluginManagerSendKey( void * handle,QByteArray data )
{
	return ::zuluCryptPluginManagerSendKey( handle,data.constData(),data.size() ) ;
}

void socketSendKey::zuluCryptPluginManagerCloseConnection( void * handle )
{
	return ::zuluCryptPluginManagerCloseConnection( handle ) ;
}

socketSendKey::~socketSendKey()
{
	//if( m_connectionHandle )
	//	zuluCryptPluginManagerCloseConnection( m_connectionHandle );

	//qDebug() << "~socketSendKey()";
}

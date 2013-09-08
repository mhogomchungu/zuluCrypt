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

#include <QObject>
#include <QTime>
#include <QDir>

socketSendKey::socketSendKey( QObject * parent,QString sockpath,QByteArray key )
{
	Q_UNUSED( parent ) ;
	m_connectionHandle = 0 ;
	m_sockpath = sockpath ;
	m_key = key ;
	m_connected = false ;
	connect( this,SIGNAL( finished() ),this,SLOT( deleteLater() ) ) ;
	QDir d ;
	d.mkdir( QDir::homePath() + QString( "/.zuluCrypt-socket/" ) ) ;
}

socketSendKey::socketSendKey( QObject * parent )
{
	Q_UNUSED( parent ) ;
}

void socketSendKey::setAddr( QString addr )
{
	m_sockpath = addr ;
}

void socketSendKey::closeConnection()
{
	m_closeConnection = false ;
	::zuluCryptPluginManagerCloseConnection( m_connectionHandle ) ;
}

void socketSendKey::sendKey( QByteArray data )
{
	m_key = data ;
	this->start() ;
}

bool socketSendKey::openConnection()
{
	QByteArray sockpath = m_sockpath.toAscii() ;
	m_connectionHandle = ::zuluCryptPluginManagerOpenConnection( sockpath.constData() ) ;
	if( m_connectionHandle ){
		m_closeConnection = true ;
		m_connected = true ;
	}else{
		m_connected = false ;
	}

	return m_connected ;
}

void socketSendKey::setKey( QByteArray key )
{
	m_key = key ;
}

void socketSendKey::sendKey( void )
{
	this->start() ;
}

QString socketSendKey::getSocketPath()
{
	u_int64_t x = static_cast<u_int64_t>( QDateTime::currentDateTime().toMSecsSinceEpoch() ) ;
	return QString( QDir::homePath() + QString( "/.zuluCrypt-socket/" ) + QString::number( x ) ) ;
}

void socketSendKey::run()
{
	if( !m_connected ){
		this->openConnection() ;
	}
	if( !m_connected ){
		emit keyNotSent() ;
	}else{
		emit gotConnected() ;
		::zuluCryptPluginManagerSendKey( m_connectionHandle,m_key.constData(),m_key.size() ) ;
		m_closeConnection = false ;
		::zuluCryptPluginManagerCloseConnection( m_connectionHandle ) ;
		emit keySent() ;
	}
}

void socketSendKey::openAndCloseConnection( QString sockAddr )
{
	QByteArray sockpath = sockAddr.toAscii() ;
	void * connection = ::zuluCryptPluginManagerOpenConnection( sockpath.constData() ) ;
	::zuluCryptPluginManagerCloseConnection( connection ) ;
}

void * socketSendKey::zuluCryptPluginManagerOpenConnection( QString sockpath )
{
	QByteArray path = sockpath.toAscii() ;
	return ::zuluCryptPluginManagerOpenConnection( path.constData() ) ;
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
	if( m_closeConnection ){
		::zuluCryptPluginManagerCloseConnection( m_connectionHandle ) ;
	}
}

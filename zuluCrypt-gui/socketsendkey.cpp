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
#include "utility.h"
#include "../zuluCrypt-cli/constants.h"

#include <QDebug>
#include <QObject>
#include <QTime>
#include <QDir>
#include <QFile>

socketSendKey::socketSendKey( QObject * parent,const QString& sockpath,const QByteArray& key )
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

void socketSendKey::setAddr( const QString& addr )
{
	m_sockpath = addr ;
}

void socketSendKey::closeConnection()
{
	m_closeConnection = false ;
	::zuluCryptPluginManagerCloseConnection( m_connectionHandle ) ;
	m_connectionHandle = 0 ;
	m_closeConnection = 0 ;
}

void socketSendKey::sendKey( const QByteArray& data )
{
	m_key = data ;
	this->start() ;
}

bool socketSendKey::openConnection()
{
	QByteArray sockpath = m_sockpath.toLatin1() ;
	m_connectionHandle = ::zuluCryptPluginManagerOpenConnection( sockpath.constData() ) ;
	if( m_connectionHandle ){
		m_closeConnection = true ;
		m_connected = true ;
	}else{
		m_connected = false ;
	}

	return m_connected ;
}

void socketSendKey::setKey( const QByteArray& key )
{
	m_key = key ;
}

void socketSendKey::sendKey( void )
{
	this->start() ;
}

QString socketSendKey::getSocketPath()
{
	QFile f( "/dev/urandom" ) ;
	f.open( QIODevice::ReadOnly ) ;
	QByteArray data = f.read( 64 ) ;
	return QString( "%1%2%3" ).arg( QDir::homePath() ).arg( QString( "/.zuluCrypt-socket/" ) ).arg( utility::hashPath( data ).mid( 1 ) ) ;
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
		size_t size = m_key.size() ;
		/*
		 * ZULUCRYPT_KEYFILE_MAX_SIZE is defined in ../zuluCrypt-cli/constants.h
		 * The variable holds the maximum keyfile size
		 */
		if( size > ZULUCRYPT_KEYFILE_MAX_SIZE ){
			size = ZULUCRYPT_KEYFILE_MAX_SIZE ;
		}
		::zuluCryptPluginManagerSendKey( m_connectionHandle,m_key.constData(),size ) ;
		m_closeConnection = false ;
		::zuluCryptPluginManagerCloseConnection( m_connectionHandle ) ;
		emit keySent() ;
	}
}

void socketSendKey::openAndCloseConnection( const QString& sockAddr )
{
	QByteArray sockpath = sockAddr.toLatin1() ;
	void * connection = ::zuluCryptPluginManagerOpenConnection( sockpath.constData() ) ;
	::zuluCryptPluginManagerCloseConnection( connection ) ;
}

void * socketSendKey::zuluCryptPluginManagerOpenConnection( const QString& sockpath )
{
	QByteArray path = sockpath.toLatin1() ;
	return ::zuluCryptPluginManagerOpenConnection( path.constData() ) ;
}

ssize_t socketSendKey::zuluCryptPluginManagerSendKey( void * handle,const QByteArray& data )
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

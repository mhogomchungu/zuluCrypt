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

socketSendKey::socketSendKey( QObject * parent,QString sockpath,QByteArray key )
{
	Q_UNUSED( parent ) ;
	m_connectionHandle = 0 ;
	m_sockpath = sockpath ;
	m_key = key ;
	connect( this,SIGNAL( finished() ),this,SLOT( deleteLater() ) ) ;
}

void socketSendKey::sendKey( void )
{
	this->start();
}

QString socketSendKey::getSocketPath()
{
	QTime T = QTime::currentTime() ;
	QString h  = QString::number( T.hour() )   ;
	QString m  = QString::number( T.minute() ) ;
	QString s  = QString::number( T.second() ) ;
	QString ms = QString::number( T.msec() )   ;
	return QString( QDir::homePath() + QString( "/.zuluCrypt-socket/" ) + h + m + s + ms  ) ;
}

void socketSendKey::run()
{
	QByteArray sockpath = m_sockpath.toAscii() ;
	m_connectionHandle = ::zuluCryptPluginManagerOpenConnection( sockpath.constData() ) ;

	if( !m_connectionHandle ){
		emit keyNotSent();
	}else{
		emit gotConnected() ;
		::zuluCryptPluginManagerSendKey( m_connectionHandle,m_key.constData(),m_key.size() ) ;
		::zuluCryptPluginManagerCloseConnection( m_connectionHandle );
		m_connectionHandle = 0 ;
		emit keySent() ;
	}
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
}

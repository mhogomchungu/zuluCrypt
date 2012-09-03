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

#include "zulusocket.h"
#include <QDebug>

/*
 * It seem to not be possible to delete the socket path from within zuluSocket class.
 * Creating this helper function and attach it to the whoever called zuluSocket and
 * have them delete the socket path when they themselves get deleted.
 */
class deleteSockPath :public QObject{
public:
	deleteSockPath( QObject * parent,QString path )
	{
		m_path = path ;
		this->setParent( parent );
	}
	void doNothing( void )
	{
		//call the function after creating the object to avoid
		// compiler warning
	}

	~deleteSockPath()
	{
		QFile::remove( m_path ) ;
	}
private:
	QString m_path ;
};

zuluSocket::zuluSocket( QObject * parent )
{
	m_server = 0 ;
	m_socket = 0 ;
	m_connected = false ;
	m_parent = parent ;
}

bool zuluSocket::isConnected()
{
	return m_connected ;
}

void zuluSocket::startServer( QString addr )
{
	m_sockAddr = addr ;
	QFile::remove( m_sockAddr ) ;
	m_server = new QLocalServer( this ) ;
	connect( m_server,SIGNAL( newConnection() ),this,SLOT( acceptConnection() ) ) ;
	m_server->listen( m_sockAddr ) ;
	deleteSockPath * dsp = new deleteSockPath( m_parent,m_sockAddr ) ;
	dsp->doNothing();
}

void zuluSocket::acceptConnection()
{
	m_socket = m_server->nextPendingConnection() ;
	connect( m_socket,SIGNAL( bytesWritten( qint64 ) ),this,SLOT( bytesWritten( qint64 ) ) ) ;
	m_connected = true ;
	emit gotConnected();
	emit gotConnected( this );
}

/*
 *QlocalSocket->send() does not seem to send all data at the same time if data is large enough.
 *Its return value says one thing and its byteWritten signal says another.
 *To work around this, sendAllData() and byteWritten() loop until all data is sent.
 */
void zuluSocket::sendData( QByteArray * data )
{
	m_data = data ;
	m_size = m_data->size() ;
	m_dataSent = 0 ;

	int chunk = 128 ;

	for( qint64 i = 0 ; i <= m_size ; i = i + chunk ){
		m_socket->write( m_data->mid( i,chunk ) ) ;
		m_socket->flush() ;
	}
}

void zuluSocket::bytesWritten( qint64 bytes )
{
	m_dataSent = m_dataSent + bytes ;
	if( m_dataSent >= m_size )
		this->done();
}

void zuluSocket::sendDataToSocket()
{
	m_socket->write( m_data->mid( m_dataSent ) ) ;
}

void zuluSocket::done()
{
	emit doneWritingData();
}

zuluSocket::~zuluSocket()
{
	if( m_socket ){
		if( m_socket->isOpen() )
			m_socket->close();
		m_socket->deleteLater(); ;
	}

	if( m_server ){
		m_server->close() ;
		m_server->deleteLater();
	}
}

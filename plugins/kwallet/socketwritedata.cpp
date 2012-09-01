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

#include "socketwritedata.h"
#include <QDebug>

socketWriteData::socketWriteData( QLocalSocket * socket,QByteArray * data )
{
	m_socket = socket ;
	m_data = data ;
	m_size = data->size() ;
	m_dataSent = 0 ;

	//connect( m_socket,SIGNAL( bytesWritten( qint64 ) ),this,SLOT( bytesWritten( qint64 ) ) ) ;
}

void socketWriteData::start()
{
	//qDebug() << "start";
	QThreadPool::globalInstance()->start( this );
	//this->sendData();
}

void socketWriteData::run()
{
	//qDebug() << "run";
	this->sendData();
}

/*
 *QlocalSocket->send() does not seem to send all data at the same time if data is large enough.
 *Its return value says one thing and its byteWritten signal says another.
 *To work around this, sendAllData() and byteWritten() loop until all data is sent.
 */
void socketWriteData::bytesWritten( qint64 bytes )
{
	m_dataSent = m_dataSent + bytes ;
	//qDebug() << "datawritten" << m_dataSent ;

	if( m_dataSent >= m_size )
		emit done() ;
	else
		this->sendData();
}

void socketWriteData::sendData()
{
	//qDebug() << "sending data";
	//m_socket->write( m_data->mid( m_dataSent ) ) ;
}



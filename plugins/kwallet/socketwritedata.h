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


#ifndef SOCKETWRITEDATA_H
#define SOCKETWRITEDATA_H

#include <QObject>
#include <QThreadPool>
#include <QRunnable>
#include <QByteArray>
#include <QtNetwork/QLocalSocket>


class socketWriteData : public QObject,public QRunnable
//class socketWriteData : public QObject
{
	Q_OBJECT
public:
	void start( void ) ;
	socketWriteData( QLocalSocket * socket,QByteArray * data ) ;
signals:
	void done( void ) ;
public slots:
	void bytesWritten( qint64 ) ;
private slots:
private:
	void sendData() ;
	void run( void ) ;
	QByteArray * m_data ;
	qint64 m_size ;
	qint64 m_dataSent ;
	QLocalSocket * m_socket ;
};

#endif // SOCKETWRITEDATA_H

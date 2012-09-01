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

#ifndef ZULUSOCKET_H
#define ZULUSOCKET_H

#include <QDebug>
#include <QObject>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QString>
#include <QByteArray>
#include <QFile>

//#include "socketwritedata.h"

class zuluSocket : public QObject
{
	Q_OBJECT
public:
	zuluSocket( QObject * );
	~zuluSocket() ;
	bool isConnected( void ) ;
	/* The socket address sockAddr ideally should be deleted by the class but it doesnt for some reason,
	 * caller of the class should manually delete it after the class is destructed.
	 */
	void startServer( QString sockAddr ) ;
signals:
	void gotConnected( void ) ;
	void doneWritingData( void ) ;
public slots:
	void sendData( QByteArray * ) ;
private slots:
	void bytesWritten( qint64 bytes ) ;
	void acceptConnection( void ) ;
private:
	void done( void ) ;
	void sendDataToSocket( void ) ;
	bool m_connected ;
	QString m_sockAddr ;
	QLocalSocket * m_socket ;
	QLocalServer * m_server ;
	//socketWriteData * m_sockWriteData ;
	qint64 m_size ;
	qint64 m_dataSent ;
	QByteArray * m_data ;
	QObject * m_parent ;
};

#endif // ZULUSOCKET_H

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

#ifndef SOCKETSENDKEY_H
#define SOCKETSENDKEY_H

#include <QThread>
#include <QString>
#include <QByteArray>

#include "../zuluCrypt-cli/pluginManager/libzuluCryptPluginManager.h"

class socketSendKey : public QThread
{
	Q_OBJECT
public:
	socketSendKey( QObject * parent = 0,const QString& sockpath = QString(),const QByteArray& key = QByteArray() ) ;
	~socketSendKey() ;
	bool openConnection( void ) ;
	void closeConnection( void ) ;
	void sendKey( void ) ;
	void sendKey( const QByteArray& ) ;
	void setAddr( const QString& ) ;
	void setKey( const QByteArray& ) ;
	static void openAndCloseConnection( const QString& ) ;
	static void * zuluCryptPluginManagerOpenConnection( const QString& ) ;
	static ssize_t zuluCryptPluginManagerSendKey( void * handle,const QByteArray& ) ;
	static void zuluCryptPluginManagerCloseConnection( void * handle ) ;
	static QString getSocketPath( void ) ;
signals:
	void gotConnected( void ) ;
	void keySent( void ) ;
	void sendingKey( void ) ;
	void keyNotSent( void ) ;
private:
	void run( void ) ;
	bool m_continue ;
	QString m_sockpath ;
	int m_count ;
	void * m_connectionHandle ;
	QByteArray m_key ;
	bool m_connected ;
	bool m_closeConnection ;
};

#endif // SOCKETSENDKEY_H

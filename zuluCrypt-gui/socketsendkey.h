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
#include <QObject>
#include <QByteArray>
#include <QByteArray>

#include "../zuluCrypt-cli/pluginManager/libzuluCryptPluginManager.h"

class socketSendKey : public QThread
{
	Q_OBJECT
public:
	socketSendKey( QObject * parent,QString sockpath,QByteArray key );
	~socketSendKey() ;
	void sendKey( void ) ;
	static void * zuluCryptPluginManagerOpenConnection( QString ) ;
	static ssize_t zuluCryptPluginManagerSendKey( void * handle,QByteArray ) ;
	static void zuluCryptPluginManagerCloseConnection( void * handle ) ;
signals:
	emit void gotConnected( void ) ;
	emit void keySent( void ) ;
	emit void sendingKey( void ) ;
	emit void keyNotSent( void ) ;
private:
	void run( void ) ;
	bool m_continue ;
	QString m_sockpath ;
	int m_count ;
	void * m_connectionHandle ;
	QByteArray m_key ;
};

#endif // SOCKETSENDKEY_H

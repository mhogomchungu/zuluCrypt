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

#ifndef GETGPGKEY_H
#define GETGPGKEY_H

#include <QThreadPool>
#include <QRunnable>
#include <QByteArray>
#include <QString>
#include <QVector>
#include <QProcess>
#include <QObject>
#include <unistd.h>
#include <functional>

typedef std::function<QByteArray( const QVector<QString>&,const QString& keyFile,const QString& password )> function_t ;

class getKey : public QObject,public QRunnable
{
	Q_OBJECT
public:
	typedef enum{
		cancelled,
		complete,
		wrongKey
	}status ;
	static void cancel( const QString& token ) ;
	getKey( const QString& token ) ;
	void setOptions( const QVector<QString>&,const QString& key,const QString& keyFile,function_t function ) ;
	~getKey() ;
	void start( void ) ;
signals:
	void done( int ) ;
private:
	void run( void ) ;
	QVector<QString> m_exe ;
	QString m_key ;
	QString m_keyFile ;
	QString m_token ;
	int m_status ;
	function_t m_function ;
};

#endif // GETGPGKEY_H

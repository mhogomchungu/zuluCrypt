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
#include <QProcess>
#include <QObject>
#include <unistd.h>

class getgpgkey :public QObject,public QRunnable
{
	Q_OBJECT
public:
	getgpgkey( const QString& gpgExe,QByteArray * key,const QString& keyFile ) ;
	~getgpgkey() ;
	void start( void ) ;
signals:
	void bytesRead( int ) ;
	void startingToReadData( void ) ;
	void doneReadingFromgpg( bool ) ;
public slots:
	void cancel( void ) ;
private:
	void run( void ) ;
	QString m_gpgExe ;
	QByteArray * m_key ;
	QString m_keyFile ;
	pid_t m_pid ;
	bool m_cancelled ;
	int m_type ;
};

#endif // GETGPGKEY_H

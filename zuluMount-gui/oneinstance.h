
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

#ifndef ONEINSTANCE_H
#define ONEINSTANCE_H

#include <Qt>
#include <QObject>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QFile>
#include <QCoreApplication>
#include <QMainWindow>
#include <QString>
#include <QMetaObject>
#include <QDir>

class oneinstance : public QObject
{
	Q_OBJECT
public:
	explicit oneinstance( QObject * parent,QString socketPath,const char *,QString ) ;
	~oneinstance() ;
	bool instanceExist( void ) ;
	void setDevice( QString ) ;
signals:
	void raise( void ) ;
	void raiseWithDevice( QString ) ;
public slots:
	void connected( void ) ;
	void gotConnection( void ) ;
private slots:
	void Exit( void ) ;
private:
	void startInstance( void ) ;
	void killProcess( void ) ;
	QLocalServer * m_localServer ;
	QLocalSocket * m_localSocket ;
	QString m_serverPath ;
	bool m_instanceExist ;
	const char * m_methodName ;
	QString m_device ;
};

#endif // ONEINSTANCE_H

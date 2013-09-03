/*
 *
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

#ifndef OPENMOUNTPOINTINFILEMANAGER_H
#define OPENMOUNTPOINTINFILEMANAGER_H

#include <QObject>
#include <QRunnable>
#include <QString>
#include <QProcess>

class openmountpointinfilemanager :public QObject,public QRunnable
{
	Q_OBJECT
public:
	explicit openmountpointinfilemanager( QString folderOpener,QString path ) ;
	~openmountpointinfilemanager();
	void start( void ) ;
signals:
	void errorStatus( int exitCode,int exitStatus,int startError ) ;
private slots:
	void startError(  QProcess::ProcessError ) ;
private:
	void run( void ) ;
	QString m_path ;
	QString m_folderOpener ;
	int m_exitCode ;
	int m_exitStatus ;
	int m_startError ;
};

#endif // OPENMOUNTPOINTINFILEMANAGER_H

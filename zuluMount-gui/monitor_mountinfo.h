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


#ifndef MONITOR_MOUNTINFO_H
#define MONITOR_MOUNTINFO_H

#include <QThread>
#include <QString>
#include <QStringList>

class QObject ;

class monitor_mountinfo : public QThread
{
	Q_OBJECT
public:
	explicit monitor_mountinfo( QObject * parent = 0 ) ;
signals:
	void stopped( void ) ;
	void threadFinished( void ) ;
	void volumeChanged( QStringList ) ;
	void volumeRemoved( QString ) ;
private slots:
	void threadStopped( void ) ;
	void stop( void ) ;
private:
	bool loopDeviceIsStillPresent( const QString& ) ;
	void removeEntry( QString ) ;
	void run( void ) ;
	QStringList updateVolumeList( void ) ;
	void updateAndCompareVolumeList( void ) ;
	QThread * m_baba ;
	QThread * m_mtoto ;
	QObject * m_babu ;
	monitor_mountinfo * m_main ;
	bool m_threadIsRunning ;
};

#endif // MONITOR_MOUNTINFO_H

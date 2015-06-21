/*
 *
 *  Copyright (c) 2013-2015
 *  name : Francis Banyikwa
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
#ifndef AUTO_MOUNT_H
#define AUTO_MOUNT_H

#include <QThread>

class QObject ;
class events ;
class volumeEntryProperties ;

class events : public QThread
{
	Q_OBJECT
public:
	explicit events( QObject * parent = 0 ) ;
	~events() ;
	void stop( void ) ;
signals:
	void stopped( void ) ;
	void volumeRemoved( QString ) ;
	void volumeMiniProperties( volumeEntryProperties * ) ;
private slots:
	void threadStopped( void ) ;
private:
	void run() ;
	void failedToStart( void ) ;
	QThread * m_baba ;
	QThread * m_mtoto ;
	QObject * m_babu ;
	events  * m_main ;
	bool m_running ;
};

#endif // AUTO_MOUNT_H

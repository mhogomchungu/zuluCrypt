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

#include "monitor_mountinfo.h"

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QProcess>

#include<fcntl.h>
#include <poll.h>

#include "bin_path.h"
#include "task.h"

monitor_mountinfo::monitor_mountinfo( QObject * parent ) : QThread( parent )
{
	m_babu = parent ;
	m_baba = static_cast< QThread * >( this ) ;
	m_main = this ;
}

void monitor_mountinfo::stop()
{
	if( m_threadIsRunning ){
		m_mtoto->terminate() ;
	}else{
		this->threadStopped() ;
	}
}

void monitor_mountinfo::threadStopped()
{
	emit stopped();
	m_threadIsRunning = false ;
}

void monitor_mountinfo::removeEntry( QString device )
{
	emit volumeRemoved( device ) ;
}

void monitor_mountinfo::run()
{
	/*
	 * Not exactly sure what i am doing here but this kind of thing seem to be necessary to prevent
	 * an occassional crash on exit with an error that reads something like "object deleted while thread is still running"
	 */
	m_mtoto = static_cast< QThread * >( this ) ;
	connect( m_mtoto,SIGNAL( terminated() ),m_main,SLOT( threadStopped() ) ) ;
	connect( m_mtoto,SIGNAL( terminated() ),m_mtoto,SLOT( deleteLater() ) ) ;
	connect( m_mtoto,SIGNAL( terminated() ),this,SLOT( deleteLater() ) ) ;

	m_threadIsRunning = true ;

	struct pollfd fds[ 1 ] ;

	int f = open( "/proc/self/mountinfo",O_RDONLY ) ;

	if( f == -1 ){
		m_threadIsRunning = false ;
		return ;
	}else{
		m_threadIsRunning = true ;
	}

	fds[ 0 ].fd = f;
	fds[ 0 ].events = POLLPRI;
	int timeout  = -1 ;
	int e ;

	QStringList volumeList = this->updateVolumeList() ;
	QStringList newList ;

	while( 1 ){
		e = poll( fds,1,timeout ) ;
		if( e == -1 ){
			continue ;
		}

		newList = this->updateVolumeList() ;

		if( volumeList.size() > newList.size() ){
			/*
			 * unmount has just happened
			 */
			int j = newList.size() ;
			for( int i = 0 ; i < j ; i++ ){
				volumeList.removeOne( newList.at( i ) ) ;
			}

			while( volumeList.size() > 0 ){
				const QString& device = volumeList.first() ;
				if( device.startsWith( QString( "/dev/" ) ) ){
					Task * t = new Task() ;
					t->setDevice( device );
					connect( t,SIGNAL( signalProperties( QString ) ),m_babu,SLOT( volumeMiniProperties( QString ) ) ) ;
					t->start( Task::VolumeMiniProperties ) ;
				}else{
					//sleep( 1 ) ; //sleep for one second for UI effect
					m_main->removeEntry( device ) ;
				}
				volumeList.removeAll( device ) ;
			}

			volumeList = newList ;

		}else if( volumeList.size() < newList.size() ){
			/*
			 * mount has happened
			 */
			int j = volumeList.size() ;
			for( int i = 0 ; i < j ; i++ ){
				newList.removeOne( volumeList.at( i ) ) ;
			}

			j = newList.size() ;
			for( int i = 0 ; i < j ; i++ ){
				const QString& device = newList.at( i ) ;
				Task * t = new Task() ;
				t->setDevice( device ) ;
				connect( t,SIGNAL( signalProperties( QString ) ),m_babu,SLOT( volumeMiniProperties( QString ) ) ) ;
				t->start( Task::VolumeMiniProperties ) ;
				volumeList.append( device ) ;
			}
		}else{
			;
		}
	}
}

QStringList monitor_mountinfo::updateVolumeList()
{
	QProcess p ;
	QString exe = QString( "%1 -E" ).arg( zuluMount ) ;
	p.start( exe );
	p.waitForFinished( -1 ) ;

	return QString( p.readAll() ).split( "\n" ) ;
}

void monitor_mountinfo::updateAndCompareVolumeList()
{

}

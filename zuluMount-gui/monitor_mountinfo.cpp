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
#include <QDir>
#include <QFile>

#include <poll.h>
#include<fcntl.h>

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

void monitor_mountinfo::removeEntry( const QString& device )
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

	const char * path = "/proc/self/mountinfo" ;

	Task::FileHandle manage_fd ;

	int fd = manage_fd( open( path,O_RDONLY ) ) ;

	if( fd == -1 ){
		m_threadIsRunning = false ;
		return ;
	}else{
		m_threadIsRunning = true ;
	}

	fds[ 0 ].fd = fd ;
	fds[ 0 ].events = POLLPRI ;
	int e ;

	auto _updateVolumeList = [](){

		QProcess p ;
		QString exe = QString( "%1 -E" ).arg( zuluMount ) ;
		p.start( exe ) ;
		p.waitForFinished( -1 ) ;
		QStringList l = QString( p.readAll() ).split( "\n" ) ;
		l.removeLast() ;
		return l ;
	} ;

	QStringList oldList = _updateVolumeList() ;
	QStringList newList ;
	QStringList temp ;

	while( 1 ){
		e = poll( fds,1,-1 ) ;
		if( e == -1 ){
			continue ;
		}

		newList = _updateVolumeList() ;

		if( oldList.size() > newList.size() ){
			/*
			 * unmount has just happened
			 */
			for( const auto& it : newList ){
				oldList.removeOne( it ) ;
			}
			for( const auto& it : oldList ){
				Task * t = new Task() ;
				t->setDevice( it ) ;
				connect( t,SIGNAL( signalProperties( QString ) ),m_babu,SLOT( volumeMiniProperties( QString ) ) ) ;
				connect( t,SIGNAL( volumeRemoved( QString ) ),m_babu,SLOT( deviceRemoved( QString ) ) ) ;
				t->start( Task::VolumeMiniProperties ) ;
			}
		}else if( newList.size() > oldList.size() ){
			/*
			 * mount has happened
			 */
			temp = newList ;
			for( const auto& it : oldList ){
				temp.removeOne( it ) ;
			}
			for( const auto& it : temp ){
				Task * t = new Task() ;
				t->setDevice( it ) ;
				connect( t,SIGNAL( signalProperties( QString ) ),m_babu,SLOT( volumeMiniProperties( QString ) ) ) ;
				t->start( Task::VolumeMiniProperties ) ;
			}
		}else{
			/*
			 * mount/unmount just happened but volume count remain the same,possible reason is because of a bind mount
			 */
		}

		oldList = newList ;
	}
}

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

bool monitor_mountinfo::loopDeviceIsStillPresent( const QString& device )
{
	QDir d( "/sys/block" ) ;
	QStringList l = d.entryList() ;
	QString e ;
	QString dev = QString( "%1\n" ).arg( device ) ;
	QByteArray s ;
	int j = l.size() ;
	QFile f ;
	for( int i = 0 ; i < j ; i++ ){
		const QString& x = l.at( i ) ;
		if( x.startsWith( "loop" ) ){
			e = QString( "/sys/block/%1/loop/backing_file" ).arg( x ) ;
			f.setFileName( e ) ;
			f.open( QIODevice::ReadOnly ) ;
			s = f.readAll() ;
			f.close() ;
			if( s == dev ){
				return true ;
			}
		}
	}
	return false ;
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

	QStringList oldList = this->updateVolumeList() ;
	QStringList newList ;
	QStringList temp ;

	while( 1 ){
		e = poll( fds,1,timeout ) ;
		if( e == -1 ){
			continue ;
		}

		newList = this->updateVolumeList() ;

		if( oldList.size() > newList.size() ){
			/*
			 * unmount has just happened
			 */
			int j = newList.size() ;
			for( int i = 0 ; i < j ; i++ ){
				oldList.removeOne( newList.at( i ) ) ;
			}
			j = oldList.size() ;
			for( int i = 0 ; i < j ; i++ ){
				const QString& device = oldList.at( i ) ;
				if( device.startsWith( QString( "/dev/" ) ) ){
					Task * t = new Task() ;
					t->setDevice( device );
					connect( t,SIGNAL( signalProperties( QString ) ),m_babu,SLOT( volumeMiniProperties( QString ) ) ) ;
					t->start( Task::VolumeMiniProperties ) ;
				}else{
					if( this->loopDeviceIsStillPresent( device ) ){
						Task * t = new Task() ;
						t->setDevice( device );
						connect( t,SIGNAL( signalProperties( QString ) ),m_babu,SLOT( volumeMiniProperties( QString ) ) ) ;
						t->start( Task::VolumeMiniProperties ) ;
					}else{
						m_main->removeEntry( device ) ;
					}
				}
			}
		}else if( newList.size() > oldList.size() ){
			/*
			 * mount has happened
			 */
			int j = oldList.size() ;
			temp = newList ;
			for( int i = 0 ; i < j ; i++ ){
				temp.removeOne( oldList.at( i ) ) ;
			}

			j = temp.size() ;
			for( int i = 0 ; i < j ; i++ ){
				const QString& device = temp.at( i ) ;
				Task * t = new Task() ;
				t->setDevice( device ) ;
				connect( t,SIGNAL( signalProperties( QString ) ),m_babu,SLOT( volumeMiniProperties( QString ) ) ) ;
				t->start( Task::VolumeMiniProperties ) ;
			}
		}

		oldList = newList ;
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

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

#include <poll.h>
#include <fcntl.h>

#include "task.h"

monitor_mountinfo::monitor_mountinfo( QObject * parent ) : QThread( parent ),m_running( false )
{
	m_babu = parent ;
	m_baba = this ;
	m_main = this ;
}

monitor_mountinfo::~monitor_mountinfo()
{
}

void monitor_mountinfo::stop()
{
	if( m_running ){
		m_mtoto->terminate() ;
	}else{
		this->threadStopped() ;
	}
}

void monitor_mountinfo::threadStopped()
{
	emit stopped();
	m_running = false ;
}

void monitor_mountinfo::run()
{
	m_mtoto = this ;

	connect( m_mtoto,SIGNAL( terminated() ),m_main,SLOT( threadStopped() ) ) ;
	connect( m_mtoto,SIGNAL( terminated() ),m_mtoto,SLOT( deleteLater() ) ) ;

	Task::FileHandle manage_fd ;

	int fd = manage_fd( open( "/proc/self/mountinfo",O_RDONLY ) ) ;

	if( fd == -1 ){
		return ;
	}else{
		m_running = true ;
	}

	struct pollfd monitor ;

	monitor.fd     = fd ;
	monitor.events = POLLPRI ;

	auto _loop = [&](){
		poll( &monitor,1,-1 ) ;
		return 1 ;
	} ;

	auto _unmountProperty = [&]( const QString& volume ){
		Task * t = new Task() ;
		t->setDevice( volume ) ;
		connect( t,SIGNAL( volumeMiniProperties( volumeEntryProperties * ) ),
			 m_babu,SLOT( volumeMiniProperties( volumeEntryProperties * ) ) ) ;
		connect( t,SIGNAL( volumeRemoved( QString ) ),
			 m_babu,SLOT( volumeRemoved( QString ) ) ) ;
		t->start( Task::VolumeMiniProperties ) ;
	} ;

	auto _mountProperty = [&]( const QString& volume ){
		Task * t = new Task() ;
		t->setDevice( volume ) ;
		connect( t,SIGNAL( volumeMiniProperties( volumeEntryProperties * ) ),
			 m_babu,SLOT( volumeMiniProperties( volumeEntryProperties * ) ) ) ;
		t->start( Task::VolumeMiniProperties ) ;
	} ;

	QStringList oldMountList = Task::updateVolumeList() ;
	QStringList newMountList ;

	auto _volumeWasUnMounted = [&](){
		return oldMountList.size() > newMountList.size() ;
	} ;

	auto _volumeWasMounted = [&](){
		return oldMountList.size() < newMountList.size() ;
	} ;

	auto _unmountedVolume = [&]( const QString& e ){
		return !newMountList.contains( e ) ;
	} ;

	auto _mountedVolume = [&]( const QString& e ){
		return !oldMountList.contains( e ) ;
	} ;

	while( _loop() ){

		newMountList = Task::updateVolumeList() ;

		if( _volumeWasUnMounted() ){

			for( const auto& it : oldMountList ){
				if( _unmountedVolume( it ) ){
					_unmountProperty( it ) ;
				}
			}
		}else if( _volumeWasMounted() ){

			for( const auto& it : newMountList ){
				if( _mountedVolume( it ) ){
					_mountProperty( it ) ;
				}
			}
		}else{
			/*
			 * mount/unmount just happened but volume count remain the same,
			 * possible reason is because of a bind mount
			 */
		}

		oldMountList = newMountList ;
	}
}

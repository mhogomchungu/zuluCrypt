/*
 *
 *  Copyright (c) 2013
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

#include <QDebug>

#include <QString>
#include <QObject>
#include <QThread>
#include <QProcess>
#include <QStringList>
#include <QDebug>

#include "bin_path.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <functional>

#include "../zuluCrypt-gui/utility.h"
#include "../zuluCrypt-gui/task.h"
#include "events.h"
#include "zulumounttask.h"
#include <sys/select.h>
/*
 * http://linux.die.net/man/7/inotify
 */

#define _startsWith( x,y ) strncmp( x,y,strlen( y ) ) == 0
#define _contains( x,y ) strstr( x,y ) != 0
#define _stringsAreEqual( x,y ) strcmp( x,y ) == 0

events::events( QObject * parent )
{
	m_baba = this ;
	m_main = this ;
	m_babu = parent ;
}

events::~events()
{
}

void events::stop()
{
	if( m_running ){
		m_mtoto->terminate() ;
	}else{
		this->threadStopped() ;
	}
}

void events::threadStopped()
{
	emit stopped() ;
	m_running = false ;
}

void events::failedToStart()
{
	qDebug() << "inotify_init() failed to start,automounting is turned off" ;
	m_running = false ;
}

void events::run()
{
	m_running = true ;
	m_mtoto   = this ;

	connect( m_mtoto,SIGNAL( finished() ),m_main,SLOT( threadStopped() ) ) ;
	connect( m_mtoto,SIGNAL( finished() ),m_mtoto,SLOT( deleteLater() ) ) ;

	connect( this,SIGNAL( volumeMiniProperties( volumeEntryProperties * ) ),
		 m_babu,SLOT( autoMountVolume( volumeEntryProperties * ) ) ) ;
	connect( this,SIGNAL( volumeRemoved( QString ) ),
		 m_babu,SLOT( volumeRemoved( QString ) ) ) ;

	int fd = inotify_init() ;

	utility_fd_raii_1( &fd ) ;

	if( fd == -1 ){
		return this->failedToStart() ;
	}

	int dev = inotify_add_watch( fd,"/dev",IN_CREATE|IN_DELETE ) ;
	int dm  = inotify_add_watch( fd,"/dev/mapper",IN_CREATE|IN_DELETE ) ;
	int md  = -1 ;

	if( utility::pathExists( "/dev/dm" ) ){
		md = inotify_add_watch( fd,"/dev/md",IN_DELETE ) ;
	}

	auto _allowed_device = []( const char * device ){

		/*
		 * dont care about these devices.
		 * /dev/sgX seem to be created when a usb device is plugged in
		 * /dev/dm-X are dm devices we dont care about since we will be dealing with them differently
		 */
		bool s = _startsWith( device,"sg" )     ||
			 _startsWith( device,"dm-" )    ||
			 _contains( device,"dev/tmp" )  ||
			 _contains( device,"dev-tmp" )  ||
			 _contains( device,".tmp.md." ) ||
			 _contains( device,"md/md-device-map" ) ;

		return s == false ;
	} ;

	auto _device_action = [&]( const struct inotify_event * event ){

		/*
		 * /dev/md/ folder seem to be deleted when the last entry in it is removed and
		 * created before the first entry is added.To account for this,monitor for the
		 * folder creation to start monitoring its contents.
		 */

		if( event->wd == dev && event->mask & IN_CREATE ){

			if( _stringsAreEqual( "md",event->name ) ){
				md = inotify_add_watch( fd,"/dev/md",IN_DELETE ) ;
				return false ;
			}
		}

		if( event->wd == dev && event->mask & IN_DELETE ){
			if( _stringsAreEqual( "md",event->name ) ){
				inotify_rm_watch( md,dev ) ;
				return false ;
			}
		}

		return true ;
	} ;

	const char * currentEvent ;
	const char * lastEvent ;

	constexpr int BUFF_SIZE = 4096 ;
	char buffer[ BUFF_SIZE ] ;

	fd_set rfds ;
	int retval ;
	ssize_t r ;

	FD_ZERO( &rfds ) ;

	int select_fd = fd + 1 ;

	auto _eventsReceived = [&](){

		/*
		 * we are blocking on select() and not on read() because QThread->terminate() does not seem to
		 * be able to get out of a blocked read() on certain Qt versions.
		 */

		FD_SET( fd,&rfds ) ;

		retval = select( select_fd,&rfds,nullptr,nullptr,nullptr ) ;

		if( retval > 0 ){
			r = read( fd,buffer,BUFF_SIZE ) ;
			lastEvent    = buffer + r ;
			currentEvent = buffer ;
			return true ;
		}else{
			return false ;
		}
	} ;

	auto _processEvent = [&]( const struct inotify_event * event ){

		if( _device_action( event ) && _allowed_device( event->name ) ){

			zuluMountTask::event e ;

			e.volumeName = event->name ;
			e.added      = event->mask == IN_CREATE ;

			if( event->wd == dm ){
				e.deviceType = zuluMountTask::dm_device ;
			}else if( event->wd == md ){
				e.deviceType = zuluMountTask::md_device ;
			}else{
				e.deviceType = zuluMountTask::device ;
			}

			Task::exec( [ this,e ](){

				auto r = zuluMountTask::deviceProperties( e ) ;

				if( r.volumeRemoved ){
					emit volumeRemoved( r.volumeName ) ;
				}else{
					emit volumeMiniProperties( r.entry ) ;
				}
			} ) ;
		}
	} ;

	#define _cast( x ) reinterpret_cast< const struct inotify_event * >( currentEvent )
	#define _eventSize sizeof( struct inotify_event )

	while( true ){

		if( _eventsReceived() ){

			while( currentEvent < lastEvent ){

				auto event = _cast( currentEvent ) ;

				if( event ){
					_processEvent( event ) ;
					currentEvent += _eventSize + event->len ;
				}else{
					currentEvent += _eventSize ;
				}
			}
		}
	}
}

/*
 *
 *  Copyright (c) 2013
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

#include "auto_mount.h"
#include <QDebug>

#include <QString>
#include <QObject>
#include <QThread>
#include <QProcess>
#include <QStringList>

#include "bin_path.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../zuluCrypt-gui/utility.h"
#include "task.h"

/*
 * http://linux.die.net/man/7/inotify
 */

auto_mount::auto_mount( QObject * parent )
{
	m_baba = this ;
	m_main = this ;
	m_babu = parent ;
}

auto_mount::~auto_mount()
{
}

void auto_mount::stop()
{
	if( m_threadIsRunning ){
		m_mtoto->terminate() ;
	}else{
		this->threadStopped() ;
	}
}

void auto_mount::threadStopped()
{
	emit stopped() ;
	m_threadIsRunning = false ;
}

void auto_mount::run()
{
	m_mtoto = this ;

	connect( m_mtoto,SIGNAL( terminated() ),m_main,SLOT( threadStopped() ) ) ;
	connect( m_mtoto,SIGNAL( terminated() ),m_mtoto,SLOT( deleteLater() ) ) ;

	Task::FileHandle manage_fd ;

	int fd = manage_fd( inotify_init() ) ;

	if( fd == -1 ){
		qDebug() << "inotify_init() failed to start,automounting is turned off" ;
		m_threadIsRunning = false ;
		return ;
	}else{
		m_threadIsRunning = true ;
	}

	int dev = inotify_add_watch( fd,"/dev",IN_CREATE|IN_DELETE ) ;
	int dm  = inotify_add_watch( fd,"/dev/mapper",IN_CREATE|IN_DELETE ) ;
	int md  = -1 ;

	if( utility::pathExists( "/dev/dm" ) ){
		md = inotify_add_watch( fd,"/dev/md",IN_DELETE ) ;
	}

	auto _allowed_device = []( const char * device ){

		auto _startsWith = []( const char * x,const char * y,size_t z ){
			return strncmp( x,y,z ) == 0 ;
		} ;

		auto _contains = []( const char * x,const char * y ){
			return strstr( x,y ) != 0 ;
		} ;

		/*
		 * dont care about these devices.
		 * /dev/sgX seem to be created when a usb device is plugged in
		 * /dev/dm-X are dm devices we dont care about since we will be dealing with them differently
		 */
		bool s = _startsWith( device,"sg",2 )   ||
			 _startsWith( device,"dm-",3 )  ||
			 _contains( device,"dev/tmp" )  ||
			 _contains( device,"dev-tmp" )  ||
			 _contains( device,".tmp.md." ) ||
			 _contains( device,"md/md-device-map" ) ;

		return s == false ;
	} ;

	auto _stringsAreEqual = []( const char * x,const char * y ){
		return strcmp( x,y ) == 0 ;
	} ;

	auto _deviceType = [&]( const struct inotify_event * event ){
		if( event->wd == dm ){
			return Task::dm_device ;
		}else if( event->wd == md ){
			return Task::md_device ;
		}else{
			return Task::device ;
		}
	} ;

	auto _deviceAction = []( const struct inotify_event * event ){
		if( event->mask == IN_DELETE ){
			return Task::deviceDeleted ;
		}else{
			return Task::deviceAdded ;
		}
	} ;

	auto _device_action = [&]( const struct inotify_event * event ){
		if( event->wd == dev && event->mask & IN_CREATE ){
			/*
			 * /dev/md path seem to be deleted when the last entry in it is removed and
			 * created before the first entry is added.To account for this,monitor for the
			 * folder creation to start monitoring its contents if it get created after we have started
			 */
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

	auto _hasEvent = [&](){
		return currentEvent < lastEvent ;
	} ;

	auto _getEvent = [&](){

		auto event = reinterpret_cast< const struct inotify_event * >( currentEvent ) ;

		if( event ){
			currentEvent += sizeof( struct inotify_event ) + event->len ;
		}else{
			currentEvent += sizeof( struct inotify_event ) ;
		}

		return event ;
	} ;

	auto _readEvents = [&](){

		auto r = read( fd,buffer,BUFF_SIZE ) ;
		lastEvent = buffer + r ;
		currentEvent = buffer ;
		return true ;
	} ;

	auto _processEvent = [&]( const struct inotify_event * event ){

		if( !event ){
			return ;
		}
		if( _device_action( event ) && _allowed_device( event->name ) ){

			Task * t = new Task() ;

			connect( t,SIGNAL( volumeMiniProperties( volumeEntryProperties * ) ),
				 m_babu,SLOT( autoMountVolume( volumeEntryProperties * ) ) ) ;
			connect( t,SIGNAL( volumeRemoved( QString ) ),
				 m_babu,SLOT( volumeRemoved( QString ) ) ) ;

			t->setDeviceType( _deviceType( event ) ) ;
			t->setDeviceAction( _deviceAction( event ) ) ;
			t->setDevice( event->name ) ;

			t->start( Task::deviceProperty ) ;
		}

	} ;

	while( _readEvents() ){

		while( _hasEvent() ){

			_processEvent( _getEvent() ) ;
		}
	}
}

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

auto_mount::auto_mount( QObject * parent ) :m_fdDir( -1 ),m_thread_helper( 0 ),m_babu( parent ),m_autoMount( this )
{
}

auto_mount::~auto_mount()
{
	if( m_fdDir != -1 ){
		close( m_fdDir ) ;
	}
}

void auto_mount::stop()
{
	if( m_threadIsRunning ){
		m_mtoto->terminate();
	}else{
		this->threadStopped();
	}
}

void auto_mount::threadStopped()
{
	emit stopped();
	m_threadIsRunning = false ;
}

void auto_mount::run()
{
	m_mtoto = this ;
	connect( m_mtoto,SIGNAL( terminated() ),m_autoMount,SLOT( threadStopped() ) ) ;
	connect( m_mtoto,SIGNAL( terminated() ),this,SLOT( deleteLater() ) ) ;

	#define BUFF_SIZE 4096
	char buffer[ BUFF_SIZE ];

	m_fdDir = inotify_init() ;
	if( m_fdDir == -1 ){
		qDebug() << "inotify_init() failed to start,automounting is turned off";
		m_threadIsRunning = false ;
		return ;
	}else{
		m_threadIsRunning = true ;
	}

	int dev    = inotify_add_watch( m_fdDir,"/dev",IN_CREATE|IN_DELETE ) ;
	int mapper = inotify_add_watch( m_fdDir,"/dev/mapper",IN_CREATE|IN_DELETE ) ;
	int md     =  inotify_add_watch( m_fdDir,"/dev/md",IN_DELETE ) ;

	struct inotify_event * pevent ;
	QString device ;

	const char * f ;
	const char * z ;
	int data_read ;
	int baseSize = sizeof( struct inotify_event ) ;

	while( 1 ) {

		data_read = read( m_fdDir,buffer,BUFF_SIZE ) ;

		z = buffer + data_read ;

		for( f = buffer ; f < z ; f = f + baseSize + pevent->len ){

			pevent = ( struct inotify_event * )f;

			m_device = f + baseSize ;

			if( strncmp( m_device,"sg",2 ) == 0 ||
					strncmp( m_device,"dm-",3 ) == 0 ||
					strstr( m_device,".dev/tmp" ) != NULL ||
					strstr( m_device,".tmp.md." ) != NULL ) {
				/*
				 * dont care about these devices.
				 * /dev/sgX seem to be created when a usb device is plugged in
				 * /dev/dm-X are dm devices we dont care about since we will be dealing with them differently
				 */
				 ;
			}else{
				if( pevent->wd == md ){
					device = QString( "/dev/md/" ) + m_device ;
				}else if( pevent->wd == dev || pevent->wd == mapper ){
					device = QString( "/dev/" ) + m_device ;
				}else{
					;
				}

				m_thread_helper = new auto_mount_helper() ;

				connect( m_thread_helper,SIGNAL( getVolumeSystemInfo( QStringList ) ),
					 m_babu,SLOT( autoMountVolumeSystemInfo( QStringList ) ) ) ;
				connect( m_thread_helper,SIGNAL( getVolumeInfo( QStringList ) ),
					 m_babu,SLOT( autoMountVolumeInfo( QStringList ) ) ) ;
				connect( m_thread_helper,SIGNAL( deviceRemoved( QString ) ),
					 m_babu,SLOT( deviceRemoved( QString ) ) ) ;

				if( pevent->wd == dev || pevent->wd == md ){
					m_thread_helper->start( device,auto_mount_helper::dev,pevent->mask ) ;
				}else if( pevent->wd == mapper ){
					m_thread_helper->start( device,auto_mount_helper::dev_mapper,pevent->mask ) ;
				}else{
					;
				}
			}
		}
	}
}

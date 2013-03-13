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

auto_mount::auto_mount( QObject * parent ) : QThread( parent ),m_buffer( 0 ),m_thread_helper( 0 )
{
}

auto_mount::~auto_mount()
{
	if( m_fdDir != -1 ){
		close( m_fdDir ) ;
	}
	if( m_buffer ){
		delete[] m_buffer ;
	}
}

void auto_mount::run()
{
	size_t offset = sizeof( struct inotify_event ) ;
	size_t BUFF_SIZE = FILENAME_MAX + offset ;

	m_buffer = new char[ BUFF_SIZE ];

	m_fdDir = inotify_init() ;
	if( m_fdDir == -1 ){
		return ;
	}

	int dev = inotify_add_watch( m_fdDir,"/dev",IN_CREATE|IN_DELETE ) ;
	int mapper = inotify_add_watch( m_fdDir,"/dev/mapper",IN_CREATE|IN_DELETE ) ;

	if( dev == -1 || mapper == - 1){
		return ;
	}

	m_device = m_buffer + offset ;
	struct inotify_event * pevent ;
	QString device ;

	QObject * parent = this->parent() ;

	while( 1 ) {

		read( m_fdDir,m_buffer,BUFF_SIZE ) ;
		pevent = ( struct inotify_event * )m_buffer;

		if( strncmp( m_device,"sg",2 ) == 0 ) {
			continue ;
		}
		if( strncmp( m_device,"dm-",3 ) == 0 ) {
			continue ;
		}
		if( strstr( m_buffer + offset,".dev/tmp" ) != NULL ) {
			continue ;
		}

		device = QString( "/dev/" ) + m_device ;

		m_thread_helper = new auto_mount_helper() ;

		connect( m_thread_helper,SIGNAL( getVolumeSystemInfo( QStringList ) ),parent,SLOT( autoMountVolumeSystemInfo( QStringList ) ) ) ;
		connect( m_thread_helper,SIGNAL( getVolumeInfo( QStringList ) ),parent,SLOT( autoMountVolumeInfo( QStringList ) ) ) ;
		connect( m_thread_helper,SIGNAL( deviceRemoved( QString ) ),parent,SLOT( deviceRemoved( QString ) ) ) ;

		if( pevent->wd == dev ){
			m_thread_helper->start( device,0,pevent->mask ) ;
		}else{
			m_thread_helper->start( device,1,pevent->mask ) ;
		}
	}
}


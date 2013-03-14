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

#include "auto_mount_helper.h"
#include <QDebug>
auto_mount_helper::auto_mount_helper( QObject * parent )
{
	Q_UNUSED( parent ) ;
}

auto_mount_helper::~auto_mount_helper()
{
}

bool auto_mount_helper::deviceIsSystem( void )
{
	QProcess p ;
	QString exe = QString( "%1 -S" ).arg( zuluMount ) ;
	p.start( exe );
	p.waitForFinished() ;
	QString s = QString( p.readAll() ) ;
	p.close();
	QStringList l = s.split( "\n" ) ;
	int j = l.size() ;

	for( int i = 0 ; i < j ; i++ ){
		if( l.at( i ) == m_device ){
			return true ;
		}
	}
	return false ;
}

void auto_mount_helper::volumeProperties( void )
{
	QProcess p ;
	QString exe = QString( "%1 -L -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;
	p.start( exe );
	p.waitForFinished() ;
	QString m = p.readAll() ;

	if( p.exitCode() == 0 ){
		QStringList l =  m.split( "\t" ) ;
		if( l.size() >= 4 ){
			if( this->deviceIsSystem() ){
				emit getVolumeSystemInfo( l ) ;
			}else{
				emit getVolumeInfo( l );
			}
		}
	}
}

void auto_mount_helper::deviceFromDev( void )
{
	if( m_device.startsWith( QString( "/dev/sd") ) || m_device.startsWith( QString( "/dev/hd" ) ) || m_device.startsWith( QString( "mmc" ) ) ){
		if( m_mask & IN_CREATE ) {
			this->volumeProperties() ;
		}else if( m_mask & IN_DELETE ){
			emit deviceRemoved( m_device ) ;
		}else{
			;
		}
	}else{
		;
	}
}

void auto_mount_helper::deviceFromDevMapper( void )
{
	int index1 = m_device.lastIndexOf( "-" ) ;

	if( index1 == -1 ){
		return ;
	}

	QString device = m_device.replace( index1,1,QString( "/" ) ) ;

	if( this->deviceMatchLVMFormat() ){
		if( m_mask & IN_CREATE ) {
			this->volumeProperties() ;
		}else if( m_mask & IN_DELETE ){
			emit deviceRemoved( device ) ;
		}else{
			;
		}
	}else{
		;
	}
}

bool auto_mount_helper::deviceMatchLVMFormat( void )
{
	if( m_device.contains( QString( ".udev/tmp" ) ) ){
		/*
		 * ignore what appear to be a temporary intermediate path
		 */
		return false ;
	}

	QByteArray b = m_device.toAscii() ;
	const char * e = b.constData() ;

	int i = 0 ;
	/*
	 * one of the two formats on LVM path is /dev/abc/def,below code checks if a volume is an LVM volume
	 * by counting the numder of "/" characters and an assumption is made that the volume is LVM if
	 * the number is 3
	 */
	while( *e ){
		if( *e == '/' ){
			i++ ;
		}
		e++ ;
	}
	return i == 3 ;
}

void auto_mount_helper::run()
{
	if( m_type == 0 ){
		this->deviceFromDev();
	}else{
		this->deviceFromDevMapper();
	}
}

void auto_mount_helper::start( QString device,int type,u_int32_t mask )
{
	m_device = device ;
	m_type   = type ;
	m_mask   = mask ;
	QThreadPool::globalInstance()->start( this );
}

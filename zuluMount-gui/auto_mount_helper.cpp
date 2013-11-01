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

#include <QThreadPool>

#include "bin_path.h"
#include <QProcess>
#include <QDir>

#include <unistd.h>
#include <sys/inotify.h>

#include "../zuluCrypt-gui/utility.h"

auto_mount_helper::auto_mount_helper( QObject * parent )
{
	connect( this,SIGNAL( getVolumeSystemInfo( QStringList ) ),parent,SLOT( autoMountVolumeSystemInfo( QStringList ) ) ) ;
	connect( this,SIGNAL( getVolumeInfo( QStringList ) ),parent,SLOT( autoMountVolumeInfo( QStringList ) ) ) ;
	connect( this,SIGNAL( deviceRemoved( QString ) ),parent,SLOT( deviceRemoved( QString ) ) ) ;
}

auto_mount_helper::~auto_mount_helper()
{
}

bool auto_mount_helper::deviceIsSystem( const QString& device )
{
	QProcess p ;
	QString exe = QString( "%1 -S" ).arg( zuluMount ) ;
	p.start( exe ) ;
	p.waitForFinished() ;
	if( p.exitCode() == 0 ){
		QString s = QString( p.readAll() ) ;
		p.close() ;
		QStringList l = s.split( "\n" ) ;
		int j = l.size() ;

		for( int i = 0 ; i < j ; i++ ){
			if( l.at( i ) == device ){
				return true ;
			}
		}
	}
	return false ;
}

void auto_mount_helper::volumeProperties( const QString& device )
{
	QProcess p ;

	QString exe = QString( "%1 -L -d \"%2\"" ).arg( zuluMount ).arg( device ) ;
	p.start( exe ) ;
	p.waitForFinished() ;
	QString m = p.readAll() ;
	QStringList l ;
	if( p.exitCode() == 0 ){
		l =  m.split( "\t" ) ;
		if( l.size() >= 4 ){
			if( this->deviceIsSystem( device ) ){
				emit getVolumeSystemInfo( l ) ;
			}else{
				emit getVolumeInfo( l ) ;
			}
		}
	}else{
		/*
		 * We will get here if the device if found to have not supported volume types ie LVM and MDRAID signatures.
		 */
	}
}

void auto_mount_helper::deviceFromDev( const QString& device )
{
	if( device.startsWith( QString( "/dev/sd") ) ||
			device.startsWith( QString( "/dev/hd" ) ) ||
			device.startsWith( QString( "/dev/mmc" ) ) ||
			device.startsWith( QString( "/dev/md" ) ) ){
		if( m_mask & IN_CREATE ) {
			this->volumeProperties( device ) ;
		}else if( m_mask & IN_DELETE ){
			emit deviceRemoved( device ) ;
		}else{
			;
		}
	}else{
		;
	}
}

bool auto_mount_helper::deviceMatchLVMFormat( const QString& device )
{
	/*
	 * LVM paths have two formats,"/dev/mapper/abc-def" and "/dev/abc/def/".
	 * The pass in argument is in the form of "/dev/abc/def" and the path is assumed to be
	 * an LVM path if a corresponding "/dev/mapper/abc-def" path is found
	 *
	 * We are just doing a simple test below and return true is the path is simply in "/dev/abc/def"
	 * format by counting the number of "/"
	 */
	QStringList l = device.split( "/" ) ;
	return l.size() == 4 ;
}

void auto_mount_helper::deviceFromDevMapper( const QString& device_1 )
{
	QString device = device_1 ;

	int index1 = device.lastIndexOf( "-" ) ;

	if( index1 == -1 ){
		return ;
	}

	device.replace( index1,1,QString( "/" ) ) ;

	if( this->deviceMatchLVMFormat( device ) ){
		if( m_mask & IN_CREATE ) {
			this->volumeProperties( device ) ;
		}else if( m_mask & IN_DELETE ){
			emit deviceRemoved( device ) ;
		}else{
			;
		}
	}else{
		;
	}
}

QString auto_mount_helper::mdRaidPath( const QString& dev )
{
	QString dev_1 ;
	QDir d( "/dev/md/" ) ;
	QDir f ;

	/*
	* wait for a while because things dont always happen as expect if we check too soon.
	*/
	sleep( 4 ) ;

	if( d.exists() ){
		QStringList l = d.entryList() ;
		int j = l.size() ;
		QString e ;
		for( int i = 0 ; i < j ; i++ ){
			e = l.at( i ) ;
			if( e == QString( "." ) || e == QString( ".." ) || e.contains( QString( "/dev/.tmp" ) ) ){
				continue ;
			}
			dev_1 = QString( "/dev/md/" ) + e ;
			f.setPath( dev_1 ) ;
			if( f.canonicalPath() == dev ){
				return dev_1 ;
			}
		}
	}
	return dev ;
}

void auto_mount_helper::deviceFromMdRaid( const QString& device )
{
	if( device.startsWith( QString( "/dev/md" ) ) ){
		if( m_mask & IN_CREATE ){
			this->volumeProperties( this->mdRaidPath( device ) ) ;
		}else if( m_mask & IN_DELETE ){
			emit deviceRemoved( this->mdRaidPath( device ) ) ;
		}else{
			;
		}
	}else{
		;
	}
}

void auto_mount_helper::removeMdRaidDevice( const QString& device )
{
	emit deviceRemoved( device ) ;
}

void auto_mount_helper::run()
{
	QString device = QString( "/dev/" ) + m_device ;

	if( m_type == auto_mount_helper::dev ){
		if( m_device.startsWith( QString( "md" ) ) ){
			return this->deviceFromMdRaid( device ) ;
		}else{
			return this->deviceFromDev( device ) ;
		}
	}else if( m_type == auto_mount_helper::dev_md ){
		return this->removeMdRaidDevice( QString( "/dev/md/" ) + m_device ) ;
	}else{
		return this->deviceFromDevMapper( device ) ;
	}
}

void auto_mount_helper::start( QString device,int type,u_int32_t mask )
{
	m_device = device ;
	m_type   = type ;
	m_mask   = mask ;
	QThreadPool::globalInstance()->start( this ) ;
}

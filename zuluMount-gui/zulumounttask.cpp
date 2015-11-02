/*
 *
 *  Copyright (c) 2014-2015
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

#include "bin_path.h"
#include "zulumounttask.h"

#include <QDir>
#include <QString>
#include <QDebug>
#include <QFile>

static QString _convert_lvm_path( const QString& dev )
{
	QString volume = dev ;
	QByteArray e = dev.toLatin1() ;

	char * begin = e.data() ;
	char * end = begin + e.size() ;

	char * k ;
	char * c ;
	char * d ;

	for( auto it = begin + 3 ; it < end ; it++ ){

		k = it - 2 ;
		c = it - 1 ;
		d = it ;

		if( *k != '-' && *c == '-' && *d != '-' ){

			*c = '/' ;

			volume = e ;

			while( volume.contains( "--" ) ){

				volume.replace( "--","-" ) ;
			}

			break ;
		}
	}

	return volume ;
}

static QString _convert_md_raid_path( const QString& dev,bool wait )
{
	QString volume = dev ;

	QString dev_1 ;
	QDir d( "/dev/md/" ) ;
	QDir f ;

	if( wait ){
		/*
		 * wait for a while because things dont always happen as expected if we check too soon.
		 */
		utility::Task::wait( 4 ) ;
	}

	if( d.exists() ){

		QStringList l = d.entryList() ;

		for( const auto& it : l ){

			dev_1 = "/dev/md/" + it ;
			f.setPath( dev_1 ) ;

			if( f.canonicalPath() == dev ){

				volume = dev_1 ;

				break ;
			}
		}
	}

	return volume ;
}

static QString _device( const QString& device )
{
	if( device.startsWith( "UUID" ) ){
		return device ;
	}else{
		QString d = device ;
		return d.replace( "\"","\"\"\"" ) ;
	}
}

static bool _volumeIsSystemVolume( const QString& e )
{
	return utility::Task( utility::appendUserUID( "%1 -S" ).arg( zuluMountPath ) ).splitOutput( '\n' ).contains( e ) ;
}

QStringList zuluMountTask::mountedVolumeList( void )
{
	return utility::Task( utility::appendUserUID( "%1 -E" ).arg( zuluMountPath ) ).splitOutput( '\n' ) ;
}

volumeEntryProperties _getVolumeProperties( const QString& e )
{
	QString device = _device( e ) ;

	auto r = utility::Task( utility::appendUserUID( "%1 -L -d \"%2\"" ).arg( zuluMountPath,device ) ) ;

	if( r.success() ) {

		return volumeEntryProperties( r.splitOutput( '\t' ),_volumeIsSystemVolume( device ) ) ;
	}else{
		return volumeEntryProperties() ;
	}
}

Task::future< volumeEntryProperties >& zuluMountTask::getVolumeProperties( const QString& e )
{
	return Task::run< volumeEntryProperties >( [ e ](){ return _getVolumeProperties( e ) ; } ) ;
}

Task::future< QString >& zuluMountTask::volumeProperties( const QString& v,const QString& volumeType )
{
	return Task::run< QString >( [ = ](){

		if( v.isEmpty() ){

			return QString() ;
		}

		QString volume = _device( v ) ;

		auto r = utility::Task( utility::appendUserUID( "%1 -s -d \"%2\"" ).arg( zuluMountPath,volume ) ) ;

		if( r.ok() ){

			return QString( r.output() ) ;
		}else{
			if( volumeType.contains( "crypto_PLAIN\n" ) ){

				/*
				* this could be a plain volume opened with an offset
				*/

				QString e = utility::appendUserUID( "%1 -s -o bogusNecessaryArgument -d \"%2\"" ) ;
				r = utility::Task( e.arg( zuluMountPath,volume ) ) ;

				if( r.ok() ){

					return QString( r.output() ) ;
				}else{
					return QString() ;
				}
			}else{
				return QString() ;
			}
		}
	} ) ;
}

utility::Task zuluMountTask::volumeUnmount( const QString& volumePath,const QString& volumeType )
{
	auto _run = []( const QString& exe ){

		auto e = utility::Task( exe ) ;

		QString output = e.output() ;
		int index = output.indexOf( QChar( ':' ) ) ;
		e.output( output.mid( index + 1 ).toLatin1() ) ;

		return e ;
	} ;

	QString volume = _device( volumePath ) ;

	auto r = _run( utility::appendUserUID( "%1 -u -d \"%2\"" ).arg( zuluMountPath,volume ) ) ;

	if( r.failed() ){

		if( volumeType.contains( "crypto_PLAIN\n" ) ){
			/*
			 * we could be trying to unmount a volume with an offset
			 */
			r = _run( utility::appendUserUID( "%1 -o bogusNecessaryArgument -u -d \"%2\"" ).arg( zuluMountPath,volume ) ) ;
		}
	}

	return r ;
}

Task::future< utility::Task >& zuluMountTask::unmountVolume( const QString& volumePath,const QString& volumeType )
{
	return Task::run< utility::Task >( [ = ](){ return zuluMountTask::volumeUnmount( volumePath,volumeType ) ; } ) ;
}

static QString _excludeVolumePath()
{
	return utility::homePath() + "/.zuluCrypt/zuluMount-gui-excludeVolumes" ;
}

struct deviceList
{
	deviceList( const QString& dev,const QString& n ) : uniqueName( n )
	{
		if( dev.startsWith( "/dev/dm-" ) ){

			QFile file( "/sys/block/" + dev.split( '/' ).last() + "/dm/name" ) ;

			if( file.open( QIODevice::ReadOnly ) ){

				QString e = file.readAll() ;
				e.truncate( e.size() - 1 ) ;
				device = _convert_lvm_path( "/dev/" + e ) ;
			}else{
				device = dev ;
			}

		}else if( dev.startsWith( "/dev/md" ) ){

			device = _convert_md_raid_path( dev,false ) ;
		}else{
			device = dev ;
		}
	}
	deviceList()
	{
	}
	QString device ;
	QString uniqueName ;
};

static QVector< deviceList > _getDevices()
{
	const char * p = "/dev/disk/by-id/" ;

	QDir d( p ) ;
	QDir e ;

	QStringList l = d.entryList() ;

	l.removeOne( "." ) ;
	l.removeOne( ".." ) ;

	QVector< deviceList > devices ;

	if( l.isEmpty() ){

		return devices ;
	}else{
		for( const auto& it : l ){

			e.setPath ( p + it ) ;

			devices.append( deviceList( e.canonicalPath(),it ) ) ;
		}
	}

	return devices ;
}

static QString _getUniqueName( const QString& device )
{
	for( const auto& it : _getDevices() ){

		if( it.device == device ){

			return it.uniqueName ;
		}
	}

	return QString() ;
}

static QStringList _split( QFile& f )
{
	return utility::split( f.readAll() ) ;
}

void zuluMountTask::addVolumeToHiddenVolumeList( const QString& e )
{
	QFile f( _excludeVolumePath() ) ;

	if( f.open( QIODevice::WriteOnly | QIODevice::Append ) ){

		QString a = _getUniqueName( e ) ;

		if( !a.isEmpty() ){

			f.write( a.toLatin1() + "\n" ) ;
		}
	}
}

QStringList zuluMountTask::hiddenVolumeList()
{
	QFile f( _excludeVolumePath() ) ;

	if( f.open( QIODevice::ReadOnly ) ){

		QVector< deviceList > l = _getDevices() ;

		QStringList e ;

		QStringList g = _split( f ) ;

		for( const auto& it : l ){

			if( g.contains( it.uniqueName ) ){

				e.append( it.device ) ;
			}
		}

		return  e ;
	}else{
		return QStringList() ;
	}
}

void zuluMountTask::removeVolumeFromHiddenVolumeList( const QString& e )
{
	QFile f( _excludeVolumePath() ) ;

	if( f.open( QIODevice::ReadOnly ) ){

		QStringList l = _split( f ) ;

		l.removeAll( _getUniqueName( e ) ) ;

		f.close() ;

		if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

			if( !l.isEmpty() ){

				for( const auto& it : l ){

					f.write( it.toLatin1() + "\n" ) ;
				}
			}
		}
	}
}

Task::future< QVector< volumeEntryProperties > >& zuluMountTask::updateVolumeList()
{
	return Task::run< QVector< volumeEntryProperties > >( [](){

		QStringList l = zuluMountTask::hiddenVolumeList() ;

		auto _validEntry = [ & ]( const QString& e ){

			if( e.startsWith( "/dev/md/md-device-map" ) ){

				return false ;
			}
			if( e.contains( "\tswap\t" ) || e.contains( "member\t" ) || e.contains( "\t/run/media/public" ) ){

				return false ;
			}
			if( !l.isEmpty() ){

				for( const auto& it : l ){

					if( e.startsWith( it + '\t' ) ){

						return false ;
					}
				}
			}
			return true ;
		} ;

		QVector< volumeEntryProperties > list ;

		auto all = utility::Task( utility::appendUserUID( "%1 -l" ).arg( zuluMountPath ),10000 ) ;

		if( all.finished() ){

			auto system = utility::Task( utility::appendUserUID( "%1 -S" ).arg( zuluMountPath ),10000 ) ;

			if( system.finished() ){

				auto a = all.splitOutput( '\n' ) ;
				auto s = system.splitOutput( '\n' ) ;

				for( const auto& it : a ){

					if( _validEntry( it ) ){

						const auto& e = utility::split( it,'\t' ) ;
						list.append( volumeEntryProperties( e,s.contains( e.first() ) ) ) ;
					}
				}
			}
		}

		return list ;
	} ) ;
}

void zuluMountTask::checkUnMount( const QString& volume )
{
	utility::Task( utility::appendUserUID( "%1 -c -d \"%2\"" ).arg( zuluMountPath,_device( volume ) ) ) ;
}

volumeMiniPropertiesTaskResult zuluMountTask::volumeMiniProperties( const QString& volume )
{
	auto _loopDeviceIsGone =[]( const QString& device ){

		QDir d( "/sys/block" ) ;
		QStringList l = d.entryList() ;
		QString e ;
		QString dev = QString( "%1\n" ).arg( device ) ;
		QByteArray s ;
		QFile f ;

		for( const auto& it : l ){

			if( it.startsWith( "loop" ) ){

				e = QString( "/sys/block/%1/loop/backing_file" ).arg( it ) ;

				f.setFileName( e ) ;

				if( f.open( QIODevice::ReadOnly ) ){

					s = f.readAll() ;

					f.close() ;
				}

				if( s == dev ){

					return false ;
				}
			}
		}
		return true ;
	} ;

	volumeMiniPropertiesTaskResult s{ volume,false,nullptr } ;

	if( !volume.startsWith( "UUID" ) && !volume.startsWith( "/dev/" ) ){

		/*
		 * There is some sort of a race condition here and things do not always work as expected
		 * try to sleep for a second to see if it will help
		 */

		utility::Task::waitForOneSecond() ;

		if( _loopDeviceIsGone( volume ) ){

			/*
			 * we were just asked to find properties of a loop device
			 * that no longer exists,remove it from the list in the GUI window
			 */

			s.volumeRemoved = true ;
			return s ;
		}
	}

	auto r = utility::Task( utility::appendUserUID( "%1 -L -d \"%2\"" ).arg( zuluMountPath,volume ) ) ;

	if( r.success() ){

		s.entry = new volumeEntryProperties( r.splitOutput( '\t' ),_volumeIsSystemVolume( volume ) ) ;
	}

	return s ;
}

volumeMiniPropertiesTaskResult zuluMountTask::deviceProperties( const zuluMountTask::event& deviceProperty )
{
	auto _mdRaidDevice = [&]( const QString& device ){

		QString d = _convert_md_raid_path( device,true ) ;

		volumeMiniPropertiesTaskResult s{ d,false,nullptr } ;

		if( deviceProperty.added ){

			s.entry = new volumeEntryProperties( _getVolumeProperties( d ) ) ;
		}else{
			s.volumeRemoved = true ;
		}

		return s ;
	} ;

	auto _dmDevice = [&]( const QString& device ){

		QString d = _convert_lvm_path( device ) ;

		volumeMiniPropertiesTaskResult s{ d,false,nullptr } ;

		if( deviceProperty.added ){

			s.entry = new volumeEntryProperties( _getVolumeProperties( d ) ) ;
		}else{
			s.volumeRemoved = true ;
		}

		return s ;
	} ;

	auto _normalDevice = [&]( const QString& device ){

		auto _allowed_device = []( const QString& device ){

			return	device.startsWith( "/dev/sd" )  ||
				device.startsWith( "/dev/hd" )  ||
				device.startsWith( "/dev/mmc" ) ;
		} ;

		volumeMiniPropertiesTaskResult s{ QString(),false,nullptr } ;

		if( _allowed_device( device ) ){

			s.volumeName = device ;

			if( deviceProperty.added ){

				s.entry = new volumeEntryProperties( _getVolumeProperties( device ) ) ;
			}else{
				s.volumeRemoved = true ;
			}
		}

		return s ;
	} ;

	auto _shouldNotGetHere = [](){

		return volumeMiniPropertiesTaskResult{ QString(),false,nullptr } ;
	} ;

	QString device = QString( "/dev/%1" ).arg( deviceProperty.volumeName ) ;

	switch( deviceProperty.deviceType ){

		case zuluMountTask::devices::device    : return _normalDevice( device ) ;
		case zuluMountTask::devices::md_device : return _mdRaidDevice( device ) ;
		case zuluMountTask::devices::dm_device : return _dmDevice( device )     ;
		default                                : return _shouldNotGetHere() ;
	}
}

static bool _delete_encfs_mount_point( const QString& m )
{
	return utility::Task( utility::appendUserUID( "%1 -b %2" ).arg( zuluMountPath,m ) ).success() ;
}

static bool _delete_encfs_m_point( const QString& m )
{
	_delete_encfs_mount_point( m ) ;
	return false ;
}

static bool _create_encfs_mount_point( const QString& m )
{
	return utility::Task( utility::appendUserUID( "%1 -B %2" ).arg( zuluMountPath,m ) ).success() ;
}

Task::future<bool>& zuluMountTask::encfsUnmount( const QString& m )
{
	return Task::run< bool >( [ m ](){

		auto _umount = [ & ](){

			if( utility::Task( "fusermount -u " + m,10000 ).success() ){

				return _delete_encfs_mount_point( m ) ;
			}else{
				return false ;
			}
		} ;

		for( int i = 0 ; i < 5 ; i++ ){

			if( _umount() ){

				return true ;
			}else{
				utility::Task::waitForOneSecond() ;
			}
		}

		return false ;
	} ) ;
}

Task::future<bool>& zuluMountTask::encfsMount( const QString& p,const QString& m,const QString& k,bool ro )
{
	return Task::run< bool >( [ p,m,k,ro ](){

		auto _encfsMount = [ & ](){

			auto _mount = [ & ](){

				QString exe ;

				if( ro ){

					exe = QString( "/usr/bin/encfs -S %1 %2 -o ro" ).arg( p,m ) ;
				}else{
					exe = QString( "/usr/bin/encfs -S %1 %2" ).arg( p,m ) ;
				}

				QProcess e ;

				e.start( exe ) ;
				e.waitForStarted() ;
				e.write( k.toLatin1() + '\n' ) ;

				e.closeWriteChannel() ;

				if( e.waitForFinished( 10000 ) ){

					return e.exitCode() == 0 ;
				}else{
					return false ;
				}
			} ;

			if( _create_encfs_mount_point( m ) ){

				if( _mount() ) {

					return true ;
				}else{
					return _delete_encfs_m_point( m ) ;
				}
			}else{
				return false ;
			}
		} ;

		QDir d( p ) ;

		QStringList l = d.entryList( QDir::Hidden | QDir::Files ) ;

		for( const auto& it : l ){

			if( it.startsWith( ".encfs" ) && it.endsWith( ".xml" ) ){

				/*
				 * encfs folders usually have a config hidden file name named ".encfs6.xml"
				 * and we assume the folder contains encfs files only if this file
				 * is present.
				 */

				return _encfsMount() ;
			}
		}

		return false ;
	} ) ;
}

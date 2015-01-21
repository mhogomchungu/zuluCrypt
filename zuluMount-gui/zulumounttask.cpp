/*
 *
 *  Copyright (c) 2014
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
	return utility::Task( QString( "%1 -S" ).arg( zuluMountPath ) ).splitOutput( '\n' ).contains( e ) ;
}

QStringList zuluMountTask::mountedVolumeList( void )
{
	return utility::Task( QString( "%1 -E" ).arg( zuluMountPath ) ).splitOutput( '\n' ) ;
}

volumeEntryProperties _getVolumeProperties( const QString& e )
{
	QString device = _device( e ) ;

	auto r = utility::Task( QString( "%1 -L -d \"%2\"" ).arg( zuluMountPath,device ) ) ;

	if( r.success() ) {
		volumeEntryProperties v( r.splitOutput( '\t' ) ) ;
		v.setisSystem( _volumeIsSystemVolume( device ) ) ;
		return v ;
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

		auto r = utility::Task( QString( "%1 -s -d \"%2\"" ).arg( zuluMountPath,volume ) ) ;

		if( r.ok() ){
			return QString( r.output() ) ;
		}else{
			if( volumeType.contains( "crypto_PLAIN\n" ) ){
				/*
				* this could be a plain volume opened with an offset
				*/
				r = utility::Task( QString( "%1 -s -o bogusNecessaryArgument -d \"%2\"" ).arg( zuluMountPath,volume ) ) ;
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

	auto r = _run( QString( "%1 -u -d \"%2\"" ).arg( zuluMountPath,volume ) ) ;

	if( r.failed() ){
		if( volumeType.contains( "crypto_PLAIN\n" ) ){
			/*
			 * we could be trying to unmount a volume with an offset
			 */
			r = _run( QString( "%1 -o bogusNecessaryArgument -u -d \"%2\"" ).arg( zuluMountPath,volume ) ) ;
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
	return QDir::homePath() + "/.zuluCrypt/zuluMount-gui-excludeVolumes" ;
}

void zuluMountTask::addVolumeToHiddenVolumeList( const QString& e )
{
	QFile f( _excludeVolumePath() ) ;

	if( f.open( QIODevice::WriteOnly | QIODevice::Append ) ){

		f.write( e.toLatin1() + "\n" ) ;
	}
}

QStringList zuluMountTask::hiddenVolumeList()
{
	QFile f( _excludeVolumePath() ) ;

	if( f.open( QIODevice::ReadOnly ) ){

		return QString( f.readAll() ).split( '\n',QString::SkipEmptyParts ) ;
	}else{
		return QStringList() ;
	}
}

void zuluMountTask::removeVolumeFromHiddenVolumeList( const QString& e )
{
	QFile f( _excludeVolumePath() ) ;

	if( f.open( QIODevice::ReadOnly ) ){

		auto l = QString( f.readAll() ).split( '\n',QString::SkipEmptyParts ) ;

		l.removeAll( e ) ;

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
			if( e.contains( "\tswap\t") || e.contains( "member\t" ) || e.contains( "\t/run/media/public" ) ){
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

		auto all = utility::Task( QString( "%1 -l" ).arg( zuluMountPath ),10000 ) ;

		if( all.finished() ){

			auto system = utility::Task( QString( "%1 -S" ).arg( zuluMountPath ),10000 ) ;

			if( system.finished() ){

				auto a = all.splitOutput( '\n' ) ;
				auto s = system.splitOutput( '\n' ) ;

				for( const auto& it : a ){

					if( _validEntry( it ) ){
						volumeEntryProperties v( utility::split( it,'\t' ) ) ;
						v.setisSystem( s.contains( v.volumeName() ) ) ;
						list.append( v ) ;
					}
				}
			}
		}

		return list ;
	} ) ;
}

void zuluMountTask::checkUnMount( const QString& volume )
{
	utility::Task( QString( "%1 -c -d \"%2\"" ).arg( zuluMountPath ).arg( _device( volume ) ) ) ;
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
				f.open( QIODevice::ReadOnly ) ;
				s = f.readAll() ;
				f.close() ;
				if( s == dev ){
					return false ;
				}
			}
		}
		return true ;
	} ;

	volumeMiniPropertiesTaskResult s = { volume,false,nullptr } ;

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

	auto r = utility::Task( QString( "%1 -L -d \"%2\"" ).arg( zuluMountPath,volume ) ) ;

	if( r.success() ){
		s.entry = new volumeEntryProperties( r.splitOutput( '\t' ) ) ;
		s.entry->setisSystem( _volumeIsSystemVolume( volume ) ) ;
	}

	return s ;
}

volumeMiniPropertiesTaskResult zuluMountTask::deviceProperties( const zuluMountTask::event& deviceProperty )
{
	auto _mdRaidDevice = [&]( const QString& device ){

		auto _mdRaidPath = []( const QString& dev ){
			QString dev_1 ;
			QDir d( "/dev/md/" ) ;
			QDir f ;

			/*
			* wait for a while because things dont always happen as expected if we check too soon.
			*/
			utility::Task::wait( 4 ) ;

			if( d.exists() ){
				QStringList l = d.entryList() ;
				QString e ;
				for( const auto& it : l ){
					dev_1 = QString( "/dev/md/" ) + it ;
					f.setPath( dev_1 ) ;
					if( f.canonicalPath() == dev ){
						return dev_1 ;
					}
				}
			}
			return dev ;
		} ;

		QString d = _mdRaidPath( device ) ;

		volumeMiniPropertiesTaskResult s = { d,false,nullptr } ;

		if( deviceProperty.added ){
			s.entry = new volumeEntryProperties( _getVolumeProperties( d ) ) ;
		}else{
			s.volumeRemoved = true ;
		}

		return s ;
	} ;

	auto _dmDevice = [&]( const QString& device ){

		auto _convertLVM = []( const QString& volume ){

			QByteArray e = volume.toLatin1() ;

			char * begin = e.data() ;
			char * end = begin + e.size() ;

			char * it = begin + 3 ;

			char * k ;
			char * c ;
			char * d ;

			QString z ;

			while( it < end ){

				k = it - 2 ;
				c = it - 1 ;
				d = it ;
				it++ ;

				if( *k != '-' && *c == '-' && *d != '-' ){
					*c = '/' ;
					z = e ;
					while( z.contains( "--" ) ){
						z = z.replace( "--","-" ) ;
					}

					return z ;
				}
			}

			return volume ;
		} ;

		QString d = _convertLVM( device ) ;

		volumeMiniPropertiesTaskResult s = { d,false,nullptr } ;

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

		volumeMiniPropertiesTaskResult s = { "",false,nullptr } ;

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
		volumeMiniPropertiesTaskResult s = { "",false,nullptr } ;
		return s ;
	} ;

	QString device = QString( "/dev/%1" ).arg( deviceProperty.volumeName ) ;

	switch( deviceProperty.deviceType ){
		case zuluMountTask::device    : return _normalDevice( device ) ;
		case zuluMountTask::md_device : return _mdRaidDevice( device ) ;
		case zuluMountTask::dm_device : return _dmDevice( device )     ;
		default                       : return _shouldNotGetHere() ;
	}
}

Task::future<bool>& zuluMountTask::encfsUnmount( const QString& m )
{
	return Task::run< bool >( [ m ](){

		auto _umount = [ & ](){

			if( utility::Task( "/usr/bin/fusermount -u " + m ).exitCode() == 0 ){

				QDir d ;
				d.rmdir( m ) ;

				return true ;
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

			if( utility::pathExists( m ) ){

				return _mount() ;
			}else{
				QDir d ;

				if( d.mkpath( m ) ){

					if( _mount() ){

						return true ;
					}else{
						d.rmdir( m ) ;

						return false ;
					}
				}else{
					return false ;
				}
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

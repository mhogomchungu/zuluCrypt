/*
 *
 *  Copyright (c) 2014
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

#include "bin_path.h"
#include "zulumounttask.h"
#include "../zuluCrypt-gui/utility.h"

#include <QDir>
#include <QString>
#include <QDebug>

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

volumeEntryProperties zuluMount::Task::getVolumeProperties( const QString& e )
{
	QString device = _device( e ) ;
	volumeEntryProperties v ;

	auto r = utility::Task( QString( "%1 -L -d \"%2\"" ).arg( zuluMountPath ).arg( device ) ) ;
	if( r.success() ) {
		volumeEntryProperties v( r.splitOutput( '\t' ) ) ;
		v.setisSystem( _volumeIsSystemVolume( device ) ) ;
		return v ;
	}else{
		return volumeEntryProperties() ;
	}
}

QString zuluMount::Task::volumeProperties( const QString& v,const QString& volumeType )
{
	if( v.isEmpty() ){
		return QString() ;
	}

	QString volume = _device( v ) ;

	auto r = utility::Task( QString( "%1 -s -d \"%2\"" ).arg( zuluMountPath ).arg( volume ) ) ;

	if( r.ok() ){
		return r.output() ;
	}else{
		if( volumeType.contains( "crypto_PLAIN\n" ) ){
			/*
			 * this could be a plain volume opened with an offset
			 */
			r = utility::Task( QString( "%1 -s -o bogusNecessaryArgument -d \"%2\"" ).arg( zuluMountPath ).arg( volume ) ) ;
			if( r.ok() ){
				return r.output() ;
			}else{
				return QString() ;
			}
		}else{
			return QString() ;
		}
	}
}

zuluMountTaskResult zuluMount::Task::unmountVolume( const QString& volumePath,const QString& volumeType )
{
	auto _run = [&]( const QString& exe ){

		zuluMountTaskResult r ;

		auto e = utility::Task( exe ) ;

		r.passed = e.exitCode() == 0 ;
		QString output = e.output() ;
		int index = output.indexOf( QChar( ':' ) ) ;
		r.outPut = output.mid( index + 1 ) ;
		return r ;
	} ;

	QString volume = _device( volumePath ) ;

	auto r = _run( QString( "%1 -u -d \"%2\"" ).arg( zuluMountPath ).arg( volume ) ) ;

	if( !r.passed ){
		if( volumeType.contains( "crypto_PLAIN\n" ) ){
			/*
			 * we could be trying to unmount a volume with an offset
			 */
			r = _run( QString( "%1 -o bogusNecessaryArgument -u -d \"%2\"" ).arg( zuluMountPath ).arg( volume ) ) ;
		}
	}

	return r ;
}

QVector< volumeEntryProperties > zuluMount::Task::updateVolumeList()
{
	auto _validEntry = []( const QString& e ){
		if( e.startsWith( "/dev/md/md-device-map" ) ){
			return false ;
		}
		if( e.contains( "\tswap\t") || e.contains( "member\t" ) || e.contains( "\t/run/media/public" ) ){
			return false ;
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
}

void zuluMount::Task::checkUnMount( const QString& volume )
{
	utility::Task( QString( "%1 -c -d \"%2\"" ).arg( zuluMountPath ).arg( _device( volume ) ) ) ;
}

volumeMiniPropertiesResult zuluMount::Task::volumeMiniProperties( const QString& volume )
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

	volumeMiniPropertiesResult s = { volume,false,nullptr } ;

	if( !volume.startsWith( "UUID" ) && !volume.startsWith( "/dev/" ) ){
		/*
		 * There is some sort of a race condition here and things do not always work as expected
		 * try to sleep for a second to see if it will help
		 */
		utility::Task::wait( 1 ) ;
		if( _loopDeviceIsGone( volume ) ){
			/*
			 * we were just asked to find properties of a loop device
			 * that no longer exists,remove it from the list in the GUI window
			 */
			s.volumeRemoved = true ;
			return s ;
		}
	}

	auto r = utility::Task( QString( "%1 -L -d \"%2\"" ).arg( zuluMountPath ).arg( volume ) ) ;

	if( r.success() ){
		s.entry = new volumeEntryProperties( r.splitOutput( '\t' ) ) ;
		s.entry->setisSystem( _volumeIsSystemVolume( volume ) ) ;
	}

	return s ;
}

volumeMiniPropertiesResult zuluMount::Task::deviceProperties( const zuluMount::deviceProperties& deviceProperty )
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

		volumeMiniPropertiesResult s = { d,false,nullptr } ;

		if( deviceProperty.added ){
			s.entry = new volumeEntryProperties ;
			*s.entry = zuluMount::Task::getVolumeProperties( d ) ;
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

		volumeMiniPropertiesResult s = { d,false,nullptr } ;

		if( deviceProperty.added ){
			s.entry = new volumeEntryProperties ;
			*s.entry = zuluMount::Task::getVolumeProperties( d ) ;
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

		volumeMiniPropertiesResult s = { "",false,nullptr } ;

		if( _allowed_device( device ) ){

			s.volumeName = device ;
			if( deviceProperty.added ){
				s.entry = new volumeEntryProperties ;
				*s.entry = zuluMount::Task::getVolumeProperties( device ) ;
			}else{
				s.volumeRemoved = true ;
			}
		}

		return s ;
	} ;

	auto _shouldNotGetHere = [](){
		volumeMiniPropertiesResult s = { "",false,nullptr } ;
		return s ;
	} ;

	QString device = QString( "/dev/%1" ).arg( deviceProperty.volumeName ) ;

	switch( deviceProperty.deviceType ){
		case zuluMount::device    : return _normalDevice( device ) ;
		case zuluMount::md_device : return _mdRaidDevice( device ) ;
		case zuluMount::dm_device : return _dmDevice( device )     ;
		default                   : return _shouldNotGetHere() ;
	}
}

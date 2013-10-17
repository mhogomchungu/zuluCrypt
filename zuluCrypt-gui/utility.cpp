/*
 *
 *  Copyright ( c ) 2011
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "utility.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <QObject>
#include <QDir>
#include <pwd.h>

#include <QDebug>

#include <blkid/blkid.h>
#include <QByteArray>
#include <QProcess>
#include <QFile>
#include <QFile>
#include <QDir>
#include <QTableWidget>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QProcessEnvironment>
#include <unistd.h>
#include <pwd.h>

#include "../zuluCrypt-cli/constants.h"
#include "../zuluCrypt-cli/bin/bash_special_chars.h"
#include "version.h"
#include "locale_path.h"
#include "mount_prefix_path.h"
#include "storage_manager.h"
#include "dialogmsg.h"

QString utility::cryptMapperPath()
{
	//return QString( crypt_get_dir() )
	return QString( "/dev/mapper/" ) ;
}

bool utility::userIsRoot()
{
	return getuid() == 0 ;
}

QString utility::userName()
{
	struct passwd * pass = getpwuid( getuid() ) ;
	return QString( pass->pw_name ) ;
}

void utility::help( const QString& app )
{
	std::cout << VERSION_STRING << std::endl ;

	QString helpMsg = QObject::tr( "\n\
options:\n\
	-d   path to where a volume to be auto unlocked/mounted is located\n\
	-m   tool to use to open a default file manager(default tool is xdg-open)\n" ) ;

	if( app == QString( "zuluMount" ) ){
		  helpMsg += QObject::tr( "\
	-e   start the application without showing the GUI\n" ) ;
	}

	QByteArray s = helpMsg.toLatin1() ;

	std::cout << s.constData() << std::endl ;
}

QString utility::shareMountPointToolTip()
{
	QString s = QObject::tr( "\
if the option is checked,a primary private mount point will be created in \"/run/media/private/$USER/\"\n\
and a secondary publicly accessible \"mirror\" mount point will be created in \"/run/media/public/\"" ) ;
	return s ;
}

QString utility::shareMountPointToolTip( const QString& path )
{
	struct stat st ;
	QString s = QString( "/run/media/public/" ) + path.split( "/" ).last() ;
	QByteArray x = s.toLatin1() ;
	const char * y = x.constData() ;
	if( stat( y,&st ) == 0 ){
		return QString( "public mount point: " ) + s ;
	}else{
		//return QString( "no public mount point" ) ;
		return QString( "" ) ;
	}
}

QString utility::sharedMountPointPath( const QString& path )
{
	if( path == QString( "/" ) ){
		return QString( "" ) ;
	}else{
		struct stat st ;
		QString s = QString( "/run/media/public/" ) + path.split( "/" ).last() ;
		QByteArray x = s.toLatin1() ;
		const char * y = x.constData() ;
		if( stat( y,&st ) == 0 ){
			return s ;
		}else{
			return QString( "" ) ;
		}
	}
}

bool utility::pathPointsToAFile( const QString& path )
{
	struct stat st ;
	QByteArray b = path.toLatin1() ;
	if( stat( b.constData(),&st ) == 0 ){
		return S_ISREG( st.st_mode ) != 0 ;
	}else{
		return false ;
	}
}

QString utility::localizationLanguage( const QString& program )
{
	QString langPath = utility::localizationLanguagePath( program ) ;
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment() ;
	QString value = env.value( QString( "LANG" ) ) ;

	QString ext = QString( ".qm" ) ;
	QString s ;
	int index ;
	if( !value.isEmpty() ){
		QStringList values = value.split( ":" ) ;

		QDir dir( langPath ) ;

		QStringList dirList = dir.entryList() ;
		dirList.removeOne( QString( "." ) ) ;
		dirList.removeOne( QString( ".." ) ) ;
		int j = values.size() ;

		for( int i = 0 ; i < j ; i++ ){
			s = values.at( i ) ;
			index = s.indexOf( "." ) ;
			if( index != -1 ){
				s.truncate( index ) ;
			}

			if( dirList.contains( s + ext ) ){
				return s ;
			}
		}
	}

	value = env.value( QString( "LANGUAGE" ) ) ;

	if( !value.isEmpty() ){
		QStringList values = value.split( ":" ) ;

		QDir dir( langPath ) ;

		QStringList dirList = dir.entryList() ;
		dirList.removeOne( QString( "." ) ) ;
		dirList.removeOne( QString( ".." ) ) ;
		int j = values.size() ;
		for( int i = 0 ; i < j ; i++ ){
			s = values.at( i ) ;
			index = s.indexOf( "." ) ;
			if( index != -1 ){
				s.truncate( index ) ;
			}
			if( dirList.contains( s + ext ) ){
				return s ;
			}
		}
	}

	return QString( "en_US" ) ;
}

QString utility::localizationLanguagePath( const QString& program )
{
	return QString( TRANSLATION_PATH ) + program ;
}

void utility::setLocalizationLanguage( const QString& program,const QString& language )
{
	Q_UNUSED( program ) ;
	Q_UNUSED( language ) ;
}

QString utility::walletName()
{
	return "zuluCrypt" ;
}

QString utility::applicationName()
{
	return "zuluCrypt" ;
}

bool utility::pathIsReadable( const QString& path )
{
	int fd = open( path.toLatin1().constData(),O_RDONLY ) ;
	if( fd != -1 ){
		close( fd ) ;
		return true ;
	}else{
		return false ;
	}
}

bool utility::setOpenVolumeReadOnly( QWidget * parent,bool check,const QString& app )
{
	QString Path = QDir::homePath() + QString( "/.zuluCrypt/" ) + app ;
	QFile f( Path + QString( "-openMode" ) ) ;

	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	if( check ){
		f.write( "1" ) ;
	}else{
		f.write( "0" ) ;
	}

	f.close() ;

	DialogMsg msg( parent ) ;
	QString m = QObject::tr( "setting this option will cause the volume to open in read only mode" ) ;

	QString path = Path + QString( "-readOnlyOption" ) ;

	f.setFileName( path ) ;

	bool st ;
	if( f.exists() ){
		f.open( QIODevice::ReadWrite ) ;
		QByteArray opt = f.readAll() ;
		if( opt == QByteArray( "0" ) && check ) {
			f.seek( 0 ) ;

			st = msg.ShowUIOKDoNotShowOption( QObject::tr( "info" ),m ) ;

			if( st ){
				f.write( "1" ) ;
			}else{
				f.write( "0" ) ;
			}

			f.close() ;
		}
	}else{
		st = msg.ShowUIOKDoNotShowOption( QObject::tr( "info" ),m ) ;

		f.open( QIODevice::WriteOnly ) ;

		if( st ){
			f.write( "1" ) ;
		}else{
			f.write( "0" ) ;
		}

		f.close() ;
	}

	return check ;
}

bool utility::getOpenVolumeReadOnlyOption( const QString& app )
{
	QString home = QDir::homePath() + QString( "/.zuluCrypt/" ) ;
	QDir d( home ) ;

	if( d.exists() ){
		;
	}else{
		d.mkdir( home ) ;
	}

	QFile f( home + app + QString( "-openMode" ) ) ;

	if( f.exists() ){
		;
	}else{
		f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
		f.write( "0" ) ;
		f.close() ;
	}

	f.open( QIODevice::ReadOnly ) ;
	int st = QString( f.readAll() ).toInt() ;

	f.close() ;
	return st == 1 ;
}

void utility::debug( const QString& s )
{
	std::cout << s.toStdString() << std::endl ;
}

void utility::debug( int s )
{
	std::cout << s << std::endl ;
}

bool utility::mapperPathExists( const QString& path )
{
	return utility::exists( utility::mapperPath( path ) ) ;
}

QString utility::mountPath( const QString& path )
{
	struct passwd * pass = getpwuid( getuid() ) ;

#if USE_HOME_PATH_AS_MOUNT_PREFIX
	return QString( "%1/%2" ).arg( QString( pass->pw_dir ) ).arg( path ) ;
#else
	return QString( "/run/media/private/%1/%2" ).arg( QString( pass->pw_dir ).split( "/" ).last() ).arg( path ) ;
#endif
}

QString utility::mapperPath( const QString& r )
{
	QString rpath = r ;

	QString path = utility::cryptMapperPath() + QString( "zuluCrypt-" ) + QString::number( getuid() ) ;

	if( rpath.startsWith( QString( "UUID=" ) ) ){
		rpath.remove( QChar( '\"' ) ) ;
		rpath.replace( QString( "UUID=" ),QString( "UUID-" ) ) ;
		path += QString( "-" ) + rpath + utility::hashPath( rpath.toLatin1() ) ;
	}else{
		path += QString( "-NAAN-" ) + rpath.split( "/" ).last() + utility::hashPath( rpath.toLatin1() ) ;
	}

	QString z = QString( BASH_SPECIAL_CHARS ) ;

	int g = z.size() ;

	for( int i = 0 ; i < g ; i++ ){
		path.replace( z.at( i ),QChar( '_' ) ) ;
	}
	return path ;
}

QString utility::hashPath( const QByteArray& p )
{
	size_t l = p.size() ;
	uint32_t hash ;
	uint32_t i ;
	const char * key = p.constData() ;
	i = hash = 0 ;
	for( ; i < l ; i++ ){
		hash += key[ i ] ;
		hash += ( hash << 10 ) ;
		hash ^= ( hash >> 6 ) ;
	}
	hash += ( hash << 3 ) ;
	hash ^= ( hash >> 11 ) ;
	hash += ( hash << 15 ) ;
	return QString( "-" ) + QString::number( hash ) ;
}

bool utility::exists( const QString& path )
{
	struct stat st ;
	return stat( path.toLatin1().data(),&st ) == 0 ;
}

bool utility::canCreateFile( const QString& path )
{
	QByteArray q = path.toLatin1() ;

	int i = open( q.constData(),O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) ;

	if( i == -1 ){
		return false ;
	}else{
		close( i ) ;
		remove( q ) ;
		return true ;
	}
}

QString utility::resolvePath( const QString& path )
{
	if( path.size() == 1 && path.at( 0 ) == QChar( '~' ) ){
		return QDir::homePath() + QString( "/" ) ;
	}else if( path.mid( 0,2 ) == QString( "~/" ) ){
		return QDir::homePath() + QString( "/" ) + path.mid( 2 ) ;
	}else if( path.mid( 0,5 ) == QString( "UUID=") ){
		return path ;
	}else if( path.startsWith( QString( "/dev/") ) ){
		return path ;
	}else{
		QDir r( path ) ;
		QString rp = r.canonicalPath() ;
		if( rp.isEmpty() ) {
			return path ;
		}else{
			return rp ;
		}
	}
}

QStringList utility::luksEmptySlots( const QString& volumePath )
{
	QStringList list ;
	QProcess N ;
	N.start( QString( ZULUCRYPTzuluCrypt ) + QString( " -b -d \"" ) + volumePath + QString( "\"" ) ) ;
	N.waitForFinished() ;
	if( N.exitCode() != 0 ){
		return list ;
	}
	QByteArray s = N.readAll() ;
	N.close() ;
	int i = 0 ;
	int z = s.size() ;
	for( int j = 0 ; j < z ; j++ ){
		if( s.at( j ) == '1' || s.at( j ) == '3' ){
			i++ ;
		}
	}
	list << QString::number( i ) ;
	list << QString::number( z - 1 ) ;
	return list ;
}

void utility::addToFavorite( const QString& dev,const QString& m_point )
{
	QString fav = QString( "%1\t%2\n" ).arg( dev ).arg( m_point ) ;
	QFile f( QDir::homePath() + QString( "/.zuluCrypt/favorites" ) ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Append ) ;
	f.write( fav.toLatin1() ) ;
	f.close() ;
}

QStringList utility::readFavorites()
{
	QFile f( QDir::homePath() + QString( "/.zuluCrypt/favorites" ) ) ;
	QStringList list ;
	if( f.open( QIODevice::ReadOnly ) ){
		QString data( f.readAll() ) ;
		f.close() ;
		if( !data.isEmpty() ){
			list = data.split( "\n" ) ;
		}
	}
	return list ;
}

void utility::removeFavoriteEntry( const QString& entry )
{
	QFile f( QDir::homePath() + QString( "/.zuluCrypt/favorites" ) ) ;
	f.open( QIODevice::ReadOnly ) ;
	QByteArray b = f.readAll() ;
	f.close() ;
	QByteArray c = b.remove( b.indexOf( entry ),entry.length() ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( c ) ;
	f.close() ;
}

QString utility::getUUIDFromPath( const QString& dev )
{
	QString device = dev ;
	device = device.replace( QString( "\"" ),QString( "\"\"\"" ) ) ;
	QString exe = QString( "%1 -U -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( device ) ;
	QProcess p ;
	p.start( exe ) ;
	p.waitForFinished() ;
	QString uuid ;
	if( p.exitCode() == 0 ){
		uuid = QString( p.readAll() ) ;
		uuid.remove( QString( "\n" ) ) ;
	}
	p.close() ;
	return uuid ;
}

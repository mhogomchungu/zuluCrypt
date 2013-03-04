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

//#include <libcryptsetup.h>

QString utility::cryptMapperPath()
{
	//return QString( crypt_get_dir() )
	return QString( "/dev/mapper/" ) ;
}

bool utility::userIsRoot()
{
	return getuid() == 0 ? true : false ;
}

QString utility::userName()
{
	struct passwd * pass = getpwuid( getuid() ) ;
	return QString( pass->pw_name ) ;
}

void utility::debug( QString s )
{
	std::cout << s.toStdString() << std::endl ;
}

void utility::debug( int s )
{
	std::cout << s << std::endl ;
}

bool utility::mapperPathExists( QString path )
{
	return utility::exists( utility::mapperPath( path ) ) ;
}

QString utility::mountPath( QString name )
{
	struct passwd * pass = getpwuid( getuid() ) ;
	return QString( "/run/media/%1/%2" ).arg( QString( pass->pw_dir ).split( "/" ).last() ).arg( name );
}

QString utility::mapperPath( QString rpath )
{
	QString path = utility::cryptMapperPath() + QString( "zuluCrypt-" ) + QString::number( getuid() ) ;

	if( rpath.startsWith( QString( "UUID=" ) ) ){
		rpath.remove( QChar( '\"' ) ) ;
		rpath.replace( QString( "UUID=" ),QString( "UUID-" ) ) ;
		path += QString( "-" ) + rpath + utility::hashPath( rpath );
	}else{
		path += QString( "-NAAN-" ) + rpath.split( "/" ).last() + utility::hashPath( rpath );
	}

	QString z = QString( BASH_SPECIAL_CHARS );

	int g = z.size() ;

	for( int i = 0 ; i < g ; i++ ){
		path.replace( z.at( i ),QChar( '_' ) );
	}
	return path ;
}

QString utility::hashPath( QString p )
{
	size_t i = 0 ;
	size_t l = p.size() ;
	double h = 0 ;

	for ( i = 0 ; i < l ; i++ ){
		h = h + p.at( i ).toAscii() ;
	}
	return QString( "-" ) + QString::number( h );
}

bool utility::exists( QString path )
{
	struct stat st ;
	return stat( path.toAscii().data(),&st ) == 0 ? true : false ;
}

bool utility::canCreateFile( QString path )
{
	QByteArray q = path.toAscii() ;

	int i = open( q.constData(),O_WRONLY|O_CREAT ) ;

	if( i == -1 ){
		return false ;
	}else{
		close( i ) ;
		remove( q );
		return true ;
	}
}

QString utility::resolvePath( QString path )
{
	if( path.size() == 1 && path.at( 0 ) == QChar( '~' ) ){
		return QDir::homePath() + QString( "/" );
	}else if( path.mid( 0,2 ) == QString( "~/" ) ){
		return QDir::homePath() + QString( "/" ) + path.mid( 2 );
	}else if( path.mid( 0,5 ) == QString( "UUID=") ){
		return path ;
	}else if( path.startsWith( QString( "/dev/") ) ){
		return path ;
	}else{
		QDir r( path ) ;
		QString rp = r.canonicalPath() ;
		return rp.isEmpty() == false ? rp : path ;
	}
}

QStringList utility::deviceProperties( QString device )
{
	int64_t i ;
	const char * buffer ;
	QStringList prp ;
	QString size ;
	prp << device ;

	QByteArray dev = device.toAscii() ;
	blkid_probe dp = blkid_new_probe_from_filename( dev.constData() ) ;
	blkid_do_probe( dp ) ;

	i = blkid_probe_get_size( dp ) ;

	if( i >= 0 ){
		size = QString::number( i ) ;
		switch( size.length() ){
			case 0:
			case 1:
			case 2:
			case 3:size + QString( " B" );break;
			case 4:size = size.mid( 0,1 ) + QString( " KB" );break;
			case 5:size = size.mid( 0,2 ) + QString( " KB" );break;
			case 6:size = size.mid( 0,3 ) + QString( " KB" );break;
			case 7:size = size.mid( 0,1 ) + QString( " MB" );break;
			case 8:size = size.mid( 0,2 ) + QString( " MB" );break;
			case 9:size = size.mid( 0,3 ) + QString( " MB" );break;
			case 10:size = size.mid( 0,1 ) + QString( " GB" );break;
			case 11:size = size.mid( 0,2 ) + QString( " GB" );break;
			case 12:size = size.mid( 0,3 ) + QString( " GB" );break ;
			case 13:size = size.mid( 0,1 ) + QString( " TB" );break;
			case 14:size = size.mid( 0,2 ) + QString( " TB" );break;
			case 15:size = size.mid( 0,3 ) + QString( " TB" );break ;
		}
	}else{
		size = QString( "Nil" );
	}

	prp << size ;

	i = blkid_probe_lookup_value( dp,"LABEL",&buffer,NULL );

	if( i == 0 ){
		prp << QString( buffer ) ;
	}else{
		prp << QString( "Nil" ) ;
	}
	i = blkid_probe_lookup_value( dp,"TYPE",&buffer,NULL );

	if( i == 0 ){
		prp << QString( buffer ) ;
	}else{
		prp << QString( "Nil" ) ;
	}
	i = blkid_probe_lookup_value( dp,"UUID",&buffer,NULL );

	if( i == 0 ){
		prp << QString( buffer ) ;
	}else{
		prp << QString( "Nil" ) ;
	}
	blkid_free_probe( dp ) ;

	return prp ;
}

bool utility::isLuks( QString volumePath )
{
	QProcess p ;
	QString path = utility::resolvePath( volumePath );
	QString exe = QString( "%1 -i -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( path );
	p.start( exe );
	p.waitForFinished() ;
	int i = p.exitCode() ;
	p.close();

	return i == 0 ;
}

QStringList utility::luksEmptySlots( QString volumePath )
{
	QStringList list ;
	QProcess N ;
	N.start( QString( ZULUCRYPTzuluCrypt ) + QString( " -b -d \"" ) + volumePath + QString( "\"" ) );
	N.waitForFinished() ;
	if( N.exitCode() != 0 ){
		return list ;
	}
	QByteArray s = N.readAllStandardOutput() ;
	N.close();
	int i = 0 ;
	for ( int j = 0 ; j < s.size() ; j++ ){
		if( s.at( j ) == '1' || s.at( j ) == '3' ){
			i++ ;
		}
	}
	list << QString::number( i ) ;
	list << QString::number(  s.size() - 1 ) ;
	return list ;
}

void utility::addToFavorite( QString dev,QString m_point )
{
	QString fav = QString( "%1\t%2\n" ).arg( dev ).arg( m_point ) ;
	QFile f( QDir::homePath() + QString( "/.zuluCrypt/favorites" ) ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Append ) ;
	f.write( fav.toAscii() ) ;
	f.close();
}

QStringList utility::readFavorites()
{
	QFile f( QDir::homePath() + QString( "/.zuluCrypt/favorites" ) ) ;
	QStringList list ;
	if( f.open( QIODevice::ReadOnly ) ){
		QString data( f.readAll() ) ;
		f.close();
		if( !data.isEmpty() ){
			list = data.split( "\n" ) ;
		}
	}
	return list ;
}

void utility::removeFavoriteEntry( QString entry )
{
	QFile f( QDir::homePath() + QString( "/.zuluCrypt/favorites" ) ) ;
	f.open( QIODevice::ReadOnly ) ;
	QByteArray b = f.readAll() ;
	f.close();
	QByteArray c = b.remove( b.indexOf( entry ),entry.length() ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( c ) ;
	f.close() ;
}

QString utility::getUUIDFromPath( QString device )
{
	device = device.replace( QString( "\"" ),QString( "\"\"\"" ) ) ;
	QString exe = QString( "%1 -U -d %2" ).arg( ZULUCRYPTzuluCrypt ).arg( device ) ;
	QProcess p ;
	p.start( exe );
	p.waitForFinished() ;
	QString uuid ;
	if( p.exitCode() == 0 ){
		uuid = QString( p.readAll() ) ;
		uuid.remove( QString( "\n" ) ) ;
	}
	p.close();
	return uuid ;
}

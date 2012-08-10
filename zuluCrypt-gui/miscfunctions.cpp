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

#include "miscfunctions.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <QObject>
#include <QDir>

//#include <libcryptsetup.h>

QString miscfunctions::cryptMapperPath()
{
	//return QString( crypt_get_dir() )
	return QString( "/dev/mapper/" ) ;
}

void miscfunctions::debug( QString s )
{
	std::cout << s.toStdString() << std::endl ;
}

void miscfunctions::debug( int s )
{
	std::cout << s << std::endl ;
}

QString miscfunctions::mapperPath( QString rpath )
{
	QString path = miscfunctions::cryptMapperPath() + QString( "zuluCrypt-" ) + QString::number( getuid() ) ;
	
	path += QString( "-NAAN-" ) + rpath.split( "/" ).last() + miscfunctions::hashPath( rpath );
	
	QString z = QString( BASH_SPECIAL_CHARS );
	
	int g = z.size() ;
	
	for( int i = 0 ; i < g ; i++ )
		path.replace( z.at( i ),QChar( '_' ) );
	
	return path ;
}

QString miscfunctions::hashPath( QString p )
{
	size_t i = 0 ;
	size_t l = p.size() ;
	double h = 0 ;
	
	for ( i = 0 ; i < l ; i++ )
		h = h + p.at( i ).toAscii() ;
	
	return QString( "-" ) + QString::number( h );
}

bool miscfunctions::exists( QString path )
{
	struct stat st ;
	return stat( path.toAscii().data(),&st ) == 0 ? true : false ;
}

bool miscfunctions::canCreateFile( QString path )
{
	char * p = path.toAscii().data() ;
	int i = open( p,O_WRONLY|O_CREAT ) ;
	
	if( i == -1 )
		return false ;
	else{
		close( i ) ;
		remove( p );
		return true ;
	}
}

QString miscfunctions::resolvePath( QString path )
{
	if( path.size() == 1 && path.at( 0 ) == QChar( '~' ) )
		return QDir::homePath() + QString( "/" );
	else if( path.mid( 0,2 ) == QString( "~/" ) )
		return QDir::homePath() + QString( "/" ) + path.mid( 2 );
	else if( path.mid( 0,5 ) == QString( "UUID=") )
		return path ;
	else{
		QDir r( path ) ;
		QString rp = r.canonicalPath() ;
		return rp.isEmpty() == false ? rp : path ;
	}
}

QStringList miscfunctions::deviceProperties( QString device )
{
	int64_t i ;
	const char * buffer ;
	QStringList prp ;
	QString size ;
	prp << device ;
	
	blkid_probe dp = blkid_new_probe_from_filename( device.toAscii().data() ) ;
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
	
	if( i == 0 )
		prp << QString( buffer ) ;
	else
		prp << QString( "Nil" ) ;
	
	i = blkid_probe_lookup_value( dp,"TYPE",&buffer,NULL );
	
	if( i == 0 )
		prp << QString( buffer ) ;
	else
		prp << QString( "Nil" ) ;
	
	i = blkid_probe_lookup_value( dp,"UUID",&buffer,NULL );
	
	if( i == 0 )
		prp << QString( buffer ) ;
	else
		prp << QString( "Nil" ) ;
	
	blkid_free_probe( dp ) ;
	
	return prp ;
}

bool miscfunctions::isLuks( QString volumePath )
{
	QProcess p ;
	QString path = miscfunctions::resolvePath( volumePath );
	QString exe = QString( "%1 -i -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( path );
	p.start( exe );
	p.waitForFinished() ;
	int i = p.exitCode() ;
	p.close();
	
	return i == 0 ? true : false ;
}

QStringList miscfunctions::luksEmptySlots( QString volumePath )
{
	QStringList list ;      
	QProcess N ;
	N.start( QString( ZULUCRYPTzuluCrypt ) + QString( " -b -d \"" ) + volumePath + QString( "\"" ) );
	N.waitForFinished() ;
	if( N.exitCode() != 0 )
		return list ;
	QByteArray s = N.readAllStandardOutput() ;
	N.close();
	int i = 0 ;
	for ( int j = 0 ; j < s.size() ; j++ )
		if( s.at( j ) == '1' || s.at( j ) == '3' )
			i++ ;
		list << QString::number( i ) ;
	list << QString::number(  s.size() - 1 ) ;
	return list ;
}

void miscfunctions::addToFavorite( QString dev,QString m_point )
{
	QString fav = QString( "%1\t%2\n" ).arg( dev ).arg( m_point ) ;
	QFile f( QDir::homePath() + QString( "/.zuluCrypt/favorites" ) ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Append ) ;
	f.write( fav.toAscii() ) ;
	f.close();
}

QStringList miscfunctions::readFavorites()
{
	QFile f( QDir::homePath() + QString( "/.zuluCrypt/favorites" ) ) ;
	
	QStringList list ;
	
	if( f.open( QIODevice::ReadOnly ) ){
		QString data( f.readAll() ) ;
		f.close();
		
		if( data.isEmpty() == false )
			list = data.split( "\n" ) ;
	}
	
	return list ;
}

void miscfunctions::removeFavoriteEntry( QString entry )
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

void miscfunctions::addItemToTable( QTableWidget * table,QString device,QString mountAddr )
{
	QString type ;
	QString path = device ;
	path.replace( "\"","\"\"\"" ) ;
	if( miscfunctions::isLuks( path ) )
		type = QObject::tr( "luks" );
	else
		type = QObject::tr( "plain" );
	
	miscfunctions::addItemToTableWithType( table,device,mountAddr,type );
}

void miscfunctions::addItemToTableWithType( QTableWidget * table,QString device,QString mountAddr,QString type )
{
	int row = table->rowCount() ;
	table->insertRow( row );
	
	QTableWidgetItem * item ;
	
	item = new QTableWidgetItem() ;
	item->setText( device );
	item->setTextAlignment( Qt::AlignCenter );
	table->setItem( row,0,item );
	
	item = new QTableWidgetItem() ;
	item->setText( mountAddr );
	item->setTextAlignment( Qt::AlignCenter );
	table->setItem( row,1,item );
	
	item = new QTableWidgetItem() ;
	item->setText( type );
	item->setTextAlignment( Qt::AlignCenter );
	table->setItem( row,2,item );
	
	table->setCurrentCell( row,1 );
}


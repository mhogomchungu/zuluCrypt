/*
 *
 *  Copyright ( c ) 2012
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

#include "savemountpointpath.h"

#include <QFile>
#include <QDir>

savemountpointpath::savemountpointpath()
{
}

void savemountpointpath::savePath( QString path,QString fileName )
{
	QString p = QDir::homePath() + QString( "/.zuluCrypt/" ) + fileName ;

	QFile f( p ) ;

	f.open( QIODevice::WriteOnly ) ;

	int index = path.lastIndexOf( "/") ;
	if( index != -1 ){
		path = path.mid( 0,index + 1 ) ;
		f.write( path.toAscii() ) ;
	}
	f.close() ;
}

QString savemountpointpath::getPath( QString path,QString fileName )
{
	QString p = QDir::homePath() + QString( "/.zuluCrypt/" ) + fileName ;
	QFile f( p ) ;

	path = path.split( "/" ).last() ;

	QString e ;
	if( f.exists() ){
		f.open( QIODevice::ReadOnly ) ;
		e = QString( f.readAll() ) + path ;
		f.close() ;
	}else{
		e = QDir::homePath() + QString( "/" ) + path ;
	}
	return e ;
}

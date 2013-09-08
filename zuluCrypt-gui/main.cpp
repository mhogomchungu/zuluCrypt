/*
 *
 *  Copyright (c) 2011
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

#include <QIcon>
#include <QtGui/QApplication>
#include "zulucrypt.h"
#include "utility.h"
#include <QCoreApplication>
#include <QStringList>

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;

	QCoreApplication::setApplicationName( QString( "zuluCrypt" ) ) ;

	QStringList q = QCoreApplication::arguments() ;
	if( q.contains( QString( "-h" ) ) ||
	    q.contains( QString( "-help" ) ) ||
	    q.contains( QString( "--help" ) ) ||
	    q.contains( QString( "-v" ) ) ||
	    q.contains( QString( "-version" ) ) ||
	    q.contains( QString( "--version" ) ) ){
		utility::help( QString( "zuluCrypt" ) ) ;
		return 0 ;
	}else{
		zuluCrypt zc ;
		zc.start() ;
		return a.exec() ;
	}
}

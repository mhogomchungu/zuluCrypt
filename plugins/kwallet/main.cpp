/*
 *
 *  Copyright (c) 2012
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


#include <QCoreApplication>
#include "wallet.h"

int main( int argc,char * argv[] )
{
	QCoreApplication a( argc,argv ) ;

	wallet w( argv[ 1 ],argv[ 2 ],argv[ 3 ] ) ;

	QCoreApplication::setApplicationName( "zuluCrypt kwallet plugin" ) ;

	QMetaObject::invokeMethod( &w,"openWallet",Qt::QueuedConnection ) ;

	return a.exec() ;
}

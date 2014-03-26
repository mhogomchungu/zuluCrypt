/*
 *
 *  Copyright (c) 2012
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

#include <QApplication>
#include "../utility/mainwindow.h"
#include <QDebug>
#include <QByteArray>
#include <QString>
#include <QFile>
#include <QObject>

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;

	MainWindow w ;

	w.setAddr( QString( argv[ 3 ] ) ) ;
	w.setkeyLabel( QObject::tr( "enter key below" ) ) ;
	w.setkeyFileLabel( QObject::tr( "enter path to keyfile below" ) ) ;
	w.setButtonIcon( QString( "keyfile" ) ) ;
	
	auto e = []( const QString& exe,const QString& keyFile,const QString& password ){
		QFile f( keyFile ) ;
		f.open( QIODevice::ReadOnly ) ;
		return password.toLatin1() + f.readAll() ;
	} ;

	w.setKeyRoutine( e ) ;
	w.show() ;

	return a.exec() ;
}

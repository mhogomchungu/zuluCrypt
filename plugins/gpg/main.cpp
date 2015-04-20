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

#include <QApplication>
#include "../mainwindow.h"

#include "gpg.h"

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;

	MainWindow w ;

	w.setToken( QString( argv[ 3 ] ) ) ;
	w.setApplicationName( "gpg" ) ;
	w.setkeyLabel( QObject::tr( "Enter GPG Key Below" ) ) ;
	w.setkeyFileLabel( QObject::tr( "Enter A Path To A GPG Keyfile Below" ) ) ;

	QVector<QString> exe ;
	exe.append( "gpg" ) ;

	w.setExe( exe ) ;
	w.setKeyFunction( gpg ) ;

	w.Show() ;

	return a.exec() ;
}

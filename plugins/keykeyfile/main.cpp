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
#include <QDebug>
#include <QByteArray>
#include <QString>
#include <QFile>
#include <QObject>

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;

	MainWindow w( []( const QVector<QString>& exe,const QString& keyFile,const QString& password ){

		Q_UNUSED( exe ) ;

		QFile f( keyFile ) ;

		f.open( QIODevice::ReadOnly ) ;

		return password.toLatin1() + f.readAll() ;
	} ) ;

	w.setToken( argv ) ;
	w.setkeyLabel( QObject::tr( "Enter Key Below" ) ) ;
	w.setkeyFileLabel( QObject::tr( "Enter A Path To A Keyfile Below" ) ) ;
	w.setButtonIcon( "Keyfile" ) ;

	w.Show() ;

	return a.exec() ;
}

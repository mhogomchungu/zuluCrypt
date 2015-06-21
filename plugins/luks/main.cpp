/*
 *
 *  Copyright (c) 2012-2015
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

	MainWindow w ;

	w.setToken( argv[ 3 ] ) ;
	w.setApplicationName( "luks" ) ;
	w.setkeyLabel( QObject::tr( " Enter LUKS Key Below" ) ) ;
	w.setkeyFileLabel( QObject::tr( "Enter A Path To A LUKS Header Below" ) ) ;

	auto e = []( const QVector<QString>& exe,const QString& keyFile,const QString& password ){
		Q_UNUSED( exe ) ;
		/*
		 * we are sending a 4 component structure.
		 * first  component at offset 0 is a u_int32_t structure holding the size of the passphrase
		 * Second component at offset 4 is a u_int32_t structure holding the size of the contents of luks header
		 * third  component at offset 8 is the passphrase to unlock the LUKS volume.
		 * last   component is at offset that marks the end of the third component.Where this offset will be depends on the length of the passphrase
		 */

		auto intToByteArray = []( quint32 s ){

			const char * e = reinterpret_cast< const char * >( &s ) ;
			return QByteArray( e,sizeof( quint32 ) ) ;
		} ;

		QFile keyfile( keyFile ) ;

		QByteArray keyFileSize  = intToByteArray( keyfile.size() ) ;
		QByteArray passWordSize = intToByteArray( password.size() ) ;

		keyfile.open( QIODevice::ReadOnly ) ;

		return passWordSize + keyFileSize + password.toLatin1() + keyfile.readAll() ;
	} ;

	w.setKeyFunction( e ) ;
	w.Show() ;

	return a.exec() ;
}

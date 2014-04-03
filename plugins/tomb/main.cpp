/*
 *
 *  Copyright (c) 2014
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
#include <QProcess>
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <QObject>
#include <QFile>
#include <QDir>

#include <sys/types.h>
#include <unistd.h>

#include "../gpg/gpg.h"

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;

	MainWindow w ;

	w.setToken( QString( argv[ 3 ] ) ) ;
	w.setApplicationName( QString( "steghide" ) ) ;
	w.setkeyLabel( QObject::tr( "enter tomb/steghide key below" ) ) ;
	w.setkeyFileLabel( QObject::tr( "enter a path to a tomb/steghide keyfile below" ) ) ;

	auto steghide = []( const QString& exe,const QString& keyFile,const QString& password ){
		QProcess p ;
		/*
		 * TODO: look into passing the passphrase more securely
		 */
		QString arg = QString( "%1 --extract -sf %2 -xf - -p %3" ).arg( exe ).arg( keyFile ).arg( password ) ;
		p.start( arg ) ;
		p.waitForFinished( -1 ) ;
		QByteArray key = p.readAllStandardOutput() ;
		p.close() ;
		if( key.isEmpty() ){
			return key ;
		}else{
			key = "\n-----BEGIN PGP MESSAGE-----\n\n" + key + "-----END PGP MESSAGE-----\n" ;

			QString temp_path = QString( "%1/%2/.tomb-%3" ).arg( QDir::homePath() ).arg( "/.zuluCrypt/" ).arg( QString::number( getpid() ) ) ;

			QFile temp_file( temp_path ) ;

			temp_file.open( QIODevice::WriteOnly ) ;
			temp_file.write( key ) ;
			temp_file.close() ;
			/*
			 * gpg() is defined in ../gpg/gpg.h
			 */
			key = gpg( QString(),temp_path,password ) ;

			temp_file.setFileName( temp_path ) ;
			temp_file.open( QIODevice::WriteOnly ) ;
			uchar * m = temp_file.map( 0,temp_file.size() ) ;
			if( m ){
				memset( m,'\0',temp_file.size() ) ;
				temp_file.unmap( m ) ;
			}
			temp_file.close() ;
			temp_file.remove() ;
			return key ;
		}
	} ;

	w.setKeyFunction( steghide ) ;
	w.Show() ;

	return a.exec() ;
}

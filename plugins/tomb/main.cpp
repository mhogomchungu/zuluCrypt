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
#include <QFile>

#include <sys/types.h>
#include <unistd.h>

#include "../gpg/gpg.h"

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;

	MainWindow w ;

	w.setToken( QString( argv[ 3 ] ) ) ;
	w.setApplicationName( QString( "tomb" ) ) ;
	w.setkeyLabel( QObject::tr( "enter tomb/steghide key below" ) ) ;
	w.setkeyFileLabel( QObject::tr( "enter a path to a tomb/steghide keyfile below" ) ) ;

	auto tomb = []( const QVector<QString>& exe,const QString& keyFile,const QString& password ){
		/*
		 * TODO: look into passing the passphrase more securely
		 */
		QString arg = QString( "%1 --extract -sf %2 -xf - -p %3" ).arg( exe.first() ).arg( keyFile ).arg( password ) ;
		QProcess p ;
		p.start( arg ) ;
		p.waitForFinished( -1 ) ;
		QByteArray key = p.readAllStandardOutput() ;
		p.close() ;
		if( key.isEmpty() ){
			return key ;
		}else{
			key = "\n-----BEGIN PGP MESSAGE-----\n\n" + key + "-----END PGP MESSAGE-----\n" ;

			QString temp_path = QString( "%1/%2/" ).arg( QDir::homePath() ).arg( "/.zuluCrypt/" ) ;

			QDir d ;
			d.mkpath( temp_path ) ;

			temp_path += QString( ".tomb-%1" ).arg( QString::number( getpid() ) ) ;
			/*
			 * temp_path will have something like "/home/ink/.zuluCrypt/.tomb-3452"
			 * this will be a path to a temporary file we will pass to gpg since gpg expects a keyfile path
			 * TODO: look into skipping creating a temporary file and do everything in memory
			 */
			QFile temp_file( temp_path ) ;

			temp_file.open( QIODevice::WriteOnly ) ;
			temp_file.write( key ) ;
			temp_file.close() ;

			QVector<QString> e ;
			e.append( exe.at( 1 ) ) ;
			/*
			 * gpg() is defined in ../gpg/gpg.h
			 */
			key = gpg( e,temp_path,password ) ;

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

	w.setKeyFunction( tomb ) ;

	QVector<QString> exe ;
	exe.append( "steghide" ) ;
	exe.append( "gpg" ) ;

	w.setExe( exe ) ;
	w.Show() ;

	return a.exec() ;
}

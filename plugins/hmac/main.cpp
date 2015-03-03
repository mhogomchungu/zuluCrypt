/*
 *
 *  Copyright (c) 2015
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
#include <QByteArray>
#include <QFile>
#include <QDebug>

#include <functional>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gcrypt.h>
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

static QByteArray _getKey( gcry_md_hd_t handle,const QString& keyFile )
{
	QFile f( keyFile ) ;

	if( !f.open( QIODevice::ReadOnly ) ){

		return QByteArray() ;
	}else{
		const int size = 1024 ;
		char buffer[ size ] ;

		qint64 e ;

		while( true ){

			e = f.read( buffer,size ) ;

			if( e <= 0 ){
				break ;
			}else{
				gcry_md_write( handle,buffer,e ) ;
			}
		}

		auto key = ( const char * )gcry_md_read( handle,0 ) ;
		auto x   = ( int )gcry_md_get_algo_dlen( GCRY_MD_SHA256 ) ;

		QByteArray r( key,x ) ;

		return r.toHex() ;
	}
}

QByteArray hmac( const QVector<QString>& exe,const QString& keyFile,const QString& password )
{
	Q_UNUSED( exe ) ;

	QByteArray key ;
	gcry_md_hd_t handle ;

	gcry_error_t r = gcry_md_open( &handle,GCRY_MD_SHA256,GCRY_MD_FLAG_HMAC ) ;

	if( r == GPG_ERR_NO_ERROR ){

		r = gcry_md_setkey( handle,password.toLatin1().constData(),password.size() ) ;

		if( r == GPG_ERR_NO_ERROR ){

			key = _getKey( handle,keyFile ) ;
		}

		gcry_md_close( handle ) ;
	}

	return key ;
}

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;

	if( gcry_control( GCRYCTL_INITIALIZATION_FINISHED_P ) == 0 ){
		gcry_check_version( NULL ) ;
		gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;
	}

	MainWindow w ;

	w.setToken( argv[ 3 ] ) ;
	w.setApplicationName( "hmac" ) ;
	w.setkeyLabel( QObject::tr( "enter a password below" ) ) ;
	w.setkeyFileLabel( QObject::tr( "enter a path to a keyfile below" ) ) ;

	w.setKeyFunction( hmac ) ;

	w.Show() ;

	return a.exec() ;
}

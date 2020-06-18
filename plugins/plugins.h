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
#ifndef PLUGINS_H
#define PLUGINS_H

#include <QProcess>
#include <QByteArray>
#include <QString>
#include <QObject>
#include <QFile>
#include <QVector>
#include <QDebug>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gcrypt.h>
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

namespace plugins
{

enum class plugin{ gpg,hmac_key,hmac_key_1,keyKeyFile,luks,steghide } ;

static inline QByteArray gpg( const QVector<QString>& exe,const QString& keyFile,const QString& password )
{
	QProcess p ;

	if( password.isEmpty() ){

		p.start( exe.first(),{ "--no-tty","--yes","--no-mdc-warning","--no-verbose","-d",keyFile } ) ;
	}else{
		p.start( exe.first(),{ "--no-tty","--yes","--no-mdc-warning","--no-verbose","--passphrase-fd","0","-d",keyFile } ) ;
	}

	p.waitForStarted() ;

	p.write( password.toLatin1() ) ;
	p.closeWriteChannel() ;
	p.waitForFinished() ;

	return p.readAllStandardOutput() ;
}

static inline QByteArray hmac_key( const QString& keyFile,const QString& password )
{
	auto _getKey = []( gcry_md_hd_t handle,const QString& keyFile ){

		QFile f( keyFile ) ;

		if( f.open( QIODevice::ReadOnly ) ){

			const int size = 1024 ;
			char buffer[ size ] ;

			while( true ){

				auto e = f.read( buffer,size ) ;

				if( e <= 0 ){
					break ;
				}else{
					gcry_md_write( handle,buffer,e ) ;
				}
			}

			auto key = reinterpret_cast< const char * >( gcry_md_read( handle,0 ) ) ;
			auto len = static_cast< int >( gcry_md_get_algo_dlen( GCRY_MD_SHA256 ) ) ;

			QByteArray r( key,len ) ;

			return r.toHex() ;
		}else{
			return QByteArray() ;
		}
	} ;

	if( gcry_control( GCRYCTL_INITIALIZATION_FINISHED_P ) == 0 ){

		gcry_check_version( nullptr ) ;
		gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;
	}

	QByteArray key ;
	gcry_md_hd_t handle ;

	auto r = gcry_md_open( &handle,GCRY_MD_SHA256,GCRY_MD_FLAG_HMAC ) ;

	if( r == GPG_ERR_NO_ERROR ){

		auto e = password.toLatin1() ;

		r = gcry_md_setkey( handle,e.constData(),e.size() ) ;

		if( r == GPG_ERR_NO_ERROR ){

			key = _getKey( handle,keyFile ) ;
		}

		gcry_md_close( handle ) ;
	}

	return key ;
}

static inline QByteArray hmac_key_1( const QVector<QString>& exe,const QString& keyFile,const QString& password )
{
	Q_UNUSED( exe )
	return hmac_key( keyFile,password ) ;
}

static inline QByteArray keyKeyFile( const QVector<QString>& exe,const QString& keyFile,const QString& password )
{
	Q_UNUSED( exe )

	QFile f( keyFile ) ;

	f.open( QIODevice::ReadOnly ) ;

	return password.toLatin1() + f.readAll() ;
}

static inline QByteArray luks( const QVector<QString>& exe,const QString& keyFile,const QString& password )
{
	Q_UNUSED( exe )
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
}

static inline QByteArray steghide( const QVector<QString>& exe,const QString& keyFile,const QString& password ){

	/*
	 * TODO: look into passing the passphrase more securely
	 */

	QProcess p ;

	p.start( exe.first(),{ "--extract","-sf",keyFile,"-xf","-","-p",password } ) ;

	p.waitForFinished( -1 ) ;

	return p.readAllStandardOutput() ;
}

} //namespace plugins

#endif // PLUGINS_H

/*
 *
 *  Copyright (c) 2014
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
#include <QDir>
#include <QObject>

#include "network_key.h"

#define DEBUG 0

static void _debug( const char * msg )
{
#if DEBUG
	puts( msg ) ;
#endif
}

#define _cast( x ) ( const char * ) x

static result_t _network_key_get_key( const arguments_t * e )
{
	zuluKey_t key ;
	zuluValue_t * value ;

	socket_t s ;

	result_t result ;

	crypt_buffer_ctx ctx ;
	crypt_buffer_result r ;

	const char * encryption_key      = e->encryption_key ;
	size_t encryption_key_key_length = strlen( encryption_key ) ;

	const char * wallet_name = "zuluCrypt" ;
	const char * wallet_key  = e->wallet_key ;

	size_t n ;

	int k = 0 ;

	char * buffer ;

	memset( &result,'\0',sizeof( result ) ) ;

	memset( &key,'\0',sizeof( zuluKey_t ) ) ;

	key.key_0_length = strlen( e->path ) + 1 ;

	if( key.key_0_length > sizeof( key.key_0 ) ){

		_debug( "error: key length buffer overflow" ) ;
		return result ;
	}else{
		memcpy( key.key_0,e->path,key.key_0_length ) ;
	}

	key.key_1_length = strlen( e->uuid ) + 1 ;

	if( key.key_1_length > sizeof( key.key_1 ) ){

		_debug( "error: key length buffer overflow" ) ;
		return result ;
	}else{
		memcpy( key.key_1,e->uuid,key.key_1_length ) ;
	}

	key.wallet_key_length = strlen( wallet_key ) ;

	if( key.wallet_key_length > sizeof( key.wallet_key ) ){

		_debug( "error: wallet key length buffer overflow" ) ;
		return result ;
	}else{
		memcpy( key.wallet_key,wallet_key,key.wallet_key_length ) ;
	}

	n = strlen( wallet_name ) ;

	if( n > sizeof( key.wallet_name ) ){

		_debug( "error: buffer overflow" ) ;
		return result ;
	}else{
		memcpy( key.wallet_name,wallet_name,n + 1 ) ;
		memcpy( key.application_name,wallet_name,n + 1 ) ;
	}

	while( 1 ){

		_debug( "client connecting ..." ) ;

		s = SocketNet( e->network_address,e->port_number ) ;

		if( SocketConnect( &s ) ){

			_debug( "client connected" ) ;
			break ;
		}else{
			if( k == 10 ){

				_debug( "failed to connect to server" ) ;
				return result ;
			}else{
				sleep( 1 ) ;
				k++ ;
			}
		}
	}

	if( crypt_buffer_init( &ctx,encryption_key,encryption_key_key_length ) ){

		if( crypt_buffer_encrypt( ctx,_cast( &key ),sizeof( zuluKey_t ),&r ) ){

			SocketSendData( s,_cast( r.buffer ),r.length ) ;

			buffer = nullptr ;

			n = SocketGetData( s,&buffer ) ;

			if( buffer ){

				if( crypt_buffer_decrypt( ctx,buffer,n,&r ) ){

					if( r.length == sizeof( zuluValue_t ) ){

						value = ( zuluValue_t * )r.buffer ;

						if( value->key_found ){

							if( value->value_length <= sizeof( result.key ) ){

								result.key_len = value->value_length ;
								memcpy( result.key,value->value,value->value_length ) ;
								result.got_key = 1 ;
							}
						}else{
							_debug( "key not found" ) ;
						}
					}
				}

				free( buffer ) ;
			}
		}

		crypt_buffer_uninit( &ctx ) ;
	}

	SocketClose( &s ) ;

	return result ;
}

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;

	MainWindow w ;

	w.setToken( *( argv + 3 ) ) ;
	w.setApplicationName( "network key" ) ;
	w.setkeyLabel( QObject::tr( "enter zuluCrypt wallet key below" ) ) ;
	w.setkeyFileLabel( QObject::tr( "enter network key below" ) ) ;
	w.setKeyFileAsKey() ;

	auto e = [ = ]( const QVector<QString>& exe,const QString& network_key,const QString& password ){

		Q_UNUSED( exe ) ;

		arguments_t e ;

		QByteArray k = password.toLatin1() ;
		QByteArray n = network_key.toLatin1() ;

		QByteArray z = QString( "UUID=\"%1\"" ).arg( QString( *( argv + 2 ) ) ).toLatin1()  ;

		e.path            = *( argv + 1 ) ;
		e.uuid            = z.constData() ;
		e.wallet_key      = k.constData() ;
		e.encryption_key  = n.constData() ;
		e.network_address = NETWORK_ADDRESS ;
		e.port_number     = PORT_NUMBER ;

		QFile file( QDir::homePath() + "/.zuluCrypt/network_key" ) ;

		QByteArray networkAddress ;

		if( file.open( QIODevice::ReadOnly ) ){

			QStringList l = QString( file.readAll() ).split( "\n" ) ;

			if( l.size() >= 2 ){

				networkAddress    = l.first().toLatin1() ;
				e.network_address = networkAddress.constData() ;
				e.port_number     = l.at( 1 ).toInt() ;
			}
		}

		auto r = _network_key_get_key( &e ) ;

		if( r.got_key ){

			return QByteArray( r.key,r.key_len ) ;
		}else{
			return QByteArray() ;
		}
	} ;

	w.setKeyFunction( e ) ;
	w.Show() ;

	return a.exec() ;
}

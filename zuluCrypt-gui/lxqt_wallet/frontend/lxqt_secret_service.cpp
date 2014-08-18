/*
 * copyright: 2013
 * name : mhogo mchungu
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "lxqt_secret_service.h"

#include "task.h"
/*
 * adding libsecret header file together with C++ header files doesnt seem to work.
 * as a workaround,a static library that interfaces with libsecret is used and a "pure" C interface of the
 * static library is then used in C++
 */
extern "C" {
char * lxqt_secret_service_get_value( const char * key,const void * ) ;
int lxqt_secret_service_password_store_sync( const char * key,const char * value,const void *,const void * ) ;
int lxqt_secret_service_clear_sync( const char * key,const void *,const void * ) ;
char ** lxqt_secret_get_all_keys( const void *,const void *,int * count ) ;
int lxqt_secret_service_wallet_size( const void * ) ;
int lxqt_secret_service_wallet_is_open( const void * ) ;
void * lxqt_secret_service_create_schema( const char * schemaName,const char * type ) ;
}

LxQt::Wallet::secretService::secretService()
{
	m_schema   = nullptr ;
	m_schema_1 = nullptr ;
}

LxQt::Wallet::secretService::~secretService()
{
	free( m_schema ) ;
	free( m_schema_1 ) ;
}

void LxQt::Wallet::secretService::setImage( const QString& image )
{
	Q_UNUSED( image ) ;
}

bool LxQt::Wallet::secretService::addKey( const QString& key,const QByteArray& value )
{
	if( key.isEmpty() ){
		return false ;
	}else{
		if( m_schema && m_schema_1 ){
			return lxqt_secret_service_password_store_sync( key.toLatin1().constBegin(),value.constData(),m_schema,m_schema_1 ) ;
		}else{
			return false ;
		}
	}
}

bool LxQt::Wallet::secretService::await_open( const QString& walletName,const QString& applicationName,
					      const QString& password,const QString& displayApplicationName )
{
	this->open( walletName,applicationName,password,displayApplicationName ) ;

	m_loop.exec() ;

	return m_opened ;
}

void LxQt::Wallet::secretService::open( const QString& walletName,const QString& applicationName,
					const QString& password,const QString& displayApplicationName )
{
	/*
	 * this backend does not use this variable
	 */
	m_password  = password ;

	Q_UNUSED( displayApplicationName ) ;

	if( applicationName.isEmpty() ){

		m_byteArrayWalletName      = walletName.toLatin1() ;
		m_byteArrayApplicationName = walletName.toLatin1() ;

		m_walletName        = m_byteArrayWalletName.constData() ;
		m_applicationName   = m_byteArrayApplicationName.constData() ;

		m_byteArraySchemaName = QString( "lxqt.Wallet.%1.%2" ).arg( walletName ).arg( walletName ).toLatin1() ;
	}else{
		m_byteArrayWalletName      = walletName.toLatin1() ;
		m_byteArrayApplicationName = applicationName.toLatin1() ;

		m_walletName        = m_byteArrayWalletName.constData() ;
		m_applicationName   = m_byteArrayApplicationName.constData() ;

		m_byteArraySchemaName = QString( "lxqt.Wallet.%1.%2" ).arg( walletName ).arg( applicationName ).toLatin1() ;
	}

	m_schema   = lxqt_secret_service_create_schema( m_byteArraySchemaName.constData(),"string" ) ;
	m_schema_1 = lxqt_secret_service_create_schema( m_byteArraySchemaName.constData(),"integer" ) ;

	auto _a = [&](){

		return lxqt_secret_service_wallet_is_open( m_schema ) ;
	} ;

	auto _b = [&]( bool opened ){

		this->walletOpened( opened ) ;
	} ;

	LxQt::Wallet::Task::run< bool >( _a ).then( _b ) ;
}

void LxQt::Wallet::secretService::walletOpened( bool opened )
{
	m_opened = opened ;
	m_loop.exit() ;
	emit walletIsOpen( opened ) ;
}

QByteArray LxQt::Wallet::secretService::readValue( const QString& key )
{
	if( m_schema ){
		QByteArray r ;
		char * e = lxqt_secret_service_get_value( key.toLatin1().constData(),m_schema ) ;
		if( e ){
			r = QByteArray( e ) ;
			free( e ) ;
		}
		return r ;
	}else{
		return QByteArray() ;
	}
}

QVector<LxQt::Wallet::walletKeyValues> LxQt::Wallet::secretService::readAllKeyValues( void )
{
	QVector<LxQt::Wallet::walletKeyValues> p ;
	QStringList l = this->readAllKeys() ;
	for( const auto& it : l ){
		LxQt::Wallet::walletKeyValues q( it,this->readValue( it ) ) ;
		p.append( q ) ;
	}
	return p ;
}

QStringList LxQt::Wallet::secretService::readAllKeys( void )
{
	if( m_schema && m_schema_1 ){
		int count ;
		QStringList l ;
		char ** c = lxqt_secret_get_all_keys( m_schema,m_schema_1,&count ) ;
		char * e ;
		if( c ){
			for( int i = 0 ; i < count ; i++ ){
				e = *( c + i ) ;
				l.append( e ) ;
				free( e ) ;
			}
			free( c ) ;
		}
		return l ;
	}else{
		return QStringList() ;
	}
}

void LxQt::Wallet::secretService::deleteKey( const QString& key )
{
	if( m_schema && m_schema_1 ){
		if( !key.isEmpty() ){
			lxqt_secret_service_clear_sync( key.toLatin1().constData(),m_schema,m_schema_1 ) ;
		}
	}
}

int LxQt::Wallet::secretService::walletSize( void )
{
	if( m_schema ){
		return lxqt_secret_service_wallet_size( m_schema ) ;
	}else{
		return -1 ;
	}
}

void LxQt::Wallet::secretService::closeWallet( bool b )
{
	Q_UNUSED( b ) ;
}

LxQt::Wallet::walletBackEnd LxQt::Wallet::secretService::backEnd( void )
{
	return LxQt::Wallet::secretServiceBackEnd ;
}

bool LxQt::Wallet::secretService::walletIsOpened( void )
{
	if( m_schema ){
		return lxqt_secret_service_wallet_is_open( m_schema ) ;
	}else{
		return false ;
	}
}

void LxQt::Wallet::secretService::setInterfaceObject( QWidget * interfaceObject )
{
	m_interfaceObject = interfaceObject ;
	connect( this,SIGNAL( walletIsOpen( bool ) ),m_interfaceObject,SLOT( walletIsOpen( bool ) ) ) ;
}

QObject * LxQt::Wallet::secretService::qObject( void )
{
	return static_cast< QObject * >( this ) ;
}

QString LxQt::Wallet::secretService::storagePath()
{
	return QString() ;
}

void LxQt::Wallet::secretService::changeWalletPassWord( const QString& walletName,const QString& applicationName )
{
	Q_UNUSED( applicationName ) ;
	Q_UNUSED( walletName ) ;
}

QStringList LxQt::Wallet::secretService::managedWalletList()
{
	return QStringList() ;
}

QString LxQt::Wallet::secretService::localDefaultWalletName()
{
	return QString() ;
}

QString LxQt::Wallet::secretService::networkDefaultWalletName()
{
	return QString() ;
}

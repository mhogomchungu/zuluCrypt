/*
 * copyright: 2013
 * name : Francis Banyikwa
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

#include "lxqt_kwallet.h"
#include "task.h"

LxQt::Wallet::kwallet::kwallet() : m_kwallet( 0 )
{

}

LxQt::Wallet::kwallet::~kwallet()
{
	if( m_kwallet ){
		m_kwallet->sync() ;
		m_kwallet->deleteLater() ;
	}
}

void LxQt::Wallet::kwallet::setImage( const QString& image )
{
	Q_UNUSED( image ) ;
}

bool LxQt::Wallet::kwallet::addKey( const QString& key,const QByteArray& value )
{
	m_kwallet->writePassword( key,value ) ;
	return true ;
}

bool LxQt::Wallet::kwallet::await_open( const QString& walletName,const QString& applicationName,
					const QString& password,const QString& displayApplicationName )
{
	if( walletName == "default" ){
		m_walletName = KWallet::Wallet::LocalWallet() ;
	}else{
		m_walletName = walletName ;
	}
	m_applicationName   = applicationName ;
	m_password          = password ;

	Q_UNUSED( displayApplicationName ) ;

	#define _task LxQt::Wallet::Task::await<KWallet::Wallet*>

	m_kwallet = _task( [ this ](){ return KWallet::Wallet::openWallet( m_walletName,0,KWallet::Wallet::Synchronous ) ; } ) ;

	if( m_kwallet != nullptr ){

		if( m_applicationName.isEmpty() ){

			m_kwallet->createFolder( m_kwallet->PasswordFolder() ) ;
			m_kwallet->setFolder( m_kwallet->PasswordFolder() ) ;
		}else{
			m_kwallet->createFolder( m_applicationName ) ;
			m_kwallet->setFolder( m_applicationName ) ;
		}

		return true ;
	}else{
		return false ;
	}
}

void LxQt::Wallet::kwallet::open( const QString& walletName,const QString& applicationName,
				  const QString& password,const QString& displayApplicationName )
{
	if( walletName == "default" ){
		m_walletName = KWallet::Wallet::LocalWallet() ;
	}else{
		m_walletName = walletName ;
	}
	m_applicationName   = applicationName ;
	m_password          = password ;

	Q_UNUSED( displayApplicationName ) ;

	m_kwallet = KWallet::Wallet::openWallet( m_walletName,0,KWallet::Wallet::Asynchronous ) ;

	connect( m_kwallet,SIGNAL( walletOpened( bool ) ),this,SLOT( walletOpened( bool ) ) ) ;
}

void LxQt::Wallet::kwallet::walletOpened( bool opened )
{
	if( opened ){

		if( m_applicationName.isEmpty() ){

			m_kwallet->createFolder( m_kwallet->PasswordFolder() ) ;
			m_kwallet->setFolder( m_kwallet->PasswordFolder() ) ;
		}else{
			m_kwallet->createFolder( m_applicationName ) ;
			m_kwallet->setFolder( m_applicationName ) ;
		}
	}

	connect( this,SIGNAL( walletOpened_1( bool ) ),m_interfaceObject,SLOT( walletIsOpen( bool ) ) ) ;
	emit walletOpened_1( opened ) ;
}

QByteArray LxQt::Wallet::kwallet::readValue( const QString& key )
{
	QString value ;
	m_kwallet->readPassword( key,value ) ;
	return value.toLatin1() ;
}

QVector<LxQt::Wallet::walletKeyValues> LxQt::Wallet::kwallet::readAllKeyValues( void )
{
	QVector<LxQt::Wallet::walletKeyValues> p ;
	QStringList l = m_kwallet->entryList() ;
	QString value ;
	int j = l.size() ;

	for( int i = 0 ; i < j ; i++ ){

		m_kwallet->readPassword( l.at( i ),value ) ;
		LxQt::Wallet::walletKeyValues q( l.at( i ),value.toLatin1() ) ;
		p.append( q ) ;
	}
	return p ;
}

QStringList LxQt::Wallet::kwallet::readAllKeys( void )
{
	return m_kwallet->entryList() ;
}

void LxQt::Wallet::kwallet::deleteKey( const QString& key )
{
	m_kwallet->removeEntry( key ) ;
}

int LxQt::Wallet::kwallet::walletSize( void )
{
	QStringList l = m_kwallet->entryList() ;
	return l.size() ;
}

void LxQt::Wallet::kwallet::closeWallet( bool b )
{
	m_kwallet->closeWallet( m_walletName,b ) ;
}

LxQt::Wallet::walletBackEnd LxQt::Wallet::kwallet::backEnd( void )
{
	return LxQt::Wallet::kwalletBackEnd ;
}

bool LxQt::Wallet::kwallet::walletIsOpened( void )
{
	return m_kwallet->isOpen() ;
}

void LxQt::Wallet::kwallet::setInterfaceObject( QWidget * interfaceObject )
{
	m_interfaceObject = interfaceObject ;
}

QObject * LxQt::Wallet::kwallet::qObject( void )
{
	return static_cast< QObject * >( this ) ;
}

QString LxQt::Wallet::kwallet::storagePath()
{
	return m_kwallet->PasswordFolder() ;
}

void LxQt::Wallet::kwallet::changeWalletPassWord( const QString& walletName,const QString& applicationName )
{
	Q_UNUSED( applicationName ) ;
	m_kwallet->changePassword( walletName,0 ) ;
	connect( this,SIGNAL( walletpassWordChanged( bool ) ),m_interfaceObject,SLOT( walletpassWordChanged( bool ) ) ) ;
	emit walletpassWordChanged( false ) ;
}

QStringList LxQt::Wallet::kwallet::managedWalletList()
{
	return m_kwallet->walletList() ;
}

QString LxQt::Wallet::kwallet::localDefaultWalletName()
{
	return m_kwallet->LocalWallet() ;
}

QString LxQt::Wallet::kwallet::networkDefaultWalletName()
{
	return m_kwallet->NetworkWallet() ;
}


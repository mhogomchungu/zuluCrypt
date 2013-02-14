/*
 *
 *  Copyright ( c ) 2012
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 */

#include "kwalletplugin.h"

class KWallet_Handle
{
public:
	KWallet_Handle() ;
	~KWallet_Handle() ;
	bool open( void ) ;
	bool close( void );
	bool setFolder( QString folder ) ;
	int readMap( QMap<QString,QString> & map ) ;
	int writeMap( QMap<QString,QString> & map ) ;
	QString getKey( QString uuid ) ;

	static bool keyDoesNotExist( QString key ) ;
	static QString KDEKwalletDefaultName( void ) ;
	static bool folderDoesNotExist( void ) ;

private:
#if BUILD_KWALLET
	KWallet::Wallet * m_wallet ;
#endif
};

KWallet_Handle::KWallet_Handle()
{
}

KWallet_Handle::~KWallet_Handle()
{
}

#if BUILD_KWALLET

bool KWallet_Handle::keyDoesNotExist( QString key )
{
	return KWallet::Wallet::keyDoesNotExist( zuluOptions::wallet(),zuluOptions::key(),key ) ;
}

QString KWallet_Handle::KDEKwalletDefaultName()
{
	return KWallet::Wallet::LocalWallet() ;
}

bool KWallet_Handle::open( void )
{
	QWidget * aW = QApplication::activeWindow() ;
	uint wId = 0;
	if( aW ){
	    wId = ( uint ) aW->winId();
	}
	m_wallet = KWallet::Wallet::openWallet( zuluOptions::wallet(),wId,KWallet::Wallet::Synchronous ) ;

	return m_wallet ? true : false ;
}

bool KWallet_Handle::close( void )
{
	return KWallet::Wallet::closeWallet( zuluOptions::wallet(),false ) ;
}

bool KWallet_Handle::setFolder( QString folder )
{
	if( !m_wallet->hasFolder( folder ) ){
		m_wallet->createFolder( folder ) ;
	}
	return m_wallet->setFolder( folder ) ;
}

int KWallet_Handle::readMap( QMap<QString,QString> & map )
{
	return m_wallet->readMap( zuluOptions::key(),map ) ;
}

int KWallet_Handle::writeMap( QMap<QString, QString> & map )
{
	return m_wallet->writeMap( zuluOptions::key(),map ) ;
}

bool KWallet_Handle::folderDoesNotExist( void )
{
	return KWallet::Wallet::folderDoesNotExist( zuluOptions::wallet(),zuluOptions::formData() ) ;
}

QString KWallet_Handle::getKey( QString uuid )
{
	QString key ;

	QString fd = zuluOptions::formData() ;
	if( !m_wallet->hasFolder( fd ) ){
		if( !m_wallet->createFolder( fd ) ){
			return key ;
		}
	}

	m_wallet->setFolder( fd )  ;

	QMap <QString,QString> map ;

	if( m_wallet->readMap( zuluOptions::key(),map ) ){
		return key ;
	}

	key = map.value( uuid ) ;

	if( key.isEmpty() ){
		uuid.remove( QChar( '\"' ) ) ;
		key = map.value( uuid ) ;
	}

	return key ;
}

kwalletplugin::kwalletplugin( QWidget * parent )
{
	m_parent = parent ;
	m_walletHandle = new KWallet_Handle() ;
}

bool kwalletplugin::KwalletIsEnabled()
{
	return true;
}

bool kwalletplugin::hasFunctionality()
{
	return true  ;
}

bool kwalletplugin::isOpen()
{
	return m_walletOpened ;
}
#else
kwalletplugin::kwalletplugin( QWidget * parent )
{
	m_parent = parent ;
	m_walletHandle = 0 ;
}
bool kwalletplugin::KwalletIsEnabled()
{
	return false ;
}
bool kwalletplugin::hasFunctionality( void )
{
	return false ;
}
QString KWallet_Handle::getKey( QString uuid )
{
	return uuid ;
}
bool KWallet_Handle::close( void )
{
	return true ;
}
bool KWallet_Handle::open( void )
{
	return false ;
}
bool KWallet_Handle::setFolder( QString s )
{
	Q_UNUSED( s ) ;
	return false ;
}
int KWallet_Handle::readMap( QMap<QString,QString> & map )
{
	Q_UNUSED( map ) ;
	return 0 ;
}
int KWallet_Handle::writeMap( QMap<QString,QString> & map )
{
	Q_UNUSED( map ) ;
	return 0 ;
}
bool KWallet_Handle::folderDoesNotExist( void )
{
	return false ;
}
bool KWallet_Handle::keyDoesNotExist( QString key )
{
	Q_UNUSED( key ) ;
	return true ;
}
QString KWallet_Handle::KDEKwalletDefaultName( void )
{
	return QString( "kdewallet" ) ;
}
bool kwalletplugin::isOpen( void )
{
	return false ;
}
#endif

QString kwalletplugin::KDEKwalletDefaultName( void )
{
	return KWallet_Handle::KDEKwalletDefaultName() ;
}

bool kwalletplugin::open()
{
	m_walletOpened = m_walletHandle->open() ;
	return m_walletOpened ;
}

bool kwalletplugin::setFolder( QString folder )
{
	return m_walletHandle->setFolder( folder ) ;
}

int kwalletplugin::readMap( QMap<QString,QString> & map )
{
	return m_walletHandle->readMap( map ) ;
}

int kwalletplugin::writeMap( QMap<QString, QString> & map )
{
	return m_walletHandle->writeMap( map ) ;
}

bool kwalletplugin::folderDoesNotExist()
{
	return KWallet_Handle::folderDoesNotExist() ;
}

QString kwalletplugin::getKey( QString uuid )
{
	return m_walletHandle->getKey( uuid ) ;
}

void kwalletplugin::close()
{
	if( m_walletOpened ){
		/*
		 * We dont close the wallet to leave it open until the application is closed
		 */
		//m_walletHandle->close() ;
		;
	}
}

bool kwalletplugin::keyDoesNotExist( QString key )
{
	return KWallet_Handle::keyDoesNotExist( key ) ;
}

kwalletplugin::~kwalletplugin()
{
	if( m_walletHandle ){
		delete m_walletHandle ;
	}
}

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

#include <QStringList>
#include <QDebug>
#include <QWidget>
#include <QApplication>
#include <QFile>
#include <QDir>

class kwalletpluginPrivate
{
public:
	kwalletpluginPrivate() ;
	~kwalletpluginPrivate() ;
	bool open( void ) ;
	bool close( void );
	bool setFolder( QString folder ) ;
	int readMap( QMap<QString,QString> & map ) ;
	int writeMap( QMap<QString,QString> & map ) ;
	QString getKey( QString uuid ) ;

	static bool keyDoesNotExist( QString key ) ;
	static QString KDEKwalletDefaultName( void ) ;
	static bool folderDoesNotExist( void ) ;
	QWidget * m_parent ;
	bool m_walletOpened ;
private:
#if BUILD_KWALLET
	KWallet::Wallet * m_wallet ;
#endif
};

kwalletpluginPrivate::kwalletpluginPrivate()
{
}

kwalletpluginPrivate::~kwalletpluginPrivate()
{
}

#if BUILD_KWALLET

bool kwalletpluginPrivate::keyDoesNotExist( QString key )
{
	return KWallet::Wallet::keyDoesNotExist( kwalletplugin::wallet(),kwalletplugin::key(),key ) ;
}

QString kwalletpluginPrivate::KDEKwalletDefaultName()
{
	return KWallet::Wallet::LocalWallet() ;
}

bool kwalletpluginPrivate::open( void )
{
	QWidget * aW = QApplication::activeWindow() ;
	uint wId = 0;
	if( aW ){
	    wId = ( uint ) aW->winId();
	}

	m_wallet = KWallet::Wallet::openWallet( kwalletplugin::wallet(),wId,KWallet::Wallet::Synchronous ) ;

	m_walletOpened = m_wallet != 0 ;

	return m_walletOpened ;
}

bool kwalletpluginPrivate::close( void )
{
	return KWallet::Wallet::closeWallet( kwalletplugin::wallet(),false ) ;
}

bool kwalletpluginPrivate::setFolder( QString folder )
{
	if( !m_wallet->hasFolder( folder ) ){
		m_wallet->createFolder( folder ) ;
	}
	return m_wallet->setFolder( folder ) ;
}

int kwalletpluginPrivate::readMap( QMap<QString,QString>& map )
{
	return m_wallet->readMap( kwalletplugin::key(),map ) ;
}

int kwalletpluginPrivate::writeMap( QMap<QString,QString>& map )
{
	if( m_wallet->writeMap( kwalletplugin::key(),map ) ){
		m_wallet->sync() ;
		return true ;
	}else{
		return false ;
	}
}

bool kwalletpluginPrivate::folderDoesNotExist( void )
{
	return KWallet::Wallet::folderDoesNotExist( kwalletplugin::wallet(),kwalletplugin::formData() ) ;
}

QString kwalletpluginPrivate::getKey( QString keyId )
{
	QString key ;

	QString fd = kwalletplugin::formData() ;
	if( !m_wallet->hasFolder( fd ) ){
		if( !m_wallet->createFolder( fd ) ){
			return key ;
		}
	}

	m_wallet->setFolder( fd )  ;

	QMap <QString,QString> map ;

	if( m_wallet->readMap( kwalletplugin::key(),map ) ){
		return key ;
	}

	key = map.value( keyId ) ;

	if( key.isEmpty() ){
		keyId.remove( QChar( '\"' ) ) ;
		key = map.value( keyId ) ;
	}

	return key ;
}

kwalletplugin::kwalletplugin( QWidget * parent )
{
	d = new kwalletpluginPrivate() ;
	d->m_parent = parent ;
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
	return d->m_walletOpened ;
}
#else
kwalletplugin::kwalletplugin( QWidget * parent )
{
	d->m_parent = parent ;
	d->m_walletOpened = false ;
}
bool kwalletplugin::KwalletIsEnabled()
{
	return false ;
}
bool kwalletplugin::hasFunctionality( void )
{
	return false ;
}
QString kwalletpluginPrivate::getKey( QString uuid )
{
	return uuid ;
}
bool kwalletpluginPrivate::close( void )
{
	return true ;
}
bool kwalletpluginPrivate::open( void )
{
	return false ;
}
bool kwalletpluginPrivate::setFolder( QString s )
{
	Q_UNUSED( s ) ;
	return false ;
}
int kwalletpluginPrivate::readMap( QMap<QString,QString> & map )
{
	Q_UNUSED( map ) ;
	return 0 ;
}
int kwalletpluginPrivate::writeMap( QMap<QString,QString> & map )
{
	Q_UNUSED( map ) ;
	return 0 ;
}
bool kwalletpluginPrivate::folderDoesNotExist( void )
{
	return false ;
}
bool kwalletpluginPrivate::keyDoesNotExist( QString key )
{
	Q_UNUSED( key ) ;
	return true ;
}
QString kwalletpluginPrivate::KDEKwalletDefaultName( void )
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
	return kwalletpluginPrivate::KDEKwalletDefaultName() ;
}

bool kwalletplugin::open()
{
	return d->open() ;
}

bool kwalletplugin::setFolder( QString folder )
{
	return d->setFolder( folder ) ;
}

int kwalletplugin::readMap( QMap<QString,QString> & map )
{
	return d->readMap( map ) ;
}

int kwalletplugin::writeMap( QMap<QString, QString> & map )
{
	return d->writeMap( map ) ;
}

bool kwalletplugin::folderDoesNotExist()
{
	return kwalletpluginPrivate::folderDoesNotExist() ;
}

QString kwalletplugin::getKey( QString uuid )
{
	return d->getKey( uuid ) ;
}

void kwalletplugin::close()
{
	if( d->m_walletOpened ){
		/*
		 * We dont close the wallet to leave it open until the application is closed
		 */
		//d->close() ;
		;
	}
}

bool kwalletplugin::keyDoesNotExist( QString key )
{
	return kwalletpluginPrivate::keyDoesNotExist( key ) ;
}

kwalletplugin::~kwalletplugin()
{
	if( d ){
		delete d ;
	}
}

#define WALLET_NAME    "zuluCrypt"
#define FORM_DATA_NAME "zuluCrypt"
#define KEY_NAME       "zuluCrypt-LUKS"

QString kwalletplugin::formData()
{
	return QString( FORM_DATA_NAME ) ;
}

QString kwalletplugin::key()
{
	return QString( KEY_NAME ) ;
}

QString kwalletplugin::wallet()
{
	QString path = kwalletplugin::walletPath() ;

	QFile f( path ) ;

	if( !f.exists() ){
		kwalletplugin::setWalletToKDEDefaultName() ;
	}
	f.open( QIODevice::ReadOnly ) ;
	QString name = QString( f.readAll() );
	f.close();
	return name ;
}

QString kwalletplugin::walletPath()
{
	return QDir::homePath() + QString( "/.zuluCrypt/walletName" ) ;
}

bool kwalletplugin::walletIsKDEWallet()
{
	return kwalletplugin::wallet() == kwalletplugin::KDEKwalletDefaultName() ;
}

void kwalletplugin::setWalletToKDEDefaultName()
{
	kwalletplugin::setWalletName( kwalletplugin::KDEKwalletDefaultName() ) ;
}

void kwalletplugin::setWalletToDefaultName()
{
	kwalletplugin::setWalletName( QString( WALLET_NAME ) ) ;
}

void kwalletplugin::setWalletName( QString name )
{
	QString path = kwalletplugin::walletPath() ;
	QFile f ( path ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( name.toAscii() ) ;
	f.close();
}

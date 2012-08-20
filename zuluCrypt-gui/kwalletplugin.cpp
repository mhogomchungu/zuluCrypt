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

#if BUILD_KWALLET

kwalletplugin::kwalletplugin( QWidget * parent )
{
	m_parent = parent ;
}

bool kwalletplugin::hasFunctionality()
{
	return true ;
}

bool kwalletplugin::open()
{
	m_wallet = KWallet::Wallet::openWallet( zuluOptions::wallet(),m_parent->winId(),KWallet::Wallet::Synchronous ) ;

	if( m_wallet )
		m_walletOpened = true ;
	else
		m_walletOpened = false ;

	return m_walletOpened ;
}

bool kwalletplugin::setFolder( QString folder )
{
	return m_wallet->setFolder( folder ) ;
}

int kwalletplugin::readMap( QMap<QString,QString> & map )
{
	return m_wallet->readMap( zuluOptions::key(),map ) ;
}

int kwalletplugin::writeMap( QMap<QString, QString> & map )
{
	return m_wallet->writeMap( zuluOptions::key(),map ) ;
}

bool kwalletplugin::isOpen()
{
	return m_walletOpened ;
}

bool kwalletplugin::folderDoesNotExist()
{
	return KWallet::Wallet::folderDoesNotExist( zuluOptions::wallet(),zuluOptions::formData() ) ;
}

QString kwalletplugin::getKey( QString uuid )
{
	QString key ;

	if( m_walletOpened == false )
		return key ;

	QString fd = zuluOptions::formData() ;
	if( !m_wallet->hasFolder( fd ) )
		if( !m_wallet->createFolder( fd ) )
			return key ;

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

void kwalletplugin::close()
{
	if( m_walletOpened ){
		KWallet::Wallet::closeWallet( zuluOptions::wallet(),false ) ;
		m_wallet->deleteLater();
	}
}

bool kwalletplugin::keyDoesNotExist( QString key )
{
	return KWallet::Wallet::keyDoesNotExist( zuluOptions::wallet(),zuluOptions::key(),key ) ;
}

kwalletplugin::~kwalletplugin()
{
}

#endif

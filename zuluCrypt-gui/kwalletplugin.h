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

#ifndef KWALLETPLUGIN_H
#define KWALLETPLUGIN_H

#include "can_build_kwallet.h"

#if BUILD_KWALLET

#include <QString>
#include <QStringList>
#include <QMap>
#include <QDebug>
#include <QWidget>
#include <kwallet.h>

class kwalletplugin
{
public:
	kwalletplugin( QWidget * );
	~kwalletplugin() ;
	QString getKey( QString uuid ) ;
	void close( void ) ;
	bool open( void ) ;
	static bool hasFunctionality( void ) ;
	bool setFolder( QString ) ;
	int readMap( QString,QMap<QString,QString> & ) ;
private:
	KWallet::Wallet * m_wallet ;
	QWidget * m_parent ;
	bool m_walletOpened ;
};
#else
#include <QString>
#include <QStringList>
class kwalletplugin
{
public:
	kwalletplugin( QWidget * p ){ m_parent = p ; m_walletOpened = false ; };
	//~kwalletplugin() ;
	QString getKey( QString uuid ){ return uuid ; } ;
	void close( void ){} ;
	bool open( void ){ return false ; } ;
	static bool hasFunctionality( void ) { return false ; } ;
	bool setFolder( QString ){ return false ; } ;
	int readMap( QString,QMap<QString,QString> & ) { QMap<QString,QString> m ; return m ; };

private:
	//KWallet::Wallet * m_wallet ;
	QWidget * m_parent ;
	bool m_walletOpened ;
};

#endif
#endif // KWALLETPLUGIN_H

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

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDebug>
#include <QWidget>
#include <QApplication>
#include <QFile>
#include <QDir>

class kwalletpluginPrivate ;

class kwalletplugin : public QObject
{
public:
	kwalletplugin( QWidget * );
	~kwalletplugin() ;

	static bool hasFunctionality( void ) ;
	/*
	 * below methods are valid only if hasFunctionality() returns true.
	 */
	QString getKey( QString uuid ) ;
	void close( void ) ;
	bool open( void ) ;
	bool setFolder( QString ) ;
	int readMap( QMap<QString,QString> & ) ;
	int writeMap( QMap<QString,QString> & ) ;
	bool isOpen( void ) ;
	static bool folderDoesNotExist( void ) ;
	static bool keyDoesNotExist( QString key ) ;
	static bool KwalletIsEnabled( void ) ;
	static QString KDEKwalletDefaultName( void ) ;

	/*
	 * below methods are always valid
	 */
	static bool walletIsKDEWallet( void ) ;
	static void setWalletToKDEDefaultName( void ) ;
	static void setWalletToDefaultName( void )    ;
	static void setWalletName( QString )          ;
	static QString walletPath( void )             ;
	static QString wallet( void )                 ;
	static QString key( void )                    ;
	static QString formData( void )               ;
private:
	kwalletpluginPrivate * d ;
};

#endif // KWALLETPLUGIN_H

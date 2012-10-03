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
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "zuluoptions.h"

#define WALLET_NAME    "zuluCrypt"
#define FORM_DATA_NAME "zuluCrypt"
#define KEY_NAME       "zuluCrypt-LUKS"

QString zuluOptions::formData()
{
	return QString( FORM_DATA_NAME ) ;
}

QString zuluOptions::key()
{
	return QString( KEY_NAME ) ;
}

QString zuluOptions::wallet()
{
	QString path = zuluOptions::walletPath() ;

	QFile f( path ) ;

	if( !f.exists() )
		zuluOptions::setWalletToKDEDefaultName() ;

	f.open( QIODevice::ReadOnly ) ;
	QString name = QString( f.readAll() );
	f.close();
	return name ;
}

QString zuluOptions::walletPath()
{
	return QDir::homePath() + QString( "/.zuluCrypt/walletName" ) ;
}

bool zuluOptions::walletIsKDEWallet()
{
	return zuluOptions::wallet() == kwalletplugin::KDEKwalletDefaultName() ;
}

void zuluOptions::setWalletToKDEDefaultName()
{
	zuluOptions::setWalletName( kwalletplugin::KDEKwalletDefaultName() ) ;
}

void zuluOptions::setWalletToDefaultName()
{
	zuluOptions::setWalletName( QString( WALLET_NAME ) ) ;
}

void zuluOptions::setWalletName( QString name )
{
	QString path = zuluOptions::walletPath() ;
	QFile f ( path ) ;

	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;

	f.write( name.toAscii() ) ;

	f.close();
}

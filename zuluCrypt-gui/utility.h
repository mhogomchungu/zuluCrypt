/*
 *
 *  Copyright ( c ) 2011
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

#ifndef MISCFUNCTIONS_H
#define MISCFUNCTIONS_H

#include <QString>
#include <QStringList>

class QByteArray ;

class utility
{
public:
	static QString cmdArgumentValue( const QStringList&,const QString& arg,const QString& defaulT = QString() ) ;
	static QStringList luksEmptySlots( const QString& volumePath ) ;
	static void addToFavorite( const QString& dev,const QString& m_point ) ;
	static QStringList readFavorites( void ) ;
	static void removeFavoriteEntry( const QString& ) ;
	static bool exists( const QString& ) ;
	static bool canCreateFile( const QString& ) ;
	static QString resolvePath( const QString& ) ;
	static QString hashPath( const QByteArray& ) ;
	static QString cryptMapperPath( void ) ;
	static void debug( const QString& ) ;
	static void debug( int ) ;
	static QString mapperPath( const QString& ) ;
	static QString getUUIDFromPath( const QString& ) ;
	static QString getVolumeID( const QString& ) ;
	static bool userIsRoot( void ) ;
	static bool mapperPathExists( const QString& path ) ;
	static QString mountPath( const QString& ) ;
	static QString userName( void ) ;
	static void help( const QString& app ) ;
	static QString shareMountPointToolTip( void ) ;
	static QString shareMountPointToolTip( const QString& ) ;
	static QString sharedMountPointPath( const QString& ) ;
	static bool pathPointsToAFile( const QString& ) ;
	static QString localizationLanguage( const QString& ) ;
	static QString localizationLanguagePath( const QString& ) ;
	static void setLocalizationLanguage( const QString&,const QString& ) ;
	static QString walletName( void ) ;
	static QString applicationName( void ) ;
	static bool pathIsReadable( const QString& ) ;
	static bool setOpenVolumeReadOnly( QWidget * parent,bool check,const QString& app ) ;
	static bool getOpenVolumeReadOnlyOption( const QString& app ) ;
	static QString keyPath( void ) ;
	static void sendKey( const QString& keyPath,const QString& key ) ;
};

#endif // MISCFUNCTIONS_H

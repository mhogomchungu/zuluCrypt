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
#include <QEvent>
#include <QProcess>
#include <QThreadPool>
#include <QRunnable>
#include <QMetaObject>

#include <functional>

#include <unistd.h>

namespace LxQt{
namespace Wallet {
class Wallet ;
}
}

class QByteArray ;
class QEvent ;

typedef std::function< void( void ) > function_t ;

class runnable : public QRunnable
{
public:
	runnable( QObject * object,const char * slot,function_t f ):
		m_function( f ),m_qObject( object ),m_slot( slot )
	{
		QThreadPool::globalInstance()->start( this ) ;
	}
private:
	void run( void )
	{
		m_function() ;
		if( m_qObject && m_slot ){
			QMetaObject::invokeMethod( m_qObject,m_slot ) ;
		}
	}
	function_t m_function ;
	QObject * m_qObject ;
	const char * m_slot ;
};

class utility
{
public:
	static void exec( function_t f )
	{
		new runnable( nullptr,nullptr,f ) ;
	}
	static void exec( QObject * object,const char * slot,function_t f )
	{
		new runnable( object,slot,f ) ;
	}
	class Task
	{
	public :
		static void wait( int s )
		{
			sleep( s ) ;
		}
		static void initTask( void )
		{
			QThreadPool::globalInstance()->setMaxThreadCount( 10 ) ;
		}
		Task( const QString& exe,int waitTime = -1 )
		{
			QProcess p ;
			p.start( exe ) ;
			m_finished   = p.waitForFinished( waitTime ) ;
			m_exitCode   = p.exitCode() ;
			m_exitStatus = p.exitStatus() ;
			m_data       = p.readAll() ;
		}
		QStringList splitOutput( char token )
		{
			return utility::split( m_data,token ) ;
		}
		QByteArray output()
		{
			return m_data ;
		}
		int exitCode()
		{
			return m_exitCode ;
		}
		int exitStatus()
		{
			return m_exitStatus ;
		}
		bool success()
		{
			return m_exitCode == 0 ;
		}
		bool finished()
		{
			return m_finished ;
		}
		bool ok()
		{
			return this->splitOutput( '\n' ).size() > 12 ;
		}
	private:
		QByteArray m_data ;
		int m_exitCode ;
		int m_exitStatus ;
		bool m_finished ;
	};

	static QString cmdArgumentValue( const QStringList&,const QString& arg,const QString& defaulT = QString() ) ;
	static QStringList luksEmptySlots( const QString& volumePath ) ;
	static void addToFavorite( const QString& dev,const QString& m_point ) ;
	static QStringList readFavorites( void ) ;
	static void removeFavoriteEntry( const QString& ) ;
	static bool pathExists( const QString& ) ;
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
	static QString getKeyFromWallet( LxQt::Wallet::Wallet *,const QString& volumeID ) ;
	static bool eventFilter( QObject * gui,QObject * watched,QEvent * event ) ;
	static QStringList split( const QString&,char token = '\n' ) ;
	static QStringList split( const QByteArray&,char token = '\n' ) ;
};

#endif // MISCFUNCTIONS_H

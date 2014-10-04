/*
 *
 *  Copyright ( c ) 2011
 *  name : Francis Banyikwa
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
#include <QDebug>
#include <functional>

#include <unistd.h>

#include "task.h"
#include "lxqt_wallet/frontend/lxqt_wallet.h"

class QByteArray ;
class QEvent ;

namespace utility
{
	struct wallet
	{
		bool opened ;
		bool notConfigured ;
		QString key ;
		QString password ;
	};

	template< typename T > void debug( T t )
	{
		qDebug() << t ;
	}

	wallet getKeyFromWallet( LxQt::Wallet::walletBackEnd,const QString& keyID,const QString& pwd = QString() ) ;

	QString cmdArgumentValue( const QStringList&,const QString& arg,
				  const QString& defaulT = QString() ) ;

	void addToFavorite( const QString& dev,const QString& m_point ) ;
	QStringList readFavorites( void ) ;
	void removeFavoriteEntry( const QString& ) ;
	bool pathExists( const QString& ) ;
	bool canCreateFile( const QString& ) ;
	QString resolvePath( const QString& ) ;
	QString hashPath( const QByteArray& ) ;
	QString cryptMapperPath( void ) ;
	QString mapperPath( const QString& ) ;
	QString getVolumeID( const QString& ) ;
	bool userIsRoot( void ) ;
	bool mapperPathExists( const QString& path ) ;
	QString mountPath( const QString& ) ;
	QString mountPathPostFix( const QString& ) ;
	QString userName( void ) ;
	void help( const QString& app ) ;
	QString shareMountPointToolTip( void ) ;
	QString shareMountPointToolTip( const QString& ) ;
	QString sharedMountPointPath( const QString& ) ;
	bool pathPointsToAFile( const QString& ) ;
	QString localizationLanguage( const QString& ) ;
	QString localizationLanguagePath( const QString& ) ;
	void setLocalizationLanguage( const QString&,const QString& ) ;
	QString walletName( void ) ;
	QString applicationName( void ) ;
	bool pathIsReadable( const QString& ) ;
	bool setOpenVolumeReadOnly( QWidget * parent,bool check,const QString& app ) ;
	bool getOpenVolumeReadOnlyOption( const QString& app ) ;
	QString keyPath( void ) ;
	void keySend( const QString& keyPath,const QString& key ) ;
	bool eventFilter( QObject * gui,QObject * watched,QEvent * event ) ;
	QStringList split( const QString&,char token = '\n' ) ;
	QStringList split( const QByteArray&,char token = '\n' ) ;
	bool userHasGoodVersionOfWhirlpool( void ) ;

	::Task::future< int >& clearVolume( const QString& volumePath,bool * exit,std::function< void( int ) > ) ;
	::Task::future< int >& exec( const QString& ) ;
	::Task::future< QStringList >& luksEmptySlots( const QString& volumePath ) ;
	::Task::future< QString >& getUUIDFromPath( const QString& ) ;
	::Task::future<QString>& getKeyFromWallet( LxQt::Wallet::Wallet * wallet,const QString& volumeID ) ;
	::Task::future< bool >& openMountPoint( const QString& path,const QString& opener ) ;
}

namespace utility
{
	template< typename... All >
	class fileDescriptorRAII
	{
	public:
		fileDescriptorRAII( All... r )
		{
			this->add( r... ) ;
		}
		~fileDescriptorRAII()
		{
			int e ;
			for( int i = 0 ; i < m_count ; i++ ){
				e = **( m_descriptors + i ) ;
				if( e != -1 ){
					::close( e ) ;
				}
			}
		}
	private:
		void add()
		{
		}
		template< typename First,typename... Rest >
		void add( First f,Rest... r )
		{
			*( m_descriptors + m_count ) = f ;
			m_count++ ;
			this->add( r... ) ;
		}
		int m_count = 0 ;
		int * m_descriptors[ sizeof...( All ) ] ;
	};

}

#define utility_fd_raii_2( x,y ) utility::fileDescriptorRAII< int*,int* > raii_x_y( x,y ) ; Q_UNUSED( raii_x_y )
#define utility_fd_raii_1( x )   utility::fileDescriptorRAII< int* > raii_x( x ) ; Q_UNUSED( raii_x )

namespace utility
{
	class Task
	{
	public :
		static ::Task::future< utility::Task >& run( const QString& exe )
		{
			return ::Task::run< utility::Task >( [ exe ](){ return utility::Task( exe ) ; } ) ;
		}
		static void wait( int s )
		{
			sleep( s ) ;
		}
		static void waitForOneSecond( void )
		{
			sleep( 1 ) ;
		}
		static void waitForTwoSeconds( void )
		{
			sleep( 2 ) ;
		}
		Task()
		{
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
		QStringList splitOutput( char token ) const
		{
			return utility::split( m_data,token ) ;
		}
		const QByteArray& output() const
		{
			return m_data ;
		}
		int exitCode() const
		{
			return m_exitCode ;
		}
		int exitStatus() const
		{
			return m_exitStatus ;
		}
		bool success() const
		{
			return m_exitCode == 0 ;
		}
		bool finished() const
		{
			return m_finished ;
		}
		bool ok() const
		{
			return this->splitOutput( '\n' ).size() > 12 ;
		}
	private:
		QByteArray m_data ;
		int m_exitCode ;
		int m_exitStatus ;
		bool m_finished ;
	};
}

#endif // MISCFUNCTIONS_H

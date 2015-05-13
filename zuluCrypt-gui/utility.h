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
#include <QWidget>
#include <QEventLoop>
#include <QTimer>

#include <functional>
#include <utility>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "task.h"
#include "lxqt_wallet/frontend/lxqt_wallet.h"

#include <QObject>
#include <QLabel>

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

	int startApplication( const char * appName,std::function<int()> ) ;

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
	QString mapperPath( const QString&,const QString& component = QString() ) ;
	QString getVolumeID( const QString& ) ;
	bool userIsRoot( void ) ;
	bool mapperPathExists( const QString& path ) ;
	QString mountPath( const QString& ) ;
	QString homeMountPath( const QString& ) ;
	QString mountPathPostFix( const QString& ) ;
	QString userName( void ) ;
	QString shareMountPointToolTip( void ) ;
	QString shareMountPointToolTip( const QString& ) ;
	QString sharedMountPointPath( const QString& ) ;
	bool pathPointsToAFile( const QString& ) ;
	bool pathPointsToAFolder( const QString& ) ;
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
	void licenseInfo( QWidget * ) ;

	::Task::future< int >& clearVolume( const QString& volumePath,bool * exit,std::function< void( int ) > ) ;
	::Task::future< int >& exec( const QString& ) ;
	::Task::future< QStringList >& luksEmptySlots( const QString& volumePath ) ;
	::Task::future< QString >& getUUIDFromPath( const QString& ) ;
	::Task::future<QString>& getKeyFromWallet( LxQt::Wallet::Wallet * wallet,const QString& volumeID ) ;
	::Task::future< bool >& openMountPoint( const QString& path,const QString& opener ) ;
}

namespace utility
{
	class fileHandle
	{
	public:
		fileHandle()
		{
		}
		fileHandle( int r ) : m_fd( r )
		{
		}
		fileHandle( int r,std::function< void( int ) > cmd ) :
			m_fd( r ),m_releaseResource( std::move( cmd ) )
		{
		}
		bool open( const char * filePath,bool ro = true )
		{
			if( ro ){
				m_fd = ::open( filePath,O_RDONLY ) ;
			}else{
				m_fd = ::open( filePath,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) ;
			}
			return m_fd != -1 ;
		}
		int open( const QString& filePath,bool ro = true )
		{
			return this->open( filePath.toLatin1().constData(),ro ) ;
		}
		int handle()
		{
			return m_fd ;
		}
		char getChar()
		{
			char z ;

			while( true ){

				::read( m_fd,&z,1 ) ;

				if( z > ' ' && z < '~' ){

					/*
					 * we are creating a keyfile that is made up
					 * of only printable characters
					 */
					break ;
				}
			}

			return z ;
		}
		void writeChar( char r )
		{
			::write( m_fd,&r,1 ) ;
		}
		~fileHandle()
		{
			m_releaseResource( m_fd ) ;
		}
	private:
		int m_fd = -1 ;

		std::function< void( int ) > m_releaseResource = []( int fd ){

			if( fd != -1 ){
				::close( fd ) ;
			}
		} ;
	} ;
}

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
		static void suspendForOneSecond( void )
		{
			utility::Task::suspend( 1 ) ;
		}
		static void suspend( int s )
		{
			QTimer t ;

			QEventLoop l ;

			QObject::connect( &t,SIGNAL( timeout() ),&l,SLOT( quit() ) ) ;

			t.start( 1000 * s ) ;

			l.exec() ;
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
		void output( const QByteArray& r )
		{
			m_data = r ;
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
			return m_exitCode == 0 && m_exitStatus == QProcess::NormalExit && m_finished == true ;
		}
		bool failed() const
		{
			return !this->success() ;
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

namespace utility
{
	class veraCryptWarning : public QObject
	{
		Q_OBJECT
	public:
		veraCryptWarning()
		{
			connect( &m_timer,SIGNAL( timeout() ),this,SLOT( update() ) ) ;
		}
		void setWarningLabel( QLabel * l )
		{
			m_label = l ;
			m_label->setVisible( false ) ;
		}
		void show( QString w )
		{
			m_warning = std::move( w ) ;
			this->show( true ) ;
		}
		void show( bool show )
		{
			if( show ){
				m_label->setVisible( true ) ;
				m_timer.start( 1000 * 1 ) ;
				this->update() ;
			}
		}
		void hide()
		{
			m_timer.stop() ;
			m_time = 0 ;
			m_label->setVisible( false ) ;
			m_label->setText( m_warning + tr( "Elapsed time: 0 seconds" ) ) ;
		}
	private slots:
		void update()
		{
			QString e ;
			if( m_time >= 60 ){

				e = tr( "Elapsed time: %0 minutes" ).arg( QString::number( m_time / 60,'f',2 ) ) ;
			}else{
				e = tr( "Elapsed time: %0 seconds" ).arg( QString::number( m_time ) ) ;
			}

			m_time++ ;

			m_label->setText( m_warning + e ) ;
		}
	private:
		QLabel * m_label ;
		QTimer m_timer ;
		float m_time = 0 ;
		QString m_warning = tr( "please be patient as unlocking a VeraCrypt volume may take a very long time.\n\n" ) ;
	};
}
#endif // MISCFUNCTIONS_H

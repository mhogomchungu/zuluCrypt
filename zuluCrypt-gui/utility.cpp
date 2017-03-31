/*
 *
 *  Copyright ( c ) 2011-2015
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

#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <QObject>
#include <QDir>
#include <pwd.h>
#include <grp.h>
#include <termios.h>

#include <memory>

#include <QTranslator>
#include <QEventLoop>
#include <QDebug>
#include <QCoreApplication>
#include <blkid/blkid.h>
#include <QByteArray>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QTableWidget>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QProcessEnvironment>
#include <unistd.h>
#include <pwd.h>
#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QMetaObject>
#include <QtNetwork/QLocalSocket>
#include "../zuluCrypt-cli/constants.h"
#include "../zuluCrypt-cli/bin/bash_special_chars.h"
#include "version.h"
#include "locale_path.h"
#include "mount_prefix_path.h"
#include "dialogmsg.h"
#include "support_whirlpool.h"
#include "readonlywarning.h"
#include "../plugins/plugins.h"
#include "plugin.h"
#include "install_prefix.h"
#include "utility.h"

#include "networkAccessManager.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <gcrypt.h>

#include "../zuluCrypt-cli/pluginManager/libzuluCryptPluginManager.h"

#include "../zuluCrypt-cli/utility/process/process.h"

#include "plugin_path.h"

#include "../zuluCrypt-cli/utility/process/process.h"

#include "reuse_mount_point.h"
#include "share_mount_prefix_path.h"

#include "zuluPolkit.h"

struct jsonResult
{
	bool finished ;
	int exitCode ;
	int exitStatus ;
	QByteArray stdError ;
	QByteArray stdOut ;
};

#if POLKIT_SUPPORT

#include "../external_libraries/json/json.hpp"

static QByteArray _json_command( const QByteArray& cookie,
				 const QByteArray& password,
				 const QString& exe )
{
	nlohmann::json json ;

	json[ "cookie" ]   = cookie.constData() ;
	json[ "password" ] = password.constData() ;
	json[ "command" ]  = exe.toLatin1().constData() ;

	return json.dump().c_str() ;
}

static jsonResult _json_result( const QByteArray& e )
{
	try{
		auto json = nlohmann::json::parse( e.constData() ) ;

		return { json[ "finished" ].get< bool >(),
			 json[ "exitCode" ].get< int >(),
			 json[ "exitStatus" ].get< int >(),
			 json[ "stdError" ].get< std::string >().c_str(),
			 json[ "stdOut" ].get< std::string >().c_str() } ;
	}catch( ... ){
		return { false,255,255,"","" } ;
	}
}

#else

static QByteArray _json_command( const QByteArray& cookie,
				 const QByteArray& password,
				 const QString& exe )
{
	Q_UNUSED( cookie ) ;
	Q_UNUSED( password ) ;
	Q_UNUSED( exe ) ;

	return QByteArray() ;
}

static jsonResult _json_result( const QByteArray& e )
{
	Q_UNUSED( e ) ;
	return { false,255,255,"","" } ;
}

#endif

static int staticGlobalUserId = -1 ;
static QByteArray _cookie ;

::Task::future< utility::Task >& utility::Task::run( const QString& exe,bool e )
{
	return utility::Task::run( exe,-1,e ) ;
}

::Task::future< utility::Task >& utility::Task::run( const QString& exe,int s,bool e )
{
	return ::Task::run< utility::Task >( [ = ](){

		auto env = QProcessEnvironment::systemEnvironment() ;

		return utility::Task( exe,s,env,_cookie,[](){ umask( 0 ) ; },e ) ;
	} ) ;
}

void utility::Task::execute( const QString& exe,int waitTime,
			     const QProcessEnvironment& env,
			     const QByteArray& password,
			     const std::function< void() >& f,
			     bool polkit )
{
	class Process : public QProcess{
	public:
		Process( const std::function< void() >& f ) : m_function( f )
		{
		}
	protected:
		void setupChildProcess()
		{
			m_function() ;
		}
	private:
		std::function< void() > m_function ;
	} p( f ) ;

	p.setProcessEnvironment( env ) ;

	if( polkit && utility::useZuluPolkit() ){

		QLocalSocket s ;

		s.connectToServer( utility::helperSocketPath() ) ;

		for( int i = 0 ; ; i++ ){

			if( s.waitForConnected() ){

				break ;

			}else if( i == 10 ){

				utility::debug() << "ERROR: Failed To Start Helper Application" ;
				return ;
			}else{
				utility::debug() << s.errorString() ;

				utility::Task::suspendForOneSecond() ;
			}
		}

		s.write( _json_command( _cookie,password,exe ) ) ;

		s.waitForBytesWritten() ;

		s.waitForReadyRead() ;

		auto e = _json_result( s.readAll() ) ;

		m_finished   = e.finished ;
		m_exitCode   = e.exitCode ;
		m_exitStatus = e.exitStatus ;
		m_stdError   = e.stdError ;
		m_stdOut     = e.stdOut ;
	}else{
		p.start( exe ) ;

		if( !password.isEmpty() ){

			p.waitForStarted() ;

			p.write( password + '\n' ) ;

			p.closeWriteChannel() ;
		}

		m_finished   = p.waitForFinished( waitTime ) ;
		m_exitCode   = p.exitCode() ;
		m_exitStatus = p.exitStatus() ;
		m_stdOut     = p.readAllStandardOutput() ;
		m_stdError   = p.readAllStandardError() ;
	}
}

void utility::startHelperExecutable( QObject * obj,const QString& arg,const char * slot,const char * slot1 )
{
	if( !utility::useZuluPolkit() ){

		QMetaObject::invokeMethod( obj,slot,Q_ARG( bool,true ),Q_ARG( QString,QString() ) ) ;

		return ;
	}

	QFile f( "/dev/urandom" ) ;

	f.open( QIODevice::ReadOnly ) ;

	_cookie = f.read( 16 ).toHex() ;

	auto exe = utility::executableFullPath( "pkexec" ) ;

	if( !exe.isEmpty() ){

		exe = QString( "%1 %2 %3 fork ").arg( exe,zuluPolkitPath,utility::helperSocketPath() ) ;

		::Task::exec( [ = ](){

			auto e = utility::Task::run( exe,false ).get() ;

			QMetaObject::invokeMethod( obj,
						   slot,
						   Q_ARG( bool,e.success() ),
						   Q_ARG( QString,arg ) ) ;
		} ) ;
	}else{
		DialogMsg().ShowUIOK( QObject::tr( "ERROR" ),QObject::tr( "Failed to locate pkexec executable" ) ) ;
		QMetaObject::invokeMethod( obj,slot1 ) ;
	}
}

QString utility::helperSocketPath()
{
	auto z = utility::homePath() + "/.zuluCrypt-socket/" ;

	z += QCoreApplication::applicationName() + ".polkit.socket" ;

	return z ;
}

bool utility::useZuluPolkit()
{
	return POLKIT_SUPPORT ;
}

void utility::quitHelper()
{
	auto e = utility::helperSocketPath() ;

	if( utility::pathExists( e ) ){

		QLocalSocket s ;

		s.connectToServer( e ) ;

		if( s.waitForConnected() ){

			s.write( _json_command( _cookie,"","exit" ) ) ;

			s.waitForBytesWritten() ;
		}
	}
}

bool utility::reUseMountPointPath()
{
	return REUSE_MOUNT_POINT ;
}

bool utility::reUseMountPoint()
{
	return utility::reUseMountPointPath() ;
}

void utility::setUID( int uid )
{
	if( utility::userIsRoot() || utility::useZuluPolkit() ){

		staticGlobalUserId = uid ;
	}
}

int utility::getUID()
{
	return staticGlobalUserId ;
}

int utility::getUserID()
{
	if( staticGlobalUserId == -1 ){

		return getuid() ;
	}else{
		return staticGlobalUserId ;
	}
}

QString utility::getStringUserID()
{
	return QString::number( utility::getUserID() ) ;
}

QString utility::appendUserUID( const QString& e )
{
	if( staticGlobalUserId == -1 ){

		return e ;
	}else{
		return e + " -K " + utility::getStringUserID() ;
	}
}

static passwd * _getPassWd()
{
	return getpwuid( utility::getUserID() ) ;
}

QString utility::userName()
{
	return _getPassWd()->pw_name ;
}

QString utility::homePath()
{
	return getpwuid( utility::getUserID() )->pw_dir ;
}

static int _help()
{
	std::cout << "\n" << VERSION_STRING << std::endl ;

	QString helpMsg = QObject::tr( "\n\
options:\n\
	-d   path to where a volume to be auto unlocked/mounted is located\n\
	-m   tool to use to open a default file manager(default tool is xdg-open)\n\
	-e   start the application without showing the GUI\n" ) ;

	std::cout << helpMsg.toLatin1().constData() << std::endl ;

	return 0 ;
}

static bool _printHelpOrVersionInfo()
{
	QStringList q = QCoreApplication::arguments() ;
	return q.contains( "-h" )        ||
	       q.contains( "-help" )     ||
	       q.contains( "--help" )    ||
	       q.contains( "-v" )        ||
	       q.contains(  "-version" ) ||
	       q.contains( "--version" ) ;
}

int utility::startApplication( const char * appName,std::function<int()> start )
{
	QCoreApplication::setApplicationName( appName ) ;

	if( _printHelpOrVersionInfo() ){

		return _help() ;
	}else{
		return start() ;
	}
}

void utility::startApplication( QObject * s,const char * e )
{
	QMetaObject::invokeMethod( s,e,Qt::QueuedConnection ) ;
}

void utility::keySend( const QString& path,const QByteArray& key )
{
	::Task::exec( [ path,key ](){

		auto handle = ::zuluCryptPluginManagerOpenConnection( path.toLatin1().constData() ) ;

		if( handle ){

			size_t size = key.size() ;

			/*
			 * ZULUCRYPT_KEYFILE_MAX_SIZE is defined in ../zuluCrypt-cli/constants.h
			 * The variable holds the maximum keyfile size
			 */

			if( size > ZULUCRYPT_KEYFILE_MAX_SIZE ){

				size = ZULUCRYPT_KEYFILE_MAX_SIZE ;
			}

			::zuluCryptPluginManagerSendKey( handle,key.constData(),size ) ;
			::zuluCryptPluginManagerCloseConnection( handle ) ;
		}
	} ) ;
}

void utility::keySend( const QString& keyPath,const QString& key )
{
	utility::keySend( keyPath,key.toLatin1() ) ;
}

void utility::createPlugInMenu( QMenu * menu,const QString& a,const QString& b,const QString& c,bool addPlugIns )
{
	QStringList l ;
	QStringList e ;

	l.append( a ) ;

	if( utility::notRunningInMixedMode() ){

		if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::libsecret ) ){

			l.append( b ) ;
		}
		if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::kwallet ) ){

			l.append( c ) ;
		}
	}

	if( addPlugIns ){

		QDir dir( ZULUCRYPTpluginPath ) ;

		if( dir.exists() ){

			e = dir.entryList() ;

			e.removeOne( "zuluCrypt-testKey" ) ;
			e.removeOne( "." ) ;
			e.removeOne( ".." ) ;
			e.removeOne( "keyring" ) ;
			e.removeOne( "kwallet" ) ;
		}

		e.append( "network" ) ;
	}

	menu->clear() ;

	auto _add_actions = [ menu ]( const QStringList& r ){

		for( const auto& it : r ){

			menu->addAction( it ) ;
		}
	} ;

	_add_actions( l ) ;
	_add_actions( e ) ;
}

::Task::future<int>& utility::exec( const QString& exe )
{
	return ::Task::run<int>( [ exe ](){ return utility::Task( exe ).exitCode() ; } ) ;
}

::Task::future<QStringList>& utility::luksEmptySlots( const QString& volumePath )
{
	return ::Task::run<QStringList>( [ volumePath ](){

		auto e = utility::appendUserUID( "%1 -b -d \"%2\"" ) ;

		auto r = utility::Task( e.arg( ZULUCRYPTzuluCrypt,volumePath ) ) ;

		if( r.success() ){

			const auto& s = r.stdOut() ;

			int i = 0 ;

			for( const auto& it : s ){

				if( it == '1' || it == '3' ){

					i++ ;
				}
			}

			return QStringList{ QString::number( i ),QString::number( s.size() - 1 ) } ;
		}

		return QStringList() ;
	} ) ;
}

::Task::future<QString>& utility::getUUIDFromPath( const QString& dev )
{
	return ::Task::run<QString>( [ dev ](){

		auto device = dev ;
		device = device.replace( "\"", "\"\"\"" ) ;
		auto exe = utility::appendUserUID( "%1 -U -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt,device ) ;

		auto r = utility::Task( exe ) ;

		if( r.success() ){

			QString uuid = r.stdOut() ;
			uuid.remove( "\n" ) ;

			if( uuid == "UUID=\"\"" ){

				return QString() ;
			}else{
				return uuid ;
			}
		}else{
			return QString() ;
		}
	} ) ;
}

void utility::dropPrivileges( int uid )
{
	if( uid == -1 ){

		uid = utility::getUserID() ;
	}

	if( uid != -1 ){

		auto id = getpwuid( uid ) ;

		if( id ){

			setenv( "LOGNAME",id->pw_name,1 ) ;
			setenv( "HOME",id->pw_dir,1 ) ;
			setenv( "USER",id->pw_name,1 ) ;
		}

		Q_UNUSED( setgid( uid ) ) ;
		Q_UNUSED( setgroups( 1,reinterpret_cast< const gid_t * >( &uid ) ) ) ;
		Q_UNUSED( setegid( uid ) ) ;
		Q_UNUSED( setuid( uid ) ) ;
	}
}

::Task::future<bool>& utility::openPath( const QString& path,const QString& opener )
{
	return ::Task::run<bool>( [ = ](){

		auto e = opener + " " + utility::Task::makePath( path ) ;

		return utility::Task::run( e,false ).get().failed() ;
	} ) ;
}

void utility::openPath( const QString& path,const QString& opener,
			QWidget * obj,const QString& title,const QString& msg )
{
	openPath( path,opener ).then( [ title,msg,obj ]( bool failed ){

		if( failed && obj ){

			DialogMsg m( obj ) ;
			m.ShowUIOK( title,msg ) ;
		}
	} ) ;
}

static ::Task::future<QString>& _getKey( LXQt::Wallet::Wallet& wallet,const QString& volumeID )
{
	return ::Task::run<QString>( [ & ](){

		decltype( wallet.readValue( volumeID ) ) key ;

		if( volumeID.startsWith( "UUID=" ) ){

			key = wallet.readValue( volumeID ) ;
		}else{
			auto uuid = utility::getUUIDFromPath( volumeID ).get() ;

			if( uuid.isEmpty() ){

				key = wallet.readValue( utility::getVolumeID( volumeID ) ) ;
			}else{
				key = wallet.readValue( uuid ) ;

				if( key.isEmpty() ){

					key = wallet.readValue( volumeID ) ;
				}
			}
		}

		return key ;
	} ) ;
}

utility::wallet utility::getKey( LXQt::Wallet::Wallet& wallet,const QString& keyID,const QString& app )
{
	utility::wallet w{ false,false,"" } ;

	auto s = wallet.backEnd() ;

	if( s == LXQt::Wallet::BackEnd::kwallet || s == LXQt::Wallet::BackEnd::libsecret ){

		if( s == LXQt::Wallet::BackEnd::kwallet ){

			w.opened = wallet.open( "default",utility::applicationName() ) ;
		}else{
			w.opened = wallet.open( utility::walletName(),utility::applicationName() ) ;
		}

		if( w.opened ){

			w.key = _getKey( wallet,keyID ).await() ;
		}

	}else if( s == LXQt::Wallet::BackEnd::internal ){

		auto walletName = utility::walletName() ;
		auto appName    = utility::applicationName() ;

		if( LXQt::Wallet::walletExists( s,walletName,appName ) ){

			wallet.setImage( utility::getIcon( app ) ) ;

			if( wallet.opened() ){

				w.opened = true ;
			}else{
				w.opened = wallet.open( walletName,appName ) ;
			}

			if( w.opened ){

				w.key = _getKey( wallet,keyID ).await() ;
				w.notConfigured = false ;
			}
		}else{
			w.notConfigured = true ;
		}
	}

	return w ;
}

static quint64 _volumeSize( const QString& e,size_t size )
{
	utility::fileHandle h ;

	if( size == 0 ){

		if( h.open( e ) ){

			return h.size() ;
		}else{
			return 0 ;
		}
	}else{
		return size ;
	}
}

static int _openVolume( const QString& e )
{
	return open( e.toLatin1().constData(),O_RDWR ) ;
}

static std::function< void( int ) > _closeFunction( const QString& e )
{
	return [ & ]( int fd ){

		if( fd != -1 ){

			for( int i = 0 ; i < 5 ; i++ ){

				if( close( fd ) == 0 ){

					utility::Task( utility::appendUserUID( "%1 -q -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt,e ) ) ;

					break ;
				}
			}
		}
	} ;
}

static bool _writeToVolume( int fd,const char * buffer,unsigned int bufferSize )
{
	return write( fd,buffer,bufferSize ) != -1 ;
}

::Task::future< int >& utility::clearVolume( const QString& volume,std::atomic_bool * exit,size_t volumeSize,std::function< void( int ) > function )
{
	return ::Task::run<int>( [ volume,exit,volumeSize,function ](){

		auto volumePath = volume ;

		volumePath.replace( "\"","\"\"\"" ) ;

		int r = utility::Task( utility::appendUserUID( "%1 -k -J -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt,volumePath ) ).exitCode() ;

		if( r != 0 ){

			return r ;
		}else{
			auto volumeMapperPath = utility::mapperPath( volume ) ;

			utility::fileHandle f( _openVolume( volumeMapperPath ),_closeFunction( volumePath ) ) ;

			int fd = f.handle() ;

			if( fd == -1 ){

				return 1 ;
			}else{
				int i = 0 ;
				int j = 0 ;

				const int bufferSize = 1024 ;

				char buffer[ bufferSize ] ;

				quint64 size         = _volumeSize( volumeMapperPath,volumeSize ) ;
				quint64 size_written = 0 ;

				if( size == 0 ){

					return 0 ;
				}

				while( _writeToVolume( fd,buffer,bufferSize ) ){

					if( *exit ){
						/*
						 * erasedevice::taskResult() has info on why we return 5 here.
						 */
						return 5 ;
					}else{
						size_written += bufferSize ;

						i = int( ( size_written * 100 / size ) ) ;

						if( i > j ){

							function( i ) ;

							j = i ;
						}

						if( size_written >= size ){

							break ;
						}
					}
				}
			}

			return 0 ;
		}
	} ) ;
}

QString utility::cryptMapperPath()
{
	//return QString( crypt_get_dir() )
	return "/dev/mapper/" ;
}

bool utility::userIsRoot()
{
	return getuid() == 0 ;
}

QString utility::shareMountPointToolTip()
{
	QString x ;
#if USE_HOME_PATH_AS_MOUNT_PREFIX
	x = QDir::homePath() + "/" ;
#else
	x = "/run/media/private/" ;
#endif
	return QObject::tr( "\
If the option is checked,a primary private mount point will be created in \"%1\"\n\
and a secondary publicly accessible \"mirror\" mount point will be created in \"%2\"" ).arg( x,SHARE_MOUNT_PREFIX "/" ) ;
}

QString utility::shareMountPointToolTip( const QString& path )
{
	auto s = QString( SHARE_MOUNT_PREFIX "/" ) + path.split( "/" ).last() ;

	if( QFile::exists( s ) ){

		return QObject::tr( "public mount point: " ) + s ;
	}else{
		return QString() ;
	}
}

QString utility::sharedMountPointPath( const QString& path )
{
	if( path == "/" ){

		return QString() ;
	}else{
		auto s = SHARE_MOUNT_PREFIX "/" + path.split( "/" ).last() ;

		if( QFile::exists( s ) ){

			return s ;
		}else{
			return QString() ;
		}
	}
}

bool utility::pathPointsToAFile( const QString& path )
{
	utility::fileHandle h ;

	if( h.open( path ) ){

		return h.isFile() ;
	}else{
		return false ;
	}
}

bool utility::pathPointsToAFolder( const QString& path )
{
	utility::fileHandle h ;

	if( h.open( path ) ){

		return h.isFolder() ;
	}else{
		return false ;
	}
}

static QString _language_path( const QString& program )
{
	return utility::homePath() + "/.zuluCrypt/" + program + ".lang" ;
}

QString utility::localizationLanguage( const QString& program )
{
	QFile f( _language_path( program ) ) ;

	if( f.open( QIODevice::ReadOnly ) ){

		QString e = f.readAll() ;

		e.remove( "\n" ) ;

		return e ;
	}else{
		return "en_US" ;
	}
}

void utility::setLocalizationLanguage( const QString& program,const QString& language )
{
	QFile f( _language_path( program ) ) ;

	if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

		f.write( language.toLatin1() ) ;

		utility::changePathOwner( f ) ;
		utility::changePathPermissions( f ) ;
	}
}

QString utility::localizationLanguagePath( const QString& program )
{
	return QString( TRANSLATION_PATH ) + program ;
}


QString utility::walletName()
{
	return "zuluCrypt" ;
}

QString utility::applicationName()
{
	return "zuluCrypt" ;
}

bool utility::pathIsReadable( const QString& path )
{
	utility::fileHandle h ;

	return h.open( path ) ;
}

bool utility::pathIsWritable( const QString& path )
{
	utility::fileHandle h ;

	if( h.open( path,false ) ){

		h.unlink() ;

		return true ;
	}else{
		return false ;
	}
}

bool utility::setOpenVolumeReadOnly( QWidget * parent,bool checked,const QString& app )
{
	return readOnlyWarning::showWarning( parent,checked,app ) ;
}

bool utility::getOpenVolumeReadOnlyOption( const QString& app )
{
	return readOnlyWarning::getOpenVolumeReadOnlyOption( app ) ;
}

QString utility::keyPath()
{
	QFile f( "/dev/urandom" ) ;

	f.open( QIODevice::ReadOnly ) ;

	QByteArray data = f.read( 64 ) ;

	QString a = utility::homePath() ;

	return QString( "%1/.zuluCrypt-socket/%2" ).arg( a,utility::hashPath( data ).mid( 1 ) ) ;
}

bool utility::eventFilter( QObject * gui,QObject * watched,QEvent * event,std::function< void() > function )
{
	if( watched == gui ){

		if( event->type() == QEvent::KeyPress ){

			auto keyEvent = static_cast< QKeyEvent* >( event ) ;

			if( keyEvent->key() == Qt::Key_Escape ){

				function() ;

				return true ;
			}
		}
	}

	return false ;
}

QStringList utility::split( const QString& str,char token )
{
	return str.split( token,QString::SkipEmptyParts ) ;
}

QStringList utility::split( const QByteArray& str,char token )
{
	return QString( str ).split( token,QString::SkipEmptyParts ) ;
}

bool utility::mapperPathExists( const QString& path )
{
	return utility::pathExists( utility::mapperPath( path ) ) ;
}

QString utility::mountPath( const QString& path )
{
	auto pass = _getPassWd() ;

#if USE_HOME_PATH_AS_MOUNT_PREFIX
	return QString( "%1/%2" ).arg( QString( pass->pw_dir ) ).arg( path ) ;
#else
	return QString( "/run/media/private/%1/%2" ).arg( QString( pass->pw_dir ).split( "/" ).last(),path ) ;
#endif
}

QString utility::homeMountPath( const QString& path )
{
	return QString( "%1/%2" ).arg( _getPassWd()->pw_dir,path ) ;
}

QString utility::mountPathPostFix( const QString& path )
{
	if( path.isEmpty() ){

		return path ;
	}else{
		auto _usable_mount_point = []( const QString& e ){

			if( utility::reUseMountPointPath() ){

				if( utility::pathExists( e ) ){

					return utility::pathPointsToAFolder( e ) ;
				}else{
					return true ;
				}

			}else{
				return !utility::pathExists( e ) ;
			}
		} ;

		auto e = utility::mountPath( path ) ;

		if( _usable_mount_point( e ) ){

			return path ;
		}else{
			QString z ;

			for( int i = 1 ; i < 1000 ; i++ ){

				z = QString::number( i ) ;

				if( _usable_mount_point( QString( "%1_%2" ).arg( e,z ) ) ){

					return QString( "%1_%2" ).arg( path,z ) ;
				}
			}

			return path ;
		}
	}
}

QString utility::mapperPath( const QString& r,const QString& component )
{
	auto rpath = r ;

	auto path = utility::cryptMapperPath() + "zuluCrypt-" + utility::getStringUserID() ;

	if( rpath.startsWith( "UUID=" ) ){

		rpath.remove( '\"' ) ;

		rpath.replace( "UUID=","UUID-" ) ;

		path += "-" + rpath + utility::hashPath( rpath.toLatin1() ) ;
	}else{
		if( component.isEmpty() ){

			path += "-NAAN-" + rpath.split( "/" ).last() + utility::hashPath( rpath.toLatin1() ) ;
		}else{
			path += component + rpath.split( "/" ).last() + utility::hashPath( rpath.toLatin1() ) ;
		}
	}

	for( const auto& it : BASH_SPECIAL_CHARS ){

		path.replace( it,'_' ) ;
	}
	return path ;
}

QString utility::hashPath( const QByteArray& p )
{
	int l = p.size() ;

	uint32_t hash = 0 ;

	auto key = p.constData() ;

	for( int i = 0 ; i < l ; i++ ){

		hash += *( key + i ) ;

		hash += ( hash << 10 ) ;

		hash ^= ( hash >> 6 ) ;
	}

	hash += ( hash << 3 ) ;

	hash ^= ( hash >> 11 ) ;

	hash += ( hash << 15 ) ;

	return "-" + QString::number( hash ) ;
}

bool utility::pathExists( const QString& path )
{
	return QFile::exists( path ) ;
}

bool utility::canCreateFile( const QString& path )
{
	return utility::pathIsWritable( path ) ;
}

QString utility::resolvePath( const QString& path )
{
	if( path.size() == 1 && path.at( 0 ) == QChar( '~' ) ){

		return utility::homePath() + "/" ;

	}else if( path.startsWith( "~/" ) ){

		return utility::homePath() + "/" + path.mid( 2 ) ;

	}else if( path.startsWith( "UUID=") ){

		return path ;

	}else if( path.startsWith( "/dev/" ) ){

		return path ;

	}else if( path.startsWith( "file://" ) ){

		return path.mid( 7 ) ;
	}else{
		QDir r( path ) ;

		auto rp = r.canonicalPath() ;

		if( rp.isEmpty() ) {

			return path ;
		}else{
			return rp ;
		}
	}
}

QString utility::executableFullPath( const QString& f )
{
	QString e = f ;

	if( e.startsWith( "/" ) ){

		auto s =  QDir( f ).canonicalPath() ;

		for( const auto& it : utility::executableSearchPaths() ){

			if( s.startsWith( it ) ){

				return s ;
			}
		}

		return QString() ;
	}

	if( e == "ecryptfs" ){

		e = "ecryptfs-simple" ;
	}

	QString exe ;

	for( const auto& it : utility::executableSearchPaths() ){

		exe = it + e ;

		if( utility::pathExists( exe ) ){

			return exe ;
		}
	}

	return QString() ;
}


static QString _absolute_exe_path( const QString& exe )
{
	auto e = utility::executableFullPath( exe ) ;

	if( e.isEmpty() ){

		return exe ;
	}else{
		return e ;
	}
}

QString utility::cmdArgumentValue( const QStringList& l,const QString& arg,const QString& defaulT )
{
	int j = l.size() ;

	for( int i = 0 ; i < j ; i++ ){

		if( l.at( i ) == arg ){

			auto e = [ & ](){

				if( i + 1 < j ){

					return l.at( i + 1 ) ;
				}else{
					return defaulT ;
				}
			} ;

			if( arg == "-m" ){

				return _absolute_exe_path( e() ) ;
			}else{
				return e() ;
			}
		}
	}

	return defaulT ;
}

static QString _device_id_to_partition_id( const QString& id )
{
	if( id.startsWith( "/dev/disk/by-id" ) ){

		auto l = id.split( '\t' ) ;

		if( l.size() > 1 ){

			QDir d( l.first() ) ;

			auto e = d.canonicalPath() ;

			if( e.isEmpty() ){

				return l.first() + '\t' + l.at( 1 ) ;
			}else{
				return e + '\t' + l.at( 1 ) ;
			}
		}else{
			return id ;
		}
	}else{
		return id ;
	}
}

static QString _partition_id_to_device_id( const QString& id,bool expand )
{
	if( id.startsWith( "/dev/" ) ){

		auto l = utility::directoryList( "/dev/disk/by-id" ) ;

		QDir r ;

		for( const auto& it : l ){

			const auto& e = it ;

			if( !e.startsWith( "dm" ) ){

				auto q = QString( "/dev/disk/by-id/%1" ).arg( e ) ;

				r.setPath( q ) ;

				if( r.canonicalPath() == id ){

					if( expand ){

						return q ;
					}else{
						return e ;
					}
				}
			}
		}
		return id ;
	}else{
		return id ;
	}
}

QString utility::getVolumeID( const QString& id,bool expand )
{
	return _partition_id_to_device_id( id,expand ) ;
}

void utility::addToFavorite( const QString& dev,const QString& m_point )
{
	if( !( dev.isEmpty() || m_point.isEmpty() ) ){

		auto fav = QString( "%1\t%2\n" ).arg( _partition_id_to_device_id( dev,true ),m_point ) ;

		QFile f( utility::homePath() + "/.zuluCrypt/favorites" ) ;

		f.open( QIODevice::WriteOnly | QIODevice::Append ) ;

		f.write( fav.toLatin1() ) ;

		utility::changePathOwner( f ) ;
		utility::changePathPermissions( f ) ;
	}
}

QStringList utility::readFavorites()
{
	QFile f( utility::homePath() + "/.zuluCrypt/favorites" ) ;

	if( f.open( QIODevice::ReadOnly ) ){

		QStringList l ;

		for( const auto& it : utility::split( f.readAll() ) ){

			if( it.startsWith( "/dev/disk/by-id" ) ){

				l.append( _device_id_to_partition_id( it ) ) ;
			}else{
				l.append( it ) ;
			}
		}

		utility::changePathOwner( f ) ;
		utility::changePathPermissions( f ) ;

		return l ;
	}else{
		return QStringList() ;
	}
}

void utility::removeFavoriteEntry( const QString& entry )
{
	auto l = utility::readFavorites() ;

	l.removeOne( entry ) ;

	QFile f( utility::homePath() + "/.zuluCrypt/favorites" ) ;

	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;

	for( const auto& it : l ){

		auto e = it.split( '\t' ) ;

		auto q = QString( "%1\t%2\n" ).arg( _partition_id_to_device_id( e.at( 0 ),true ),e.at( 1 ) ) ;

		f.write( q.toLatin1() ) ;
	}

	utility::changePathPermissions( f ) ;
	utility::changePathOwner( f ) ;
}

void utility::readFavorites( QMenu * m,bool truncate,bool showFolders )
{
	m->clear() ;

	auto _add_action = [ m,truncate ]( const QString& e ){

		auto ac = new QAction( m ) ;

		if( truncate ){

			auto l = utility::split( e,'\t' ) ;

			if( l.size() > 0 ){

				ac->setText( l.first() ) ;
			}
		}else{
			ac->setText( e ) ;
		}

		ac->setEnabled( !e.startsWith( "/dev/disk/by-id" ) ) ;

		return ac ;
	} ;

	m->addAction( new QAction( QObject::tr( "Manage Favorites" ),m ) ) ;

	m->addAction( new QAction( QObject::tr( "Mount All" ),m ) ) ;

	m->addSeparator() ;

	for( const auto& it : utility::readFavorites() ){

		if( showFolders ){

			m->addAction( _add_action( it ) ) ;
		}else{
			auto e = utility::split( it,'\t' ).first() ;

			if( utility::pathExists( e ) ){

				if( !utility::pathPointsToAFolder( e ) ){

					m->addAction( _add_action( it ) ) ;
				}
			}
		}
	}
}

int utility::favoriteClickedOption( const QString& opt )
{
	if( opt == QObject::tr( "Manage Favorites" ) ){

		return 1 ;

	}else if( opt == QObject::tr( "Mount All" ) ){

		return 2 ;
	}else{
		return 3 ;
	}
}

bool utility::userHasGoodVersionOfWhirlpool()
{
#ifdef GCRYPT_VERSION_NUMBER
	return GCRYPT_VERSION_NUMBER >= 0x010601 && SUPPORT_WHIRLPOOL ;
#else
	return SUPPORT_WHIRLPOOL ;
#endif
}

void utility::licenseInfo( QWidget * parent )
{
	QString license = QString( "%1\n\n\
This program is free software: you can redistribute it and/or modify \
it under the terms of the GNU General Public License as published by \
the Free Software Foundation, either version 2 of the License, or \
( at your option ) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <http://www.gnu.org/licenses/>." ).arg( VERSION_STRING ) ;

	DialogMsg m( parent ) ;
	m.ShowUIInfo( QObject::tr( "about zuluCrypt" ),false,license ) ;
}

static utility::array_t _default_dimensions( const char * defaults )
{
	auto l = QString( defaults ).split( ' ' ) ;

	utility::array_t e ;

	auto f = e.data() ;

	auto j = l.size() ;

	for( int i = 0 ; i < j ; i++ ){

		*( f + i ) = l.at( i ).toInt() ;
	}

	return e ;
}

static utility::array_t _dimensions( const QString& path,const char * defaults,int size )
{
	QFile f( path ) ;

	if( !f.exists() ){

		if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

			utility::changePathPermissions( f ) ;

			f.write( defaults ) ;

			utility::changePathPermissions( f ) ;
			utility::changePathOwner( f ) ;

			f.close() ;
		}else{
			qDebug() << "failed to open config file" ;
			return _default_dimensions( defaults ) ;
		}
	}

	if( f.open( QIODevice::ReadOnly ) ){

		auto l = utility::split( f.readAll(),' ' ) ;

		utility::array_t p ;

		if( l.size() != size || size > int( p.size() ) ){

			qDebug() << "failed to parse config file" ;
			return _default_dimensions( defaults ) ;
		}

		auto f = p.data() ;

		auto j = l.size() ;

		for( int i = 0 ; i < j ; i++ ){

			bool ok ;

			int e = l.at( i ).toInt( &ok ) ;

			if( ok ){

				*( f + i ) = e ;
			}else{
				qDebug() << "failed to parse config file option" ;
				return _default_dimensions( defaults ) ;
			}
		}

		return p ;
	}else{
		qDebug() << "failed to open config file" ;
		return _default_dimensions( defaults ) ;
	}
}

utility::array_t utility::getWindowDimensions( const QString& application )
{
	auto path = utility::homePath() + "/.zuluCrypt/" + application + "-gui-ui-options" ;

	if( application == "zuluCrypt" ){

		return _dimensions( path,"297 189 782 419 298 336 100",7 ) ;
	}else{
		return _dimensions( path,"205 149 910 477 220 320 145 87 87",9 ) ;
	}
}

void utility::setWindowDimensions( const QString& application,const std::initializer_list<int>& e )
{
	auto path = utility::homePath() + "/.zuluCrypt/" + application + "-gui-ui-options" ;

	QFile f( path ) ;

	if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

		utility::changePathPermissions( f ) ;

		for( const auto& it : e ){

			f.write( QString( QString::number( it ) + " " ).toLatin1() ) ;

			utility::changePathPermissions( f ) ;
			utility::changePathOwner( f ) ;
		}
	}
}

QFont utility::getFont( QWidget * widget )
{
	QString fontPath = utility::homePath() + "/.zuluCrypt/font" ;

	QFile x( fontPath ) ;

	if( x.open( QIODevice::ReadOnly ) ){

		auto l = utility::split( x.readAll() ) ;

		if( l.size() >= 4 ){

			QFont F ;

			const QString& fontFamily = l.at( 0 ) ;
			const QString& fontSize   = l.at( 1 ) ;
			const QString& fontStyle  = l.at( 2 ) ;
			const QString& fontWeight = l.at( 3 ) ;

			F.setFamily( fontFamily ) ;

			F.setPointSize( fontSize.toInt() ) ;

			if( fontStyle == "normal" ){

				F.setStyle( QFont::StyleNormal ) ;

			}else if( fontStyle == "italic" ){

				F.setStyle( QFont::StyleItalic ) ;
			}else{
				F.setStyle( QFont::StyleOblique ) ;
			}

			if( fontWeight == "normal" ){

				F.setWeight( QFont::Normal ) ;
			}else{
				F.setWeight( QFont::Bold ) ;
			}

			return F ;
		}else{
			return widget->font() ;
		}
	}else{
		return widget->font() ;
	}
}

void utility::saveFont( const QFont& Font )
{
	QFile f( utility::homePath() + "/.zuluCrypt/font" ) ;

	if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

		utility::changePathOwner( f ) ;
		utility::changePathPermissions( f ) ;

		auto s = QString( "%1\n%2\n" ).arg( Font.family(),QString::number( Font.pointSize() ) ) ;

		if( Font.style() == QFont::StyleNormal ){

			s = s + "normal\n" ;

		}else if( Font.style() == QFont::StyleItalic ){

			s = s + "italic\n" ;
		}else{
			s = s + "oblique\n" ;
		}

		if( Font.weight() == QFont::Normal ){

			s = s + "normal\n" ;
		}else{
			s = s + "bold" ;
		}

		f.write( s.toLatin1() ) ;

		utility::changePathPermissions( f ) ;
		utility::changePathOwner( f ) ;
	}
}

bool utility::runningInMixedMode()
{
	return utility::useZuluPolkit() ;
}

bool utility::notRunningInMixedMode()
{
	return !utility::runningInMixedMode() ;
}

bool utility::userBelongsToGroup( const char * groupname )
{
	auto user = getpwuid( utility::getUserID() ) ;

	if( user != nullptr ){

		auto grp = getgrnam( groupname ) ;

		if( grp != nullptr ){

			auto name  = user->pw_name ;

			for( auto e = grp->gr_mem ; *e ; e++ ){

				if( strcmp( *e,name ) == 0 ){

					return true ;
				}
			}
		}
	}

	return false ;
}

int utility::pluginKey( QWidget * w,QByteArray * key,const QString& p )
{
	plugins::plugin pluginType ;
	QString pluginString ;
	QVector<QString> exe ;

	if( p == "hmac" ){

		pluginType   = plugins::plugin::hmac_key ;
		pluginString = QObject::tr( "hmac plugin.\n\nThis plugin generates a key using below formular:\n\nkey = hmac(sha256,passphrase,keyfile contents)" ) ;

	}else if( p == "keykeyfile" ){

		pluginType   = plugins::plugin::keyKeyFile ;
		pluginString = QObject::tr( "keykeyfile plugin.\n\nThis plugin generates a key using below formular:\n\nkey = passphrase + keyfile contents" ) ;

	}else if( p == "gpg" ){

		pluginType   = plugins::plugin::gpg ;
		pluginString = QObject::tr( "gpg plugin.\n\nThis plugin retrives a key locked in a gpg file with a symmetric key" ) ;

		if( utility::pathExists( "/usr/bin/gpg" ) ){

			exe.append( "/usr/bin/gpg" ) ;

		}else if( utility::pathExists( "/usr/local/bin/gpg" ) ){

			exe.append( "/usr/local/bin/gpg" ) ;

		}else if( utility::pathExists( "/usr/sbin/gpg" ) ){

			exe.append( "/usr/sbin/gpg" ) ;

		}else{

			DialogMsg msg( w ) ;

			msg.ShowUIOK( QObject::tr( "ERROR" ),QObject::tr( "Could not find \"gpg\" executable in \"/usr/local/bin\",\"/usr/bin\" and \"/usr/sbin\"" ) ) ;

			return 1 ;
		}

	}else{
		return 1 ;
	}

	QEventLoop l ;

	plugin::instance( w,pluginType,[ & ]( const QByteArray& e ){

		*key = e ;

		if( e.isEmpty() ){

			l.exit( 1 ) ;
		}else{
			l.exit( 0 ) ;
		}

	},pluginString,exe ) ;

	return l.exec() ;
}

void utility::showTrayIcon( QAction * ac,QSystemTrayIcon * trayIcon,bool zuluCrypt )
{
	Q_UNUSED( zuluCrypt ) ;

	QFile f( utility::homePath() + "/.zuluCrypt/tray" ) ;

	if( !f.exists() ){

		f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
		f.write( "1" ) ;
		f.close() ;
	}

	f.open( QIODevice::ReadOnly ) ;

	char c ;

	f.read( &c,1 ) ;

	ac->setCheckable( true ) ;

	if( c == '1' ){

		ac->setChecked( true ) ;
		trayIcon->show() ;
	}else{
		ac->setChecked( false ) ;
		trayIcon->hide() ;
	}

	utility::changePathPermissions( f ) ;
	utility::changePathOwner( f ) ;
}

void utility::trayProperty( QSystemTrayIcon * trayIcon,bool zuluCrypt )
{
	Q_UNUSED( zuluCrypt ) ;

	QFile f( utility::homePath() + "/.zuluCrypt/tray" ) ;

	f.open( QIODevice::ReadOnly ) ;

	char c ;

	f.read( &c,1 ) ;

	f.close() ;

	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;

	if( c == '1' ){

		f.write( "0" ) ;
		trayIcon->hide() ;
	}else{
		f.write( "1" ) ;
		trayIcon->show() ;
	}

	utility::changePathPermissions( f ) ;
	utility::changePathOwner( f ) ;
}

class translator
{
public:
	void set( const QString& app,const QByteArray& r,int s )
	{
		QCoreApplication::installTranslator( [ & ](){

			auto f = m_translator.data() ;
			auto e = *( f + s ) ;

			if( e ){

				QCoreApplication::removeTranslator( e ) ;

				delete e ;
			}

			e = new QTranslator() ;

			e->load( r.constData(),utility::localizationLanguagePath( app ) ) ;

			*( f + s ) = e ;

			return e ;
		}() ) ;
	}
	~translator()
	{
		for( auto e : m_translator ){

			if( e ){

				/*
				 * QApplication appear to already be gone by the time we get here.
				 */
				//QCoreApplication::removeTranslator( e ) ;

				delete e ;
			}
		}
	}

private:
	std::array< QTranslator *,2 > m_translator = { { nullptr,nullptr } } ;
} static _translator ;

static void _selectOption( QMenu * m,const QString& opt )
{
	utility::selectMenuOption s( m,false ) ;
	s.selectOption( opt ) ;
}

void utility::setLocalizationLanguage( bool translate,QMenu * m,const QString& app )
{
	auto r = utility::localizationLanguage( app ).toLatin1() ;

	if( translate ){

		_translator.set( app,r,0 ) ;

		if( app == "zuluMount-gui" ){

			/*
			 * We are loading zuluCrypt-gui translation file to get translations for
			 * lxqtwallet strings.
			 */
			_translator.set( "zuluCrypt-gui",r,1 ) ;
		}
	}else{
		QDir d( utility::localizationLanguagePath( app ) ) ;

		auto t = d.entryList() ;

		if( !t.isEmpty() ){

			t.removeOne( "." ) ;
			t.removeOne( ".." ) ;

			for( auto& it : t ){

				m->addAction( it.remove( ".qm" ) )->setCheckable( true ) ;
			}
		}

		_selectOption( m,r ) ;
	}
}

void utility::languageMenu( QWidget * w,QMenu * m,QAction * ac,const char * app )
{
	auto e = ac->text() ;

	e.remove( "&" ) ;

	utility::setLocalizationLanguage( app,e ) ;

	utility::setLocalizationLanguage( true,m,app ) ;

	_selectOption( m,e ) ;

	return ;

	DialogMsg msg( w ) ;

	msg.ShowUIOK( QObject::tr( "INFO" ),QObject::tr( "Translation will be done the next time you restart." ) ) ;
}

QStringList utility::directoryList( const QString& e )
{
	QDir d( e ) ;

	auto l = d.entryList() ;

	l.removeOne( "." ) ;
	l.removeOne( ".." ) ;

	return l ;
}

static QString _iconNamePath( const QString& app )
{
	return utility::homePath() + "/.zuluCrypt/" + app + ".iconName" ;
}

void utility::setIconMenu( const QString& app,QAction * ac,QWidget * w,
			   std::function< void( const QString& ) >&& function )
{
	ac->setMenu( [ & ](){

		auto m = new QMenu( w ) ;

		auto n = new utility::selectMenuOption( m,true,std::move( function ) ) ;

		w->connect( m,SIGNAL( triggered( QAction * ) ),n,SLOT( selectOption( QAction * ) ) ) ;

		if( !QFile::exists( _iconNamePath( app ) ) ){

			utility::setIcons( app,app ) ;
		}

		QFile f( _iconNamePath( app ) ) ;

		f.open( QIODevice::ReadOnly ) ;

		QString s = f.readAll() ;

		QDir d( INSTALL_PREFIX "/share/icons/hicolor/48x48/apps/" ) ;

		d.setNameFilters( { "zuluCrypt.*","zuluMount.*" } ) ;

		for( auto& it : d.entryList() ){

			if( it.startsWith( app ) ){

				if( it != "zuluCrypt.png" && it != "zuluMount.png" ){

					it.remove( app + "." ) ;
				}

				it.remove( ".png" ) ;

				auto ac = m->addAction( it ) ;

				ac->setCheckable( true ) ;

				ac->setChecked( it == s ) ;
			}
		}

		return m ;
	}() ) ;
}

void utility::setIcons( const QString& app,const QString& iconName )
{
	QFile f( _iconNamePath( app ) ) ;

	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;

	f.write( iconName.toLatin1() ) ;

	utility::changePathOwner( f ) ;
	utility::changePathPermissions( f ) ;
}

QIcon utility::getIcon( const QString& app )
{
	if( !QFile::exists( _iconNamePath( app ) ) ){

		utility::setIcons( app,app ) ;
	}

	QFile f( _iconNamePath( app ) ) ;

	if( f.open( QIODevice::ReadOnly ) ){

		QString e = f.readAll() ;

		if( e == "zuluCrypt" || e == "zuluMount" ){

			QIcon icon( INSTALL_PREFIX "/share/icons/hicolor/48x48/apps/" + e + ".png" ) ;
			return QIcon::fromTheme( app,icon ) ;
		}else{
			return QIcon( INSTALL_PREFIX "/share/icons/hicolor/48x48/apps/" + app + "." + e + ".png" ) ;
		}
	}else{
		QIcon icon( INSTALL_PREFIX "/share/icons/hicolor/48x48/apps/" + app + ".png" ) ;

		return QIcon::fromTheme( app,icon ) ;
	}
}

static QString _veraCryptOptionPath( const QString& app )
{
	return utility::homePath() + "/.zuluCrypt/" + app + ".autoSetVolumeAsVeraCrypt" ;
}

bool utility::autoSetVolumeAsVeraCrypt( const QString& app )
{
	return utility::pathExists( _veraCryptOptionPath( app ) ) ;
}

void utility::autoSetVolumeAsVeraCrypt( const QString& app,bool set )
{
	if( set ){

		QFile f( _veraCryptOptionPath( app ) ) ;
		f.open( QIODevice::WriteOnly ) ;
	}else{
		QFile::remove( _veraCryptOptionPath( app ) ) ;
	}
}

static QString _auto_open_config_path( const QString& app )
{
	return utility::homePath() + "/.zuluCrypt/" + app + "-gui.NoAutoOpenFolder" ;
}

void utility::autoOpenFolderOnMount( const QString& app,bool e )
{
	auto x = _auto_open_config_path( app ) ;

	if( e ){

		QFile::remove( x ) ;
	}else{
		QFile( x ).open( QIODevice::WriteOnly ) ;
	}
}

bool utility::autoOpenFolderOnMount( const QString& app )
{
	return !QFile::exists( _auto_open_config_path( app ) ) ;
}

QString utility::powerOffCommand()
{
	QFile f( utility::homePath() + "/.zuluCrypt/power-off-command" ) ;

	QString e ;

	if( f.open( QIODevice::ReadOnly ) ){

		e = f.readAll() ;

		e.remove( '\n' ) ;
	}

	return e ;
}

QString utility::prettyfySpaceUsage( quint64 s )
{
	auto _convert = [ & ]( const char * p,double q ){

		auto e = QString::number( double( s ) / q,'f',2 ) ;

		e.remove( ".00" ) ;

		return QString( "%1 %2" ).arg( e,p ) ;
	} ;

	switch( QString::number( s ).size() ){

		case 0 :
		case 1 : case 2 : case 3 :

			return QString( "%1 B" ).arg( QString::number( s ) ) ;

		case 4 : case 5 : case 6 :

			return _convert( "KB",1024 ) ;

		case 7 : case 8 : case 9 :

			return _convert( "MB",1048576 ) ;

		case 10: case 11 : case 12 :

			return _convert( "GB",1073741824 ) ;

		default:
			return _convert( "TB",1024.0 * 1073741824 ) ;
	}
}

void utility::createHomeFolder()
{
	utility::createFolderPath( utility::homePath() + "/.zuluCrypt/" ) ;
}

void utility::createFolderPath( const QString& e )
{
	QDir().mkdir( e ) ;

	utility::changePathOwner( e ) ;
	utility::changePathPermissions( e,0777 ) ;
}

QStringList utility::plainDmCryptOptions()
{
	const QByteArray _options = R"(aes.cbc-essiv:sha256.256.ripemd160
aes.cbc-essiv:sha256.256.sha256
aes.cbc-essiv:sha256.256.sha512
aes.cbc-essiv:sha256.256.sha1
aes.cbc-essiv:sha256.512.ripemd160
aes.cbc-essiv:sha256.512.sha256
aes.cbc-essiv:sha256.512.sha512
aes.cbc-essiv:sha256.512.sha1
aes.xts-plain64.256.ripemd160
aes.xts-plain64.256.sha256
aes.xts-plain64.256.sha512
aes.xts-plain64.256.sha1
aes.xts-plain64.512.ripemd160
aes.xts-plain64.512.sha256
aes.xts-plain64.512.sha512
aes.xts-plain64.512.sha1)" ;

	QFile f( utility::homePath() + "/.zuluCrypt/plainDmCryptOptions" ) ;

	if( !f.exists() ){

		if( f.open( QIODevice::WriteOnly ) ){

			f.write( _options ) ;

			utility::changePathOwner( f ) ;
			utility::changePathPermissions( f,0666 ) ;

			f.close() ;
		}
	}

	if( f.open( QIODevice::ReadOnly ) ){

		return utility::split( f.readAll() ) ;
	}

	return QStringList() ;
}

QStringList utility::supportedFileSystems()
{
	QFile f( utility::homePath() + "/.zuluCrypt/supportedFileSystems" ) ;

	if( !f.exists() ){

		if( f.open( QIODevice::WriteOnly ) ){

			f.write( "ext4\nvfat\nntfs\next2\next3\nexfat\nbtrfs" ) ;

			utility::changePathOwner( f ) ;
			utility::changePathPermissions( f,0666 ) ;

			f.close() ;
		}
	}

	if( f.open( QIODevice::ReadOnly ) ){

		return utility::split( f.readAll() ) ;
	}

	return { "ext4","vfat","ntfs","ext2","ext3","exfat","btrfs" } ;
}

std::pair< bool,QByteArray > utility::getKeyFromNetwork( const QString& e )
{
	QFile f( utility::homePath() + "/.zuluCrypt/network" ) ;

	if( !f.open( QIODevice::ReadOnly ) ){

		return { false,"" } ;
	}

	QUrl url ;

	QByteArray data = "device=" + utility::split( e,' ' ).last().toLatin1() ;

	QByteArray host ;

	for( const auto& it : utility::split( f.readAll() ) ){

		if( it.startsWith( "url=" ) ){

			url.setUrl( it.toLatin1().constData() + 4 ) ;

		}else if( it.startsWith( "host=" ) ){

			host = it.toLatin1().constData() + 5 ;
		}else{
			data += "&" + it ;
		}
	}

	QNetworkRequest n( url ) ;

	n.setRawHeader( "Host",host ) ;
	n.setRawHeader( "Content-Type","application/x-www-form-urlencoded" ) ;

	NetworkAccessManager m ;

	auto s = m.post( n,data ) ;

	if( s->error() == QNetworkReply::NoError ){

		return { true,s->readAll() } ;
	}else{
		return { false,"" } ;
	}
}

void utility::setHDPI( const QString& e )
{
	Q_UNUSED( e ) ;

#if QT_VERSION >= 0x050600

	QApplication::setAttribute( Qt::AA_EnableHighDpiScaling ) ;

	QFile f( utility::homePath() + "/.zuluCrypt/" + e + ".scaleFactor" ) ;

	if( !f.exists() ){

		f.open( QIODevice::WriteOnly ) ;
		f.write( "1" ) ;
		f.close() ;
	}

	if( f.open( QIODevice::ReadOnly ) ){

		qputenv( "QT_SCALE_FACTOR",f.readAll().replace( "\n","" ) ) ;
	}
#endif
}

bool utility::platformIsLinux()
{
	return true ;
}

bool utility::platformIsOSX()
{
	return false ;
}

QStringList utility::executableSearchPaths()
{
	return { "/usr/local/bin/",
		"/usr/local/sbin/",
		"/usr/bin/",
		"/usr/sbin/",
		"/bin/",
		"/sbin/",
		"/opt/local/bin/",
		"/opt/local/sbin/",
		"/opt/bin/",
		"/opt/sbin/" } ;
}

QString utility::executableSearchPaths( const QString& e )
{
	if( e.isEmpty() ){

		return utility::executableSearchPaths().join( ":" ) ;
	}else{
		return e + ":" + utility::executableSearchPaths().join( ":" ) ;
	}
}

static inline bool _terminalEchoOff( struct termios * old,struct termios * current )
{
	if( tcgetattr( 1,old ) != 0 ){

		return false ;
	}

	*current = *old;
	current->c_lflag &= ~ECHO;

	if( tcsetattr( 1,TCSAFLUSH,current ) != 0 ){

		return false ;
	}else{
		return true ;
	}
}

QString utility::readPassword( bool addNewLine )
{
	std::cout << "Password: " << std::flush ;

	struct termios old ;
	struct termios current ;

	_terminalEchoOff( &old,&current ) ;

	QString s ;
	int e ;

	int m = 1024 ;

	for( int i = 0 ; i < m ; i++ ){

		e = std::getchar() ;

		if( e == '\n' || e == -1 ){

			break ;
		}else{
			s += static_cast< char >( e ) ;
		}
	}

	tcsetattr( 1,TCSAFLUSH,&old ) ;

	if( addNewLine ){

		std::cout << std::endl ;
	}

	return s ;
}

QString utility::fileManager()
{
	QFile f( utility::homePath() + "/.zuluCrypt/FileManager" ) ;

	if( !f.exists() ){

		f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;

		f.write( "xdg-open" ) ;

		f.close() ;
	}

	if( !f.open( QIODevice::ReadOnly ) ){

		return "xdg-open" ;
	}else{
		return utility::split( f.readAll() ).first() ;
	}
}

void utility::setFileManager( const QString& e )
{
	QFile f( utility::homePath() + "/.zuluCrypt/FileManager" ) ;

	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;

	if( e.isEmpty() ){

		f.write( "xdg-open" ) ;
	}else{
		f.write( e.toLatin1() ) ;
	}
}

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

#include "utility.h"
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

#include <memory>

#include <QTranslator>
#include <QEventLoop>
#include <QDebug>
#include <QCoreApplication>
#include <blkid/blkid.h>
#include <QByteArray>
#include <QProcess>
#include <QFile>
#include <QFile>
#include <QDir>
#include <QTableWidget>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QProcessEnvironment>
#include <unistd.h>
#include <pwd.h>

#include <QEvent>
#include <QKeyEvent>

#include "../zuluCrypt-cli/constants.h"
#include "../zuluCrypt-cli/bin/bash_special_chars.h"
#include "version.h"
#include "locale_path.h"
#include "mount_prefix_path.h"
#include "storage_manager.h"
#include "dialogmsg.h"
#include "support_whirlpool.h"
#include "readonlywarning.h"
#include "../plugins/plugins.h"
#include "plugin.h"
#include "install_prefix.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <gcrypt.h>

#include "lxqt_wallet/frontend/lxqt_wallet.h"

#include "../zuluCrypt-cli/pluginManager/libzuluCryptPluginManager.h"

#include "../zuluCrypt-cli/utility/process/process.h"

#include "plugin_path.h"

#include "../zuluCrypt-cli/utility/process/process.h"

#include "reuse_mount_point.h"

static int staticGlobalUserId = -1 ;

bool utility::reUseMountPointPath()
{
	return REUSE_MOUNT_POINT ;
}

void utility::setUID( int uid )
{
	if( utility::userIsRoot() ){

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

void utility::keySend( const QString& path,const QString& key )
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

			::zuluCryptPluginManagerSendKey( handle,key.toLatin1().constData(),size ) ;
			::zuluCryptPluginManagerCloseConnection( handle ) ;
		}
	} ) ;
}

void utility::createPlugInMenu( QMenu * menu,const QString& a,const QString& b,const QString& c,bool addPlugIns )
{
	QStringList l ;
	QStringList e ;

	l.append( a ) ;

	if( utility::notRunningInMixedMode() ){

		if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::secretServiceBackEnd ) ){

			l.append( b ) ;
		}
		if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::kwalletBackEnd ) ){

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

			const auto& s = r.output() ;

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

::Task::future<QString>& utility::getKeyFromWallet( LxQt::Wallet::Wallet * wallet,const QString& volumeID )
{
	return ::Task::run<QString>( [ wallet,volumeID ](){

		decltype( wallet->readValue( volumeID ) ) key ;

		if( volumeID.startsWith( "UUID=" ) ){

			key = wallet->readValue( volumeID ) ;
		}else{
			auto uuid = utility::getUUIDFromPath( volumeID ).get() ;

			if( uuid.isEmpty() ){

				key = wallet->readValue( utility::getVolumeID( volumeID ) ) ;
			}else{
				key = wallet->readValue( uuid ) ;

				if( key.isEmpty() ){

					key = wallet->readValue( volumeID ) ;
				}
			}
		}

		return key ;
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

			QString uuid = r.output() ;
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

utility::Array::Array( const QString& s,char splitter )
{
	if( !s.isEmpty() ){

		m_list = s.toLatin1().split( splitter ) ;
	}

	this->setUp() ;
}

utility::Array::Array( const QStringList& s )
{
	for( const auto& it : s ){

		m_list.append( it.toLatin1() ) ;
	}

	this->setUp() ;
}

size_t utility::Array::size()
{
	return m_list.size() ;
}

void utility::Array::setUp()
{
	auto p = m_list.size() ;

	m_vector.resize( p + 1 ) ;

	auto q = m_vector.data() ;

	for( decltype( p ) i = 0 ; i < p ; i++ ){

		*( q + i ) = m_list.at( i ).constData() ;
	}

	*( q + p ) = nullptr ;
}

char * const * utility::Array::value()
{
	return const_cast< char * const * >( m_vector.data() ) ;
}

static bool _execute_process( const QString& m,const QString& exe,const QString& env,int uid )
{
	if( exe.startsWith( "/" ) && utility::pathExists( exe ) ){

		auto e = m ;

		e.replace( "\"","\"\"\"" ) ;

		return utility::Task( exe + " \"" + m + "\"",-1,env.split( "\n" ),[ uid ](){

			if( uid != -1 ){

				Q_UNUSED( setgid( uid ) ) ;
				Q_UNUSED( setgroups( 1,reinterpret_cast< const gid_t * >( &uid ) ) ) ;
				Q_UNUSED( setegid( uid ) ) ;
				Q_UNUSED( setuid( uid ) ) ;
			}

		} ).success() ;
	}else{
		return false ;
	}
}

::Task::future<bool>& utility::openPath( const QString& path,const QString& opener,const QString& env )
{
	return ::Task::run<bool>( [ env,path,opener ](){

		return _execute_process( path,opener,env,utility::getUID() ) == false ;
	} ) ;
}

void utility::openPath( const QString& path,const QString& opener,const QString& env,QWidget * obj,const QString& title,const QString& msg )
{
	openPath( path,opener,env ).then( [ title,msg,obj ]( bool failed ){

		if( failed && obj ){

			DialogMsg m( obj ) ;
			m.ShowUIOK( title,msg ) ;
		}
	} ) ;
}

utility::wallet utility::getKeyFromWallet( LxQt::Wallet::walletBackEnd storage,const QString& keyID,const QString& pwd )
{
	utility::wallet w{ false,false,"","" } ;

	auto _getBackEnd = []( LxQt::Wallet::walletBackEnd e ){

		return LxQt::Wallet::getWalletBackend( e ) ;
	} ;

	using storage_t = std::unique_ptr< LxQt::Wallet::Wallet > ;

	if( storage == LxQt::Wallet::kwalletBackEnd ){

		storage_t e( _getBackEnd( storage ) ) ;

		w.opened = e->await_open( "default",utility::applicationName() ) ;

		if( w.opened ){

			w.key = utility::getKeyFromWallet( e.get(),keyID ).await() ;
		}

		return w ;

	}else if( storage == LxQt::Wallet::internalBackEnd ){

		auto walletName = utility::walletName() ;
		auto appName    = utility::applicationName() ;

		if( LxQt::Wallet::walletExists( LxQt::Wallet::internalBackEnd,walletName,appName ) ){

			storage_t e( _getBackEnd( storage ) ) ;

			e->setImage( ":/zuluCrypt.png" ) ;

			w.opened = e->await_open( walletName,appName,pwd ) ;

			if( w.opened ){

				w.key = utility::getKeyFromWallet( e.get(),keyID ).await() ;
				w.password = e->qObject()->objectName() ;
				w.notConfigured = false ;
			}

			return w ;
		}else{
			w.notConfigured = true ;
			return w ;
		}

	}else if( storage == LxQt::Wallet::secretServiceBackEnd ){

		storage_t e( _getBackEnd( storage ) ) ;

		w.opened = e->await_open( utility::walletName(),utility::applicationName() ) ;

		if( w.opened ){

			w.key = utility::getKeyFromWallet( e.get(),keyID ).await() ;
		}

		return w ;
	}else{
		/*
		 * shouldnt get here
		 */
		return w ;
	}
}

static quint64 _volumeSize( const QString& e )
{
	utility::fileHandle h ;

	if( h.open( e ) ){

		return h.size() ;
	}else{
		return 0 ;
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

::Task::future< int >& utility::clearVolume( const QString& volume,bool * exit,std::function< void( int ) > function )
{
	return ::Task::run<int>( [ volume,exit,function ](){

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

				quint64 size         = _volumeSize( volumeMapperPath ) ;
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
	return QObject::tr( "\
if the option is checked,a primary private mount point will be created in \"/run/media/private/$USER/\"\n\
and a secondary publicly accessible \"mirror\" mount point will be created in \"/run/media/public/\"" ) ;
}

QString utility::shareMountPointToolTip( const QString& path )
{
	auto s = QString( "/run/media/public/" ) + path.split( "/" ).last() ;

	if( QFile::exists( s ) ){

		return "public mount point: " + s ;
	}else{
		return QString() ;
	}
}

QString utility::sharedMountPointPath( const QString& path )
{
	if( path == "/" ){

		return QString() ;
	}else{
		auto s = "/run/media/public/" + path.split( "/" ).last() ;

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

		rpath.remove( QChar( '\"' ) ) ;

		rpath.replace( "UUID=","UUID-" ) ;

		path += QString( "-" ) + rpath + utility::hashPath( rpath.toLatin1() ) ;
	}else{
		if( component.isEmpty() ){

			path += "-NAAN-" + rpath.split( "/" ).last() + utility::hashPath( rpath.toLatin1() ) ;
		}else{
			path += component + rpath.split( "/" ).last() + utility::hashPath( rpath.toLatin1() ) ;
		}
	}

	QString z = BASH_SPECIAL_CHARS ;

	for( const auto& it : z ){

		path.replace( it,QChar( '_' ) ) ;
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

	return QString( "-" ) + QString::number( hash ) ;
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

static QString _absolute_exe_path( const QString& exe )
{
	QString e = "/usr/local/bin/" + exe ;

	if( utility::pathExists( e ) ){

		return e ;
	}

	e = "/usr/local/sbin/" + exe ;

	if( utility::pathExists( e ) ){

		return e ;
	}

	e = "/usr/bin/" + exe ;

	if( utility::pathExists( e ) ){

		return e ;
	}

	e = "/usr/sbin/" + exe ;

	if( utility::pathExists( e ) ){

		return e ;
	}

	e = "/bin/" + exe ;

	if( utility::pathExists( e ) ){

		return e ;
	}

	e = "/sbin/" + exe ;

	if( utility::pathExists( e ) ){

		return e ;
	}

	return exe ;
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

	if( defaulT == "xdg-open" ){

		return _absolute_exe_path( defaulT ) ;
	}else{
		return defaulT ;
	}
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

		utility::changeFilePermissions( f ) ;

		f.write( fav.toLatin1() ) ;
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

	utility::changeFilePermissions( f ) ;
}

void utility::readFavorites( QMenu * m,bool truncate )
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

	m->addSeparator() ;

	for( const auto& it : utility::readFavorites() ){

		m->addAction( _add_action( it ) ) ;
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
	m.ShowUIInfo( QObject::tr( "about zuluCrypt" ),license ) ;
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

			utility::changeFilePermissions( f ) ;

			f.write( defaults ) ;

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

		utility::changeFilePermissions( f ) ;

		for( const auto& it : e ){

			f.write( QString( QString::number( it ) + " " ).toLatin1() ) ;
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

		utility::changeFileOwner( f ) ;
		utility::changeFilePermissions( f ) ;

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
	}
}

bool utility::runningInMixedMode()
{
	return utility::userIsRoot() && utility::getUID() != -1 ;
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

int utility::pluginKey( QDialog * w,QString * key,const QString& p )
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

	plugin::instance( w,pluginType,[ & ]( const QString& e ){

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

	QString home = utility::homePath() + "/.zuluCrypt/" ;
	QDir d( home ) ;

	if( !d.exists() ){

		d.mkdir( home ) ;
	}

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
}

static void _set_checked( QMenu * m,const QString& e )
{
	for( auto& it : m->actions() ){

		QString p = it->text() ;

		p.remove( "&" ) ;

		it->setChecked( e == p ) ;
	}
}

void utility::setLocalizationLanguage( bool translate,QWidget * w,QAction * ac,const char * app )
{
	auto r = utility::localizationLanguage( app ).toLatin1() ;

	auto e = utility::localizationLanguagePath( app ) ;

	if( translate ){

		auto translator = new QTranslator( w ) ;

		if( r == "en_US" ){
			/*
			 * english_US language,its the default and hence dont load anything
			 */
		}else{
			translator->load( r.constData(),e ) ;
			QCoreApplication::installTranslator( translator ) ;
		}

	}else{
		auto m = new QMenu( w ) ;

		m->setFont( w->font() ) ;

		w->connect( m,SIGNAL( triggered( QAction * ) ),w,SLOT( languageMenu( QAction * ) ) ) ;

		QDir d( e ) ;

		m->addAction( "en_US" )->setCheckable( true ) ;

		auto t = d.entryList() ;

		if( !t.isEmpty() ){

			t.removeOne( "." ) ;
			t.removeOne( ".." ) ;

			for( auto& it : t ){

				m->addAction( it.remove( ".qm" ) )->setCheckable( true ) ;
			}
		}

		if( ac ){

			ac->setMenu( m ) ;
		}

		_set_checked( m,r ) ;
	}
}

void utility::languageMenu( QWidget * w,QMenu * m,QAction * ac,const char * app )
{
	Q_UNUSED( m ) ;

	auto e = ac->text() ;

	e.remove( "&" ) ;

	utility::setLocalizationLanguage( app,e ) ;

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

QIcon utility::getIcon( const QString& application )
{
	QIcon icon( INSTALL_PREFIX "/share/icons/hicolor/48x48/apps/" + application + ".png" ) ;

	return QIcon::fromTheme( application,icon ) ;
}

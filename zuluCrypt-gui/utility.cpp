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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <blkid/blkid.h>

#include <gcrypt.h>

#include "lxqt_wallet/frontend/lxqt_wallet.h"

#include "../zuluCrypt-cli/pluginManager/libzuluCryptPluginManager.h"

#include "../zuluCrypt-cli/utility/process/process.h"

#include "plugin_path.h"

#include "../zuluCrypt-cli/utility/process/process.h"

static int staticGlobalUserId = -1 ;

void utility::setUID( int uid )
{
	staticGlobalUserId = uid ;
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

	if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::secretServiceBackEnd ) ){
		l.append( b ) ;
	}
	if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::kwalletBackEnd ) ){
		l.append( c ) ;
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

		QString e = utility::appendUserUID( "%1 -b -d \"%2\"" ) ;

		auto r = utility::Task( e.arg( ZULUCRYPTzuluCrypt,volumePath ) ) ;

		QStringList l ;

		if( r.success() ){

			const QByteArray& s = r.output() ;
			int i = 0 ;
			for( const auto& it : s ){
				if( it == '1' || it == '3' ){
					i++ ;
				}
			}
			l.append( QString::number( i ) ) ;
			l.append( QString::number( s.size() - 1 ) ) ;
		}

		return l ;
	} ) ;
}

::Task::future<QString>& utility::getKeyFromWallet( LxQt::Wallet::Wallet * wallet,const QString& volumeID )
{
	return ::Task::run<QString>( [ wallet,volumeID ](){

		QByteArray key ;

		if( volumeID.startsWith( "UUID=" ) ){
			key = wallet->readValue( volumeID ) ;
		}else{
			QString uuid = utility::getUUIDFromPath( volumeID ).get() ;
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

		QString device = dev ;
		device = device.replace( "\"", "\"\"\"" ) ;
		QString exe = utility::appendUserUID( "%1 -U -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt,device ) ;

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

bool utility::ProcessExecute( const QString& m,const QString& e,const QString& env,int uid )
{
	QByteArray exe     = e.toLatin1() ;
	QByteArray m_point = m.toLatin1() ;

	if( !exe.startsWith( "/" ) ){

		auto e = utility::Task( "which " + exe ) ;

		if( e.failed() ){

			return false ;
		}

		exe = e.splitOutput( '\n' ).first().toLatin1() ;
	}

	process_t p = Process( exe.constData(),m_point.constData(),nullptr ) ;

	if( uid != -1 && !env.isEmpty() ){

		QStringList l = utility::split( env ) ;

		QList< QByteArray > r ;

		for( const auto& it : l ){

			r.append( it.toLatin1() ) ;
		}

		QVector< const char * > e( r.size() + 1 ) ;

		const char ** z = e.data() ;

		for( int i = 0 ; i < r.size() ; i++ ){

			*( z + i ) = r.at( i ).constData() ;
		}

		*( z + r.size() ) = nullptr ;

		ProcessSetOptionUser( p,uid ) ;

		ProcessSetEnvironmentalVariable( p,( char * const * ) z ) ;

		ProcessStart( p ) ;
	}else{
		ProcessStart( p ) ;
	}

	return ProcessWaitUntilFinished( &p ) != 0 ;
}

::Task::future<bool>& utility::openMountPoint( const QString& path,const QString& opener,const QString& env )
{
	return ::Task::run<bool>( [ env,path,opener ](){

		return utility::ProcessExecute( path,opener,env,utility::getUID() ) ;
	} ) ;
}

utility::wallet utility::getKeyFromWallet( LxQt::Wallet::walletBackEnd storage,const QString& keyID,const QString& pwd )
{
	utility::wallet w{ false,false,"","" } ;

	if( storage == LxQt::Wallet::kwalletBackEnd ){

		auto e = LxQt::Wallet::getWalletBackend( LxQt::Wallet::kwalletBackEnd ) ;

		w.opened = e->await_open( "default",utility::applicationName() ) ;

		if( w.opened ){
			w.key = utility::getKeyFromWallet( e,keyID ).await() ;
		}

		e->deleteLater() ;

		return w ;

	}else if( storage == LxQt::Wallet::internalBackEnd ){

		QString walletName = utility::walletName() ;
		QString appName    = utility::applicationName() ;

		if( LxQt::Wallet::walletExists( LxQt::Wallet::internalBackEnd,walletName,appName ) ){

			auto e = LxQt::Wallet::getWalletBackend( LxQt::Wallet::internalBackEnd ) ;

			e->setImage( ":/zuluCrypt.png" ) ;

			w.opened = e->await_open( walletName,appName,pwd ) ;

			if( w.opened ){
				w.key = utility::getKeyFromWallet( e,keyID ).await() ;
				w.password = e->qObject()->objectName() ;
				w.notConfigured = false ;
			}

			e->deleteLater() ;

			return w ;
		}else{
			w.notConfigured = true ;
			return w ;
		}

	}else if( storage == LxQt::Wallet::secretServiceBackEnd ){

		auto e = LxQt::Wallet::getWalletBackend( LxQt::Wallet::secretServiceBackEnd ) ;

		w.opened = e->await_open( utility::walletName(),utility::applicationName() ) ;

		if( w.opened ){
			w.key = utility::getKeyFromWallet( e,keyID ).await() ;
		}

		e->deleteLater() ;

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
	quint64 r = 0 ;

	int f = open( e.toLatin1().constData(),O_RDONLY ) ;

	if( f != -1 ){
		r = quint64( blkid_get_dev_size( f ) ) ;
		close( f ) ;
	}

	return r ;
}

static int _openVolume( const QString& e )
{
	return open( e.toLatin1().constData(),O_RDWR ) ;
}

static std::function< void( int ) > _closeFunction( const QString& e )
{
	return [ e ]( int fd ){

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

		QString volumePath = volume ;

		volumePath.replace( "\"","\"\"\"" ) ;

		int r = utility::Task( utility::appendUserUID( "%1 -k -J -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt,volumePath ) ).exitCode() ;

		if( r != 0 ){
			return r ;
		}else{
			QString volumeMapperPath = utility::mapperPath( volume ) ;

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
	QString s = QObject::tr( "\
if the option is checked,a primary private mount point will be created in \"/run/media/private/$USER/\"\n\
and a secondary publicly accessible \"mirror\" mount point will be created in \"/run/media/public/\"" ) ;
	return s ;
}

QString utility::shareMountPointToolTip( const QString& path )
{
	struct stat st ;
	QString s = QString( "/run/media/public/" ) + path.split( "/" ).last() ;
	QByteArray x = s.toLatin1() ;
	const char * y = x.constData() ;
	if( stat( y,&st ) == 0 ){
		return "public mount point: " + s ;
	}else{
		//return QString( "no public mount point" ) ;
		return QString() ;
	}
}

QString utility::sharedMountPointPath( const QString& path )
{
	if( path == "/" ){
		return QString() ;
	}else{
		struct stat st ;
		QString s = "/run/media/public/" + path.split( "/" ).last() ;
		QByteArray x = s.toLatin1() ;
		const char * y = x.constData() ;
		if( stat( y,&st ) == 0 ){
			return s ;
		}else{
			return QString() ;
		}
	}
}

bool utility::pathPointsToAFile( const QString& path )
{
	struct stat st ;
	QByteArray b = path.toLatin1() ;
	if( stat( b.constData(),&st ) == 0 ){
		return S_ISREG( st.st_mode ) != 0 ;
	}else{
		return false ;
	}
}

bool utility::pathPointsToAFolder( const QString& path )
{
	struct stat st ;
	QByteArray b = path.toLatin1() ;
	if( stat( b.constData(),&st ) == 0 ){
		return S_ISDIR( st.st_mode ) != 0 ;
	}else{
		return false ;
	}
}

QString utility::localizationLanguage( const QString& program )
{
	QString langPath = utility::localizationLanguagePath( program ) ;
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment() ;
	QString value = env.value( "LANG" ) ;

	QString ext( ".qm" ) ;
	QString s ;
	int index ;
	if( !value.isEmpty() ){
		QStringList values = value.split( ":" ) ;

		QDir dir( langPath ) ;

		QStringList dirList = dir.entryList() ;
		dirList.removeOne( "." ) ;
		dirList.removeOne( ".." ) ;

		for( const auto& it : values ){
			s = it ;
			index = s.indexOf( "." ) ;
			if( index != -1 ){
				s.truncate( index ) ;
			}

			if( dirList.contains( s + ext ) ){
				return s ;
			}
		}
	}

	value = env.value( "LANGUAGE" ) ;

	if( !value.isEmpty() ){
		QStringList values = value.split( ":" ) ;

		QDir dir( langPath ) ;

		QStringList dirList = dir.entryList() ;
		dirList.removeOne( "." ) ;
		dirList.removeOne( ".." ) ;

		for( const auto& it : values){
			s = it ;
			index = s.indexOf( "." ) ;
			if( index != -1 ){
				s.truncate( index ) ;
			}
			if( dirList.contains( s + ext ) ){
				return s ;
			}
		}
	}

	return QString( "en_US" ) ;
}

QString utility::localizationLanguagePath( const QString& program )
{
	return QString( TRANSLATION_PATH ) + program ;
}

void utility::setLocalizationLanguage( const QString& program,const QString& language )
{
	Q_UNUSED( program ) ;
	Q_UNUSED( language ) ;
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
	int fd = open( path.toLatin1().constData(),O_RDONLY ) ;
	if( fd != -1 ){
		close( fd ) ;
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

bool utility::eventFilter( QObject * gui,QObject * watched,QEvent * event )
{
	if( watched == gui ){

		if( event->type() == QEvent::KeyPress ){

			QKeyEvent * keyEvent = static_cast< QKeyEvent* >( event ) ;

			if( keyEvent->key() == Qt::Key_Escape ){

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
	QString s = str ;
	return s.split( token,QString::SkipEmptyParts ) ;
}

bool utility::mapperPathExists( const QString& path )
{
	if( utility::pathExists( utility::mapperPath( path ) ) ){
		return true ;
	}else{
		return utility::pathExists( utility::mapperPath( path,"-VERA-" ) ) ;
	}
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
		auto _path_not_found = []( const QString& e ){ return !utility::pathExists( e ) ; } ;

		QString e = utility::mountPath( path ) ;

		if( _path_not_found( e ) ){

			return path ;
		}else{
			QString z ;

			for( int i = 1 ; i < 1000 ; i++ ){

				z = QString::number( i ) ;

				if( _path_not_found( QString( "%1_%2" ).arg( e,z ) ) ){

					return QString( "%1_%2" ).arg( path,z ) ;
				}
			}

			return path ;
		}
	}
}

QString utility::mapperPath( const QString& r,const QString& component )
{
	QString rpath = r ;

	QString path = utility::cryptMapperPath() + "zuluCrypt-" + utility::getStringUserID() ;

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

	QString z = QString( BASH_SPECIAL_CHARS ) ;

	for( const auto& it : z ){

		path.replace( it,QChar( '_' ) ) ;
	}
	return path ;
}

QString utility::hashPath( const QByteArray& p )
{
	size_t l = p.size() ;
	uint32_t hash ;
	uint32_t i ;
	const char * key = p.constData() ;
	i = hash = 0 ;
	for( ; i < l ; i++ ){
		hash += key[ i ] ;
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
	struct stat st ;
	return stat( path.toLatin1().data(),&st ) == 0 ;
}

bool utility::canCreateFile( const QString& path )
{
	QByteArray q = path.toLatin1() ;

	int i = open( q.constData(),O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) ;

	if( i == -1 ){
		return false ;
	}else{
		close( i ) ;
		remove( q ) ;
		return true ;
	}
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
		QString rp = r.canonicalPath() ;
		if( rp.isEmpty() ) {
			return path ;
		}else{
			return rp ;
		}
	}
}

QString utility::cmdArgumentValue( const QStringList& l,const QString& arg,const QString& defaulT )
{
	int j = l.size() ;
	for( int i = 0 ; i < j ; i++ ){
		if( l.at( i ) == arg ){
			if( i + 1 < j ){
				return l.at( i + 1 ) ;
			}else{
				return defaulT ;
			}
		}
	}

	return defaulT ;
}

void utility::addToFavorite( const QString& dev,const QString& m_point )
{
	QString fav = QString( "%1\t%2\n" ).arg( dev,m_point ) ;
	QFile f( utility::homePath() + "/.zuluCrypt/favorites" ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Append ) ;
	f.write( fav.toLatin1() ) ;
	f.close() ;
}

QStringList utility::readFavorites()
{
	QFile f( utility::homePath() + "/.zuluCrypt/favorites" ) ;
	QStringList list ;
	if( f.open( QIODevice::ReadOnly ) ){
		QString data( f.readAll() ) ;
		f.close() ;
		if( !data.isEmpty() ){
			list = data.split( "\n" ) ;
		}
	}
	return list ;
}

void utility::removeFavoriteEntry( const QString& entry )
{
	QFile f( utility::homePath() + "/.zuluCrypt/favorites" ) ;
	f.open( QIODevice::ReadOnly ) ;
	QByteArray b = f.readAll() ;
	f.close() ;
	QByteArray c = b.remove( b.indexOf( entry ),entry.length() ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( c ) ;
	f.close() ;
}

QString utility::getVolumeID( const QString& id )
{
	if( id.startsWith( "/dev/" ) ){
		QDir d( "/dev/disk/by-id" ) ;
		QStringList l = d.entryList() ;
		l.removeOne( "." ) ;
		l.removeOne( ".." ) ;
		QDir r ;
		for( const auto& it : l ){
			const QString& e = it ;
			if( !e.startsWith( "dm" ) ){
				r.setPath( QString( "/dev/disk/by-id/%1" ).arg( e ) ) ;
				if( r.canonicalPath() == id ){
					return e ;
				}
			}
		}
		return id ;
	}else{
		return id ;
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

static QVector<int> _default_dimensions( const char * defaults )
{
	QStringList l = QString( defaults ).split( ' ' ) ;

	QVector<int> e ;

	for( const auto& it : l ){

		e.append( it.toInt() ) ;
	}

	return e ;
}

static QVector<int> _dimensions( const QString& path,const char * defaults,int size )
{
	QFile f( path ) ;

	if( !f.exists() ){

		if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

			f.write( defaults ) ;

			f.close() ;
		}else{
			qDebug() << "failed to open config file" ;
			return _default_dimensions( defaults ) ;
		}
	}

	if( f.open( QIODevice::ReadOnly ) ){

		QStringList l = QString( f.readAll() ).split( ' ',QString::SkipEmptyParts ) ;

		if( l.size() != size ){

			qDebug() << "failed to parse config file" ;
			return _default_dimensions( defaults ) ;
		}

		QVector<int> customDimensions ;

		for( const auto& it : l ){

			bool ok ;

			int e = it.toInt( &ok ) ;

			if( ok ){

				customDimensions.append( e ) ;
			}else{
				qDebug() << "failed to parse config file option" ;
				return _default_dimensions( defaults ) ;
			}
		}

		return customDimensions ;
	}else{
		qDebug() << "failed to open config file" ;
		return _default_dimensions( defaults ) ;
	}
}

QVector<int> utility::getWindowDimensions( const QString& application )
{
	QString path = QDir::homePath() + "/.zuluCrypt/" + application + "-gui-ui-options" ;

	if( application == "zuluCrypt" ){

		return _dimensions( path,"0 0 782 419 298 336 100",7 ) ;
	}else{
		return _dimensions( path,"0 0 910 477 220 320 145 87 87",9 ) ;
	}
}

void utility::setWindowDimensions( const QVector<int>& e,const QString& application )
{
	QString path = QDir::homePath() + "/.zuluCrypt/" + application + "-gui-ui-options" ;

	QFile f( path ) ;

	if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

		for( const auto& it : e ){

			f.write( QString( QString::number( it ) + " " ).toLatin1() ) ;
		}
	}
}

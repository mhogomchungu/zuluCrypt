/*
 *
 *  Copyright ( c ) 2017
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


#include "zulupolkit.h"
#include "task.hpp"
#include "bin_path.h"
#include "../zuluCrypt-gui/executablesearchpaths.h"
#include "json.h"

#include <termios.h>
#include <memory>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <iostream>

#include <QTimer>
#include <QDir>
#include <QProcess>
#include <QCoreApplication>
#include <QFile>

namespace utility
{
	QString executableFullPath( const QString& e )
	{
		QString exe ;

		for( const auto& it : executableSearchPaths::values() ){

			exe = it + e ;

			if( QFile::exists( exe ) ){

				return exe ;
			}
		}

		return QString() ;
	}
}

static bool _terminalEchoOff( struct termios * old,struct termios * current )
{
	if( tcgetattr( 1,old ) != 0 ){

		return false ;
	}

	*current = *old ;
	current->c_lflag &= ~ECHO ;

	if( tcsetattr( 1,TCSAFLUSH,current ) != 0 ){

		return false ;
	}else{
		return true ;
	}
}

#if QT_VERSION > QT_VERSION_CHECK( 5,0,0 )
	#include <QFileDevice>

	#define zuluPermission QFileDevice
#else
	#define zuluPermission QFile
#endif

static QByteArray _zulupolkit( const QString& cmd,const QString& path,const QString& data )
{
	QFile e( path ) ;

	QDir().mkpath( "/etc/zuluCrypt" ) ;

	auto q = zuluPermission::ReadOwner | zuluPermission::WriteOwner ;
	QFile().setPermissions( "/etc/zuluCrypt",q | zuluPermission::ExeOwner ) ;

	QByteArray s ;

	if( cmd == "Read" ){

		if( e.open( QIODevice::ReadOnly ) ){

			s = e.readAll() ;
		}

	}else if( cmd == "Write" ){

		if( e.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

			e.write( data.toLatin1() ) ;
		}
	}

	e.setPermissions( q ) ;

	return s ;
}

zuluPolkit::zuluPolkit( const QStringList& s ) : m_arguments( s )
{
	connect( &m_server,SIGNAL( newConnection() ),this,SLOT( gotConnection() ) ) ;
}

zuluPolkit::~zuluPolkit()
{
	QDir().remove( m_socketPath ) ;
}

static void _set_path_writable_by_others( const QString& e )
{
	QFile f( e ) ;

	for( decltype( umask( 0 ) ) i = 0 ; i < 1000 ; i++ ){

		umask( i ) ;

		f.open( QIODevice::WriteOnly ) ;

		auto s = f.permissions() ;

		f.close() ;
		f.remove() ;

		if( s & zuluPermission::WriteOther ){

			break ;
		}
	}
}

void zuluPolkit::start()
{
	if( m_arguments.size() > 1 ){

		m_cookie = this->readStdin() ;

		m_socketPath = m_arguments.at( 1 ) ;

		QFile::remove( m_socketPath ) ;

		auto s = umask( 0 ) ;

		_set_path_writable_by_others( m_socketPath ) ;

		m_server.listen( m_socketPath ) ;

		umask( s ) ;
	}
}

static void _respond( QLocalSocket& s,const QByteArray& e )
{
	nlohmann::json json ;

	json[ "stdOut" ]     = e.constData() ;
	json[ "stdError" ]   = e.constData() ;
	json[ "exitCode" ]   = 0 ;
	json[ "exitStatus" ] = 0 ;
	json[ "finished" ]   = true ;

	s.write( json.dump().c_str() ) ;

	s.waitForBytesWritten() ;
}

static void _respond( QLocalSocket& s,const Task::process::result& e = Task::process::result() )
{
	nlohmann::json json ;

	json[ "stdOut" ]     = e.std_out().constData() ;
	json[ "stdError" ]   = e.std_error().constData() ;
	json[ "exitCode" ]   = e.exit_code() ;
	json[ "exitStatus" ] = e.exit_status() ;
	json[ "finished" ]   = e.finished() ;

	s.write( json.dump().c_str() ) ;

	s.waitForBytesWritten() ;
}

static bool _correct_cmd( const QString& cmd )
{
	auto a = ZULUCRYPTzuluCrypt" " ;
	auto b = zuluMountPath" " ;
	auto su = utility::executableFullPath( "su" ) ;
	auto e = su + " - -c \"" + utility::executableFullPath( "ecryptfs-simple" ) ;
	auto f = su + " - -c \"'" + utility::executableFullPath( "ecryptfs-simple" ) ;
	return cmd.startsWith( a ) || cmd.startsWith( b ) || cmd.startsWith( e ) || cmd.startsWith( f ) ;
}

void zuluPolkit::gotConnection()
{
	std::unique_ptr< QLocalSocket > m( m_server.nextPendingConnection() ) ;

	auto& s = *m ;

	try{
		s.waitForReadyRead() ;

		auto json = nlohmann::json::parse( s.readAll().constData() ) ;

		auto path     = QString::fromStdString( json[ "path" ].get< std::string >() ) ;
		auto data     = QString::fromStdString( json[ "data" ].get< std::string >() ) ;
		auto password = QString::fromStdString( json[ "password" ].get< std::string >() ) ;
		auto cookie   = QString::fromStdString( json[ "cookie" ].get< std::string >() ) ;
		auto command  = QString::fromStdString( json[ "command" ].get< std::string >() ) ;

		if( cookie == m_cookie ){

			if( command == "exit" ){

				return QCoreApplication::quit() ;

			}else if( command == "Read" || command == "Write" ){

				return _respond( s,_zulupolkit( command,path,data ) ) ;

			}else if( _correct_cmd( command ) ){

				return _respond( s,Task::process::run( command,password.toLatin1() ).get() ) ;
			}
		}

	}catch( ... ){}

	_respond( s,"zuluPolkit: Booooooooo!!!!" ) ;
}

QString zuluPolkit::readStdin()
{
	std::cout << "Token: " << std::flush ;

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

	std::cout << std::endl ;

	return s ;
}

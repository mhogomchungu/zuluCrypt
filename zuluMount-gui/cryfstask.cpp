/*
 *
 *  Copyright (c) 2014-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cryfstask.h"
#include "bin_path.h"

#include <QDir>
#include <QString>
#include <QDebug>
#include <QFile>

#include <iostream>

using cs = cryfsTask::status ;

static QString _makePath( const QString& e )
{
	return utility::Task::makePath( e ) ;
}

static bool _m( const QString& e )
{
	return utility::Task( utility::appendUserUID( e ) ).success() ;
}

static bool _delete_folder( const QString& m )
{	
	return _m( QString( "%1 -b %2" ).arg( zuluMountPath,_makePath( m ) ) ) ;
}

static bool _create_folder( const QString& m )
{
	return _m( QString( "%1 -B %2" ).arg( zuluMountPath,_makePath( m ) ) ) ;

}

static QString _configFilePath( const cryfsTask::options& opt )
{
	if( opt.configFilePath.startsWith( "/" ) || opt.configFilePath.isEmpty() ){

		return opt.configFilePath ;
	}else{
		return utility::homePath() + "/" + opt.configFilePath ;
	}
}

bool cryfsTask::deleteMountFolder( const QString& m )
{
	return _delete_folder( m ) ;
}

Task::future< bool >& cryfsTask::encryptedFolderUnMount( const QString& m )
{
	return Task::run< bool >( [ m ](){

		auto cmd = "fusermount -u " + _makePath( m ) ;

		auto _umount = [ & ](){

			if( utility::Task( cmd,10000 ).success() ){

				return _delete_folder( m ) ;
			}else{
				return false ;
			}
		} ;

		utility::Task::waitForOneSecond() ;

		for( int i = 0 ; i < 5 ; i++ ){

			if( _umount() ){

				return true ;
			}else{
				utility::Task::waitForOneSecond() ;
			}
		}

		return false ;
	} ) ;
}

static QString _args( const QString& exe,const cryfsTask::options& opt,
		      const QString& configFilePath,bool create )
{
	auto cipherFolder = _makePath( opt.cipherFolder ) ;

	auto mountPoint   = _makePath( opt.plainFolder ) ;

	const auto& type = opt.type ;

	auto mountOptions = [ & ](){

		if( !opt.mOpt.isEmpty() ){

			if( type == "cryfs" ){

				return QString( "--unmount-idle %1" ).arg( opt.mOpt ) ;

			}else if( type == "encfs" ){

				return QString( "--idle=%1" ).arg( opt.mOpt ) ;
			}
		}

		return QString() ;
	}() ;

	auto separator = [ & ](){

		if( type == "cryfs" ){

			return "--" ;

		}else if( type == "encfs" ){

			return "-S" ;
		}else{
			return "" ;
		}
	}() ;

	auto configPath = [ & ](){

		if( type.isOneOf( "cryfs","gocryptfs","securefs" ) ){

			if( !configFilePath.isEmpty() ){

				return "--config " + _makePath( configFilePath ) ;
			}
		}

		return QString() ;
	}() ;

	if( type.isOneOf( "gocryptfs","securefs" ) ){

		auto mode = [ & ](){

			if( opt.ro ){

				return "-o ro" ;
			}else{
				return "-o rw" ;
			}
		}() ;

		if( type == "gocryptfs" ){

			if( create ){

				auto e = QString( "%1 --init %2 %3" ) ;
				return e.arg( exe,configPath,cipherFolder ) ;
			}else{
				auto e = QString( "%1 %2 %3 %4 %5" ) ;

				return e.arg( exe,mode,configPath,cipherFolder,mountPoint ) ;
			}
		}else{
			if( create ){

				auto e = QString( "%1 create %2 %3" ) ;
				return e.arg( exe,configPath,cipherFolder ) ;
			}else{
				auto e = QString( "%1 mount -b %2 %3 -o fsname=securefs@%4 -o subtype=securefs %5 %6" ) ;

				return e.arg( exe,configPath,mode,cipherFolder,cipherFolder,mountPoint ) ;
			}
		}
	}else{
		auto e = QString( "%1 %2 %3 %4 %5 %6 -o fsname=%7@%8 -o subtype=%9" ) ;

		auto opts = e.arg( exe,cipherFolder,mountPoint,mountOptions,configPath,
				   separator,type.name(),cipherFolder,type.name() ) ;

		if( opt.ro ){

			return opts + " -o ro" ;
		}else{
			return opts + " -o rw" ;
		}
	}
}

static cs _cmd( bool create,const cryfsTask::options& opt,
		const QString& password,const QString& configFilePath )
{
	const auto& app = opt.type ;

	auto exe = app.executableFullPath() ;

	if( exe.isEmpty() ){

		if( app == "cryfs" ){

			return cs::cryfsNotFound ;

		}else if( app == "encfs" ){

			return cs::encfsNotFound ;

		}else if( app == "securefs" ){

			return cs::securefsNotFound ;
		}else{
			return cs::gocryptfsNotFound ;
		}
	}else{
		auto e = utility::Task( _args( exe,opt,configFilePath,create ),20000,[](){

			QProcessEnvironment env ;

			env.insert( "CRYFS_NO_UPDATE_CHECK","TRUE" ) ;
			env.insert( "CRYFS_FRONTEND","noninteractive" ) ;

			return env ;

		}(),password.toLatin1() ) ;

		auto _taskOutput = [ & ](){

			return e.output() + "\n-----\n" + e.stdError() ;
		} ;

		if( e.finished() ){

			if( e.success() ){

				return cs::success ;
			}else{
				auto a = "Did you enter the correct password?" ;
				auto b = "password incorrect" ;
				auto c = "Password incorrect" ;
				auto d = "Invalid password" ;

				if( utility::containsAtleastOne( _taskOutput(),a,b,c,d ) ){

					std::cout << _taskOutput().constData() << std::endl ;

					if( app == "cryfs" ){

						return cs::cryfs ;

					}else if( app == "encfs" ){

						return cs::encfs ;

					}else if( app == "securefs" ){

						return cs::securefs ;
					}else{
						return cs::gocryptfs ;
					}
				}
			}
		}

		std::cout << _taskOutput().constData() << std::endl ;

		return cs::backendFail ;
	}
}

Task::future< cs >& cryfsTask::encryptedFolderMount( const options& opt,bool reUseMountPoint )
{
	return Task::run< cs >( [ opt,reUseMountPoint ](){

		auto _mount = [ reUseMountPoint ]( const QString& app,const options& copt,
				const QString& configFilePath ){

			auto opt = copt ;

			opt.type = app ;

			if( _create_folder( opt.plainFolder ) || reUseMountPoint ){

				auto e = _cmd( false,opt,opt.key,configFilePath ) ;

				if( e == cs::success ){

					opt.openFolder( opt.plainFolder ) ;
				}else{
					cryfsTask::deleteMountFolder( opt.plainFolder ) ;
				}

				return e ;
			}else{
				return cs::failedToCreateMountPoint ;
			}
		} ;

		if( opt.configFilePath.isEmpty() ){

			if( utility::pathExists( opt.cipherFolder + "/cryfs.config" ) ){

				return _mount( "cryfs",opt,QString() ) ;

			}else if( utility::pathExists( opt.cipherFolder + "/gocryptfs.conf" ) ){

				return _mount( "gocryptfs",opt,QString() ) ;

			}else if( utility::pathExists( opt.cipherFolder + "/.securefs.json" ) ){

				return _mount( "securefs",opt,QString() ) ;
			}else{
				auto encfs6 = opt.cipherFolder + "/.encfs6.xml" ;
				auto encfs5 = opt.cipherFolder + "/.encfs5" ;
				auto encfs4 = opt.cipherFolder + "/.encfs4" ;

				if( utility::atLeastOnePathExists( encfs6,encfs5,encfs4 ) ){

					return _mount( "encfs",opt,QString() ) ;
				}else{
					return cs::unknown ;
				}
			}
		}else{
			auto e = _configFilePath( opt ) ;

			if( utility::pathExists( e ) ){

				if( e.endsWith( "gocryptfs.conf" ) ){

					return _mount( "gocryptfs",opt,e ) ;

				}else if( e.endsWith( "securefs.json" ) ){

					return _mount( "securefs",opt,e ) ;
				}else{
					return _mount( "cryfs",opt,e ) ;
				}
			}else{
				return cs::unknown ;
			}
		}
	} ) ;
}

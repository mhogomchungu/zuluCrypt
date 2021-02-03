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

#include "favorites.h"

#include "utility.h"

#include <QDir>
#include <QFile>
#include <QCryptographicHash>
#include <QStandardPaths>

static QByteArray _sha256( const QString& e )
{
	return QCryptographicHash::hash( e.toUtf8(),QCryptographicHash::Sha256 ).toHex() ;
}

static utility2::result< QString > _config_path()
{
	auto m = [](){

		auto& settings = utility::settingsObject() ;

		if( !settings.contains( "ConfigLocation" ) ){

			auto m = QStandardPaths::standardLocations( QStandardPaths::ConfigLocation ) ;

			if( !m.isEmpty() ){

				settings.setValue( "ConfigLocation",m.first() + "/zuluCrypt/" ) ;
			}else{
				//TODO: what to do here???
			}
		}

		return settings.value( "ConfigLocation" ).toString() ;
	}() ;



	m += "/favorites/" ;

	if( utility::pathExists( m ) ){

		return m ;
	}else{
		if( QDir().mkpath( m ) ){

			return m ;
		}else{
			return {} ;
		}
	}
}

static QString _create_path( const QString& m,const favorites::entry& e )
{
	auto a = utility::split( e.volumePath,'@' ).last() ;

	a = utility::split( a,'/' ).last() ;

	a.replace( ":","" ) ;

	auto b = a + e.mountPointPath ;

	return m + a + "-" + _sha256( b ) + ".json" ;
}

static QString _create_entry_path( const favorites::entry& e )
{
	auto s = _config_path() ;

	if( s.has_value() ){

		return _create_path( s.value(),e ) ;
	}else{
		return {} ;
	}
}

static utility2::result< favorites::entry > _favorites( const QString& path )
{
	SirikaliJson json( QFile( path ),[]( const QString& e ){

		Q_UNUSED( e )
	} ) ;

	favorites::entry m ;

	if( json.passed() ){

		m.internalConfigPath   = QDir( path ).absolutePath() ;		
		m.mountPointPath       = json.getString( "mountPointPath" ) ;		
		m.preMountCommand      = json.getString( "preMountCommand" ) ;
		m.postMountCommand     = json.getString( "postMountCommand" ) ;
		m.preUnmountCommand    = json.getString( "preUnmountCommand" ) ;
		m.postUnmountCommand   = json.getString( "postUnmountCommand" ) ;
		m.mountOptions         = json.getString( "mountOptions" ) ;

		m.password             = json.getByteArray( "password" ) ;

		m.volumePath           = json.getString( "volumePath" ) ;

		favorites::triState::readTriState( json,m.readOnlyMode,"mountReadOnly" ) ;
		favorites::triState::readTriState( json,m.autoMount,"autoMountVolume" ) ;

		return m ;
	}else{
		return {} ;
	}
}

static void _update_favorites( favorites * f )
{
	auto& settings = utility::settingsObject() ;

	if( settings.contains( "Favotites" ) ){

		 QStringList l ;

		 for( const auto& it : settings.value( "Favotites" ).toStringList() ){

			 favorites::entry e ;

			 auto s = [ & ](){

				 if( it.startsWith( "/dev/disk/by-id" ) ){

					auto a = utility::deviceIDToPartitionID( it ) ;
					return utility::split( a,'\t' ) ;
				}else{
					return utility::split( it,'\t' ) ;
				}
			 }() ;

			 e.volumePath = s.at( 0 ) ;
			 e.mountPointPath = s.at( 1 ) ;

			f->add( e ) ;
		 }

		 settings.remove( "Favotites" ) ;
	}
}

favorites::favorites()
{
	_update_favorites( this ) ;

	this->reload() ;
}

void favorites::reload()
{
	m_favorites.clear() ;

	const auto m = _config_path() ;

	if( m.has_value() ){

		const auto& a = m.value() ;

		const auto s = QDir( a ).entryList( QDir::Filter::Files | QDir::Filter::Hidden ) ;

		for( const auto& it : s ){

			auto m = _favorites( a + it ) ;

			if( m.has_value() ){

				m_favorites.emplace_back( std::move( m.RValue() ) ) ;
			}
		}
	}else{
		utility::debug() << "Failed To Get Favorites List" ;
	}
}

favorites::error favorites::add( const favorites::entry& e )
{
	auto m = _config_path() ;

	if( !m.has_value() ){

		utility::debug() << "Failed To Get Favorites Path" ;

		return error::FAILED_TO_CREATE_ENTRY ;
	}

	auto a = _create_path( m.value(),e ) ;

	SirikaliJson json( []( const QString& e ){

		Q_UNUSED( e )
	} ) ;

	json[ "volumePath" ]           = e.volumePath ;
	json[ "mountPointPath" ]       = e.mountPointPath ;
	json[ "mountOptions" ]         = e.mountOptions ;
	json[ "preMountCommand" ]      = e.preMountCommand ;
	json[ "postMountCommand" ]     = e.postMountCommand ;
	json[ "preUnmountCommand" ]    = e.preUnmountCommand ;
	json[ "postUnmountCommand" ]   = e.postUnmountCommand ;
	json[ "password" ]             = e.password ;

	favorites::triState::writeTriState( json,e.readOnlyMode,"mountReadOnly" ) ;
	favorites::triState::writeTriState( json,e.autoMount,"autoMountVolume" ) ;

	if( utility::pathExists( a ) ){

		utility::debug() << "Favorite Entry Already Exist" ;
		return error::ENTRY_ALREADY_EXISTS ;
	}else{
		if( json.passed() && json.toFile( a ) ){

			while( true ){

				if( utility::pathExists( a ) ){

					break ;
				}else{
					utility::debug() << "Waiting for a file to show up to the file system" ;
					utility::Task::suspendForOneSecond() ;
				}
			}

			this->reload() ;
			return error::SUCCESS ;
		}else{
			utility::debug() << "Failed To Create Favorite Entry" ;
			return error::FAILED_TO_CREATE_ENTRY ;
		}
	}
}

const std::vector< favorites::entry >& favorites::readFavorites() const
{
	return m_favorites ;
}

utility2::result_ref< const favorites::entry& > favorites::readFavoriteByPath( const QString& configPath ) const
{
	auto path = QDir( configPath ).absolutePath() ;

	for( const auto& it : m_favorites ){

		if( it.internalConfigPath == path ){

			return it ;
		}
	}

	return {} ;
}

utility2::result< favorites::entry > favorites::readFavoriteByFileSystemPath( const QString& path ) const
{
	return _favorites( path ) ;
}

utility2::result_ref< const favorites::entry& > favorites::readFavorite( const QString& volumePath,
									 const QString& mountPath ) const
{
	if( mountPath.isEmpty() ){

		for( const auto& it : m_favorites ){

			if( it.volumePath == volumePath ){

				return it ;
			}
		}
	}else{
		for( const auto& it : m_favorites ){

			if( it.volumePath == volumePath && it.mountPointPath == mountPath ){

				return it ;
			}
		}
	}

	return {} ;
}

void favorites::replaceFavorite( const favorites::entry& old,const favorites::entry& New )
{
	this->removeFavoriteEntry( old ) ;
	this->add( New ) ;
}

template< typename Function >
static bool _remove( const favorites::entry& e,Function function )
{
	auto s = function( e ) ;

	if( !s.isEmpty() && utility::pathExists( s ) ){

		return QFile::remove( s ) ;
	}else{
		return false ;
	}
}

void favorites::removeFavoriteEntry( const favorites::entry& e )
{
	if( _remove( e,_create_entry_path ) ){

		this->reload() ;

	}else if( _remove( e,_create_entry_path ) ){

		this->reload() ;
	}else{
		utility::debug() << "Failed To Remove Favorite Entry: " + e.volumePath ;
	}
}

favorites::entry::entry()
{
}

favorites::entry::entry( const QString& e,const QString& mountPath ) :
	volumePath( e ),mountPointPath( mountPath )
{
}

void favorites::volEntry::setAutoMount( bool s )
{
	auto m = m_tmpFavorite.get() ;

	if( m ){

		utility::debug() << "Changing a setting of a temporary favorite: " + m->volumePath ;

		m->autoMount = s ;
	}else{
		QString aa = "Creating a copy of a favorite at: %1\nto change a setting" ;

		utility::debug() << aa.arg( m_favorite->volumePath ) ;

		auto a = *m_favorite ;
		a.autoMount = s ;

		m_favorite = this->entry( std::move( a ) ) ;
	}
}

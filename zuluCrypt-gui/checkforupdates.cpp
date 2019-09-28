/*
 *
 *  Copyright (c) 2015
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

#include "checkforupdates.h"
#include "dialogmsg.h"
#include "version_1.h"
#include "utility.h"

#include "json.h"

static const int _timeOut = 10 ;

checkForUpdates::checkForUpdates( QWidget * widget ) :
	m_widget( widget ),
	m_networkRequest( QUrl( "https://api.github.com/repos/mhogomchungu/zuluCrypt/releases" ) )
{
	m_networkRequest.setRawHeader( "Host","api.github.com" ) ;
	m_networkRequest.setRawHeader( "Accept-Encoding","text/plain" ) ;
}

void checkForUpdates::check( bool s )
{
	m_autocheck = s ;

	m_network.get( _timeOut,m_networkRequest,[ this ]( QNetworkReply& e ){

		this->showResult( this->parseResult( e.readAll() ) ) ;

	},[ this ](){

		auto s = QString::number( _timeOut ) ;
		auto e = QObject::tr( "Network Request Failed To Respond Within %1 Seconds." ).arg( s ) ;

		DialogMsg( m_widget ).ShowUIOK( QObject::tr( "ERROR" ),e ) ;
	} ) ;
}

void checkForUpdates::run( const QString& e )
{
	Q_UNUSED( e )

	if( utility::settingsObject().value( "autoChecForkUpdates" ).toBool() ){

		this->check( true ) ;
	}
}

void checkForUpdates::run()
{
	this->check( false ) ;
}

QString checkForUpdates::parseResult( const QByteArray& data )
{
	if( data.isEmpty() ){

		return QString() ;
	}

	auto _found_release = []( const QString& e ){

		for( const auto& it : e ){

			/*
			 * A release version has version in format of "A.B.C"
			 *
			 * ie it only has dots and digits. Presence of any other
			 * character makes the release assumed to be a beta/alpha
			 * or prerelease version(something like "A.B.C-rc1" or
			 * "A.B.C.beta6"
			 */
			if( it != '.' && !( it >= '0' && it <= '9' ) ){

				return false ;
			}
		}

		return true ;
	} ;

	for( const auto& it : nlohmann::json::parse( data.constData() ) ){

		auto e = it.find( "tag_name" ) ;

		if( e != it.end() ){

			auto r = QString::fromStdString( e.value() ).remove( 'v' ) ;

			if( _found_release( r ) ){

				return r ;
			}
		}
	}

	return QString() ;
}

void checkForUpdates::showResult( const QString& l )
{
	DialogMsg msg( m_widget ) ;

	if( l.isEmpty() ){

		if( !m_autocheck ){

			msg.ShowUIOK( QObject::tr( "ERROR" ),QObject::tr( "Failed To Check For Update." ) ) ;
		}
	}else{
		auto _tr = []( const QString& a,const QString& b ){

			return QObject::tr( "\nInstalled Version Is : %1.\nLatest Version Is : %2.\n" ).arg( a,b ) ;
		} ;

		if( m_autocheck ){

			if( l != THIS_VERSION ){

				msg.ShowUIOK( QObject::tr( "Update Available" ),_tr( THIS_VERSION,l ) ) ;
			}
		}else{
			msg.ShowUIOK( QObject::tr( "Version Info" ),_tr( THIS_VERSION,l ) ) ;
		}
	}
}

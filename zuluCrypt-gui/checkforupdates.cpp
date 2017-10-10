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

checkForUpdates::checkForUpdates( QWidget * widget,bool autocheck ) :
		m_widget( widget ),m_autocheck( autocheck )
{
	QUrl url( "https://api.github.com/repos/mhogomchungu/zuluCrypt/releases" ) ;

	QNetworkRequest e ;

	e.setRawHeader( "Host","api.github.com" ) ;
	e.setRawHeader( "Accept-Encoding","text/plain" ) ;

	e.setUrl( url ) ;

	m_timer.setInterval( 1000 * _timeOut ) ;

	connect( &m_timer,SIGNAL( timeout() ),this,SLOT( timeOut() ),Qt::QueuedConnection ) ;

	m_timer.start() ;

	m_network.get( &m_networkReply,e,[ this ]( QNetworkReply& e ){

		m_timer.stop() ;

		this->showResult( this->parseResult( e.readAll() ) ) ;

		this->deleteLater() ;
	} ) ;
}

void checkForUpdates::timeOut()
{
	m_timer.stop() ;

	if( m_network.cancel( m_networkReply ) ){

		auto s = QString::number( _timeOut ) ;
		auto e = tr( "Network Request Failed To Respond Within %1 Seconds." ).arg( s ) ;

		DialogMsg( m_widget ).ShowUIOK( tr( "ERROR" ),e ) ;

		this->deleteLater() ;
	}
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

			msg.ShowUIOK( tr( "ERROR" ),tr( "Failed To Check For Update." ) ) ;
		}
	}else{
		auto _tr = []( const QString& a,const QString& b ){

			return QObject::tr( "\nInstalled Version Is : %1.\nLatest Version Is : %2.\n" ).arg( a,b ) ;
		} ;

		if( m_autocheck ){

			if( l != THIS_VERSION ){

				msg.ShowUIOK( tr( "Update Available" ),_tr( THIS_VERSION,l ) ) ;
			}
		}else{
			msg.ShowUIOK( tr( "Version Info" ),_tr( THIS_VERSION,l ) ) ;
		}
	}
}

void checkForUpdates::instance( QWidget * widget,const QString& e )
{
	if( utility::pathExists( utility::homePath() + "/.zuluCrypt/autoCheckUpdates." + e ) ){

		new checkForUpdates( widget,true ) ;
	}
}

void checkForUpdates::instance( QWidget * widget )
{
	new checkForUpdates( widget,false ) ;
}

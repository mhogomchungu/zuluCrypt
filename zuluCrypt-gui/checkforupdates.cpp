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

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#include <QWidget>

#include "utility.h"
#include "dialogmsg.h"
#include "version_1.h"

void checkForUpdates::networkReply( QNetworkReply * q )
{
	utility::qObject_unique_ptr< QNetworkReply > p( q ) ;

	QString l = p->readAll() ;

	DialogMsg msg( m_widget ) ;

	if( l.isEmpty() ){

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed To Check For Update." ) ) ;

		return ;
	}

	l.replace( "\n","" ) ;

	if( m_autocheck ){

		if( l != "Not Found" && l != THIS_VERSION ){

			l = tr( "\nInstalled Version Is : %1.\nLatest Version Is : %2.\n" ).arg( THIS_VERSION,l ) ;
			msg.ShowUIOK( tr( "Update Available" ),l ) ;
		}
	}else{
		if( l != "Not Found" ){

			l = tr( "\nInstalled Version Is : %1.\nLatest Version Is : %2.\n" ).arg( THIS_VERSION,l ) ;
			msg.ShowUIOK( tr( "Version Info" ),l ) ;
		}else{
			msg.ShowUIOK( tr( "ERROR" ),tr( "Failed To Check For Update." ) ) ;
		}
	}
}

checkForUpdates::checkForUpdates( QWidget * widget,bool autocheck ) : m_widget( widget ),m_autocheck( autocheck )
{
	connect( &m_manager,SIGNAL( finished( QNetworkReply * ) ),this,SLOT( networkReply( QNetworkReply * ) ) ) ;

	QNetworkRequest r( QUrl( "https://raw.githubusercontent.com/mhogomchungu/zuluCrypt/master/version" ) ) ;

	r.setRawHeader( "Host","raw.githubusercontent.com" ) ;
	r.setRawHeader( "User-Agent","Mozilla/5.0 (X11; Linux x86_64; rv:39.0) Gecko/20100101 Firefox/39.0" ) ;
	r.setRawHeader( "Accept-Encoding","text/plain" ) ;

	m_manager.get( r ) ;
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

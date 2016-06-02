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

#include <QWidget>

#include "utility.h"
#include "dialogmsg.h"
#include "version_1.h"

static QString _tr( const QString& a,const QString& b )
{
	return QObject::tr( "\nInstalled Version Is : %1.\nLatest Version Is : %2.\n" ).arg( a,b ) ;
}

checkForUpdates::checkForUpdates( QWidget * widget,bool autocheck ) :
        m_widget( widget ),m_autocheck( autocheck )
{
        m_networkAccessManager.get( [](){

                QUrl url( "https://raw.githubusercontent.com/mhogomchungu/zuluCrypt/master/version" ) ;

                QNetworkRequest e( url ) ;

                e.setRawHeader( "Host","raw.githubusercontent.com" ) ;
                e.setRawHeader( "Accept-Encoding","text/plain" ) ;

                return e ;

        }(),[ this ]( NetworkAccessManager::NetworkReply e ){

		QString l = e->readAll() ;

		DialogMsg msg( m_widget ) ;

		if( l.isEmpty() ){

			if( !m_autocheck ){

				msg.ShowUIOK( tr( "ERROR" ),tr( "Failed To Check For Update." ) ) ;
			}
		}else{
			l.replace( "\n","" ) ;

			if( m_autocheck ){

				if( l != "Not Found" && l != THIS_VERSION ){

					msg.ShowUIOK( tr( "Update Available" ),_tr( THIS_VERSION,l ) ) ;
				}
			}else{
				if( l != "Not Found" ){

					msg.ShowUIOK( tr( "Version Info" ),_tr( THIS_VERSION,l ) ) ;
				}else{
					msg.ShowUIOK( tr( "ERROR" ),tr( "Failed To Check For Update." ) ) ;
				}
			}
		}

		this->deleteLater() ;
	} ) ;
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

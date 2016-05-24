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

#ifndef CHECKFORUPDATES_H
#define CHECKFORUPDATES_H

#include <QVector>
#include <QObject>
#include <QWidget>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QEventLoop>

#include <functional>
#include <utility>

class NetworkAccessManager : public QObject
{
	Q_OBJECT
public:
	NetworkAccessManager()
	{
		connect( &m_manager,SIGNAL( finished( QNetworkReply * ) ),
			 this,SLOT( networkReply( QNetworkReply * ) ),Qt::QueuedConnection ) ;
	}
	void get( const QNetworkRequest& r,std::function< void( QNetworkReply * ) >&& f )
	{
		m_entries.append( { m_manager.get( r ),f } ) ;
	}
	QNetworkReply * get( const QNetworkRequest& r )
	{
		QNetworkReply * reply ;

		QEventLoop l ;

		this->get( r,[ & ]( QNetworkReply * e ){

			reply = e ;

			l.quit() ;
		} ) ;

		l.exec() ;

		return reply ;
	}
private slots:
	void networkReply( QNetworkReply * r )
	{
		auto s = m_entries.size() ;

		for( decltype( s ) i = 0 ; i < s ; i++ ){

			const auto& q = m_entries.at( i ) ;

			if( q.first == r ){

				q.second( r ) ;

				m_entries.remove( i ) ;

				break ;
			}
		}
	}
private:
	using pair_t = std::pair< QNetworkReply *,std::function< void( QNetworkReply * ) > > ;

	QVector< pair_t > m_entries ;
	QNetworkAccessManager m_manager ;
};

class checkForUpdates : public QObject
{
public:
	static bool autoCheck( void ) ;
	static void autoCheck( bool ) ;

	checkForUpdates( QWidget *,bool ) ;

	static void instance( QWidget *,const QString& ) ;
	static void instance( QWidget * ) ;
private:
	void show( const QByteArray&,const QByteArray& ) ;
	QWidget * m_widget ;
	bool m_autocheck ;
	NetworkAccessManager m_networkAccessManager ;
};

#endif // CHECKFORUPDATES_H

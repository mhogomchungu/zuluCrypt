/*
 *
 *  Copyright (c) 2013
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

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QEventLoop>
#include <QVector>

#include <functional>
#include <utility>
#include <memory>

class NetworkAccessManager : public QObject
{
	Q_OBJECT
public:
	class NetworkReply
	{
	public:
		NetworkReply( QNetworkReply * e ) : m_QNetworkReply( e )
		{
		}
		NetworkReply( NetworkReply&& other )
		{
			if( m_QNetworkReply ){

				m_QNetworkReply->deleteLater() ;
			}

			m_QNetworkReply = other.m_QNetworkReply ;
			other.m_QNetworkReply = nullptr ;
		}
		QNetworkReply * release()
		{
			auto e = m_QNetworkReply ;
			m_QNetworkReply = nullptr ;
			return e ;
		}
		~NetworkReply()
		{
			if( m_QNetworkReply ){

				m_QNetworkReply->deleteLater() ;
			}
		}
		QNetworkReply * operator->()
		{
			return m_QNetworkReply ;
		}
	private:
		QNetworkReply * m_QNetworkReply = nullptr ;
	};

	using function_t = std::function< void( NetworkAccessManager::NetworkReply ) > ;

	NetworkAccessManager()
	{
		connect( &m_manager,SIGNAL( finished( QNetworkReply * ) ),
			 this,SLOT( networkReply( QNetworkReply * ) ),Qt::QueuedConnection ) ;
	}
	QNetworkAccessManager * getQNetworkAccessManager()
	{
		return &m_manager ;
	}
	void get( const QNetworkRequest& r,function_t&& f )
	{
		m_entries.append( { m_manager.get( r ),std::move( f ) } ) ;
	}
	void get( const QNetworkRequest& r,QNetworkReply ** e,function_t&& f )
	{
		auto q = m_manager.get( r ) ;

		*e = q ;

		m_entries.append( { q,std::move( f ) } ) ;
	}
	QNetworkReply * get( const QNetworkRequest& r )
	{
		QNetworkReply * q ;

		QEventLoop l ;

		this->get( r,[ & ]( NetworkAccessManager::NetworkReply e ){

			q = e.release() ;

			l.quit() ;
		} ) ;

		l.exec() ;

		return q ;
	}
	template< typename T >
	void post( const QNetworkRequest& r,const T& e,function_t&& f )
	{
		m_entries.append( { m_manager.post( r,e ),std::move( f ) } ) ;
	}
	template< typename T >
	void post( const QNetworkRequest& r,const T& e,QNetworkReply ** z,function_t&& f )
	{
		auto q = m_manager.post( r,e ) ;

		*z = q ;

		m_entries.append( { q,std::move( f ) } ) ;
	}
	template< typename T >
	QNetworkReply * post( const QNetworkRequest& r,const T& e )
	{
		QNetworkReply * q ;

		QEventLoop l ;

		this->post( r,e,[ & ]( NetworkAccessManager::NetworkReply e ){

			q = e.release() ;

			l.quit() ;
		} ) ;

		l.exec() ;

		return q ;
	}
	void head( const QNetworkRequest& r,function_t&& f )
	{
		m_entries.append( { m_manager.head( r ),std::move( f ) } ) ;
	}
	void head( const QNetworkRequest& r,QNetworkReply ** e,function_t&& f )
	{
		auto q = m_manager.head( r ) ;

		*e = q ;

		m_entries.append( { q,std::move( f ) } ) ;
	}
	QNetworkReply * head( const QNetworkRequest& r )
	{
		QNetworkReply * q ;

		QEventLoop l ;

		this->head( r,[ & ]( NetworkAccessManager::NetworkReply e ){

			q = e.release() ;

			l.quit() ;
		} ) ;

		l.exec() ;

		return q ;
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
	QVector< std::pair< QNetworkReply *,function_t > > m_entries ;
	QNetworkAccessManager m_manager ;
};

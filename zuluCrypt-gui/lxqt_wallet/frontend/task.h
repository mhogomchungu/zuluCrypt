/*
 *
 *  Copyright (c) 2014
 *  name : mhogo mchungu
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

#ifndef TASK_H
#define TASK_H

#include <functional>
#include <QThread>
#include <QDebug>
#include <QEventLoop>

namespace LxQt{

namespace Wallet{

namespace Task
{
	class Thread : public QThread
	{
		Q_OBJECT
	public:
		Thread()
		{
			connect( this,SIGNAL( finished() ),this,SLOT( deleteLater() ) ) ;
		}
	protected:
		virtual ~Thread()
		{
		}
	private:
		virtual void run( void )
		{
		}
	};

	template< typename T >
	class future
	{
	public:
		future() : m_function( []( const T& t ){ Q_UNUSED( t ) ; } )
		{
		}
		void setActions( std::function< void( void ) > start,
				 std::function< void( void ) > cancel )
		{
			m_start = start ;
			m_cancel= cancel ;
		}
		void then( std::function< void( const T& ) > function )
		{
			m_function = function ;
			m_start() ;
		}
		void start()
		{
			m_start() ;
		}
		void cancel()
		{
			m_cancel() ;
		}
		void run( const T& arg )
		{
			m_function( arg ) ;
		}
	private:
		std::function< void( const T& ) > m_function ;
		std::function< void( void ) > m_start ;
		std::function< void( void ) > m_cancel ;
	};

	template< typename T >
	class ThreadHelper : public Thread
	{
	public:
		ThreadHelper( std::function< T ( void ) > function ) :m_function( function )
		{
		}
		future<T>& taskContinuation( void )
		{
			m_future.setActions( [ this ](){ this->start() ; },
					     [ this ](){ this->deleteLater() ; } ) ;
			return m_future ;
		}
	private:
		~ThreadHelper()
		{
			m_future.run( m_cargo ) ;
		}
		void run( void )
		{
			m_cargo =  m_function() ;
		}
		std::function< T ( void ) > m_function ;
		future<T> m_future ;
		T m_cargo ;
	};

	class future_1
	{
	public:
		future_1() : m_function( [](){} )
		{
		}
		void setActions( std::function< void( void ) > start,
				 std::function< void( void ) > cancel )
		{
			m_start = start ;
			m_cancel= cancel ;
		}
		void then( std::function< void( void ) > function )
		{
			m_function = function ;
			m_start() ;
		}
		void start()
		{
			m_start() ;
		}
		void run()
		{
			m_function() ;
		}
		void cancel()
		{
			m_cancel() ;
		}
	private:
		std::function< void( void ) > m_function ;
		std::function< void( void ) > m_start ;
		std::function< void( void ) > m_cancel ;
	};

	class ThreadHelper_1 : public Thread
	{
	public:
		ThreadHelper_1( std::function< void ( void ) > function ) : m_function( function )
		{
		}
		future_1& taskContinuation( void )
		{
			m_future.setActions( [ this ](){ this->start() ; },
					     [ this ](){ this->deleteLater() ; } ) ;
			return m_future ;
		}
	private:
		~ThreadHelper_1()
		{
			m_future.run() ;
		}
		void run( void )
		{
			m_function() ;
		}
		std::function< void ( void ) > m_function ;
		future_1 m_future ;
	};

	/*
	 * Below APIs runs two tasks,the first one will be run in a different thread and
	 * the second one will be run on the original thread after the completion of the
	 * first one.
	 */

	template< typename T >
	future<T>& run( std::function< T ( void ) > function )
	{
		auto t = new ThreadHelper<T>( function ) ;
		return t->taskContinuation() ;
	}

	static inline future_1& run( std::function< void( void ) > function )
	{
		auto t = new ThreadHelper_1( function ) ;
		return t->taskContinuation() ;
	}

	static inline void exec( std::function< void( void ) > function )
	{
		Task::run( function ).start() ;
	}

	/*
	 * Below APIs implements resumable functions where a function will be "blocked"
	 * waiting for the function to return without "hanging" the current thread.
	 *
	 * recommending reading up on C#'s await keyword to get a sense of what is being
	 * discussed below.
	 */

	static inline void await( Task::future_1& t )
	{
		QEventLoop p ;

		t.then( [ & ](){ p.exit() ; } ) ;

		p.exec() ;
	}

	static inline void await( std::function< void( void ) > function )
	{
		Task::await( Task::run( function ) ) ;
	}

	template< typename T >
	T await( Task::future<T>& t )
	{
		QEventLoop p ;
		T q ;

		t.then( [ & ]( const T& r ){  q = r ; p.exit() ; } ) ;

		p.exec() ;

		return q ;
	}

	template< typename T >
	T await( std::function< T ( void ) > function )
	{
		return Task::await( Task::run( function ) ) ;
	}
}

}

}

#if 0

/*
 * templated version that passes a return value of one function to another function
 */
auto _a = [](){
	/*
	 * task _a does what task _a does here.
	 *
	 * This function body will run on a different thread
	 */
	return 0 ;
}

auto _b = []( const int& r ){
	/*
	 * task _b does what task _b does here.
	 *
	 * r is a const reference to a value returned by _a
	 *
	 * This function body will run on the original thread
	 */
}

Task::run<int>( _a ).then( _b ) ;

/*
 * Non templated version that does not pass around return value
 */
auto _c = [](){
	/*
	 * task _a does what task _a does here.
	 *
	 * This function body will run on a different thread
	 */
}

auto _d = [](){
	/*
	 * task _b does what task _b does here.
	 *
	 * r is a const reference to a value returned by _a
	 *
	 * This function body will run on the original thread
	 */
}

Task::run( _c ).then( _d ) ;

/*
 * if no continuation
 */
Task::exec( _c ) ;

/*
 * Task::await() is used to "block" the calling thread until the function returns.
 *
 * Its use case is to do sync programming without hanging the calling thread.
 *
 * example use case for it is to "block" on function in a GUI thread withough blocking the GUI thread
 * hanging the application.
 */

/*
 * await example when the called function return no result
 */
Task::await( _c ) ;

/*
 * await example when the called function return a result
 */
int r = Task::await<int>( _a ) ;

#endif

#endif // TASK_H

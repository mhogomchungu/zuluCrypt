/*
 * copyright: 2014
 * name : mhogo mchungu
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __TASK_H_INCLUDED__
#define __TASK_H_INCLUDED__

#include <utility>
#include <future>
#include <functional>
#include <QThread>
#include <QEventLoop>

/*
 *
 * Examples on how to use the library are at the end of this file.
 *
 */

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
		future() : m_function( []( T t ){ Q_UNUSED( t ) ; } )
		{
		}
		void setActions( std::function< void( void ) > start,
				 std::function< void( void ) > cancel,
				 std::function< T ( void ) > get )
		{
			m_start  = std::move( start ) ;
			m_cancel = std::move( cancel ) ;
			m_get    = std::move( get ) ;
		}
		void then( std::function< void( T ) > function )
		{
			m_function = std::move( function ) ;
			m_start() ;
		}
		T get()
		{
			return m_get() ;
		}
		T await()
		{
			QEventLoop p ;

			T q ;

			m_function = [ & ]( T r ){ q = std::move( r ) ; p.exit() ; } ;

			m_start() ;

			p.exec() ;

			return q ;
		}
		void start()
		{
			m_start() ;
		}
		void cancel()
		{
			m_cancel() ;
		}
		void run( T r )
		{
			m_function( std::move( r ) ) ;
		}
	private:
		std::function< void( T ) > m_function ;
		std::function< void( void ) > m_start ;
		std::function< void( void ) > m_cancel ;
		std::function< T ( void ) > m_get ;
	};

	template< typename T >
	class ThreadHelper : public Thread
	{
	public:
		ThreadHelper( std::function< T ( void ) >&& function ) : m_function( std::move( function ) )
		{
		}
		future<T>& Future( void )
		{
			m_future.setActions( [ this ](){ this->start() ; },
					     [ this ](){ this->deleteLater() ; },
					     [ this ](){ T r = m_function() ; this->deleteLater() ; return r ; } ) ;
			return m_future ;
		}
	private:
		~ThreadHelper()
		{
			m_future.run( std::move( m_cargo ) ) ;
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
				 std::function< void( void ) > cancel,
				 std::function< void( void ) > get )
		{
			m_start  = std::move( start ) ;
			m_cancel = std::move( cancel ) ;
			m_get    = std::move( get ) ;
		}
		void then( std::function< void( void ) > function )
		{
			m_function = std::move( function ) ;
			m_start() ;
		}
		void get()
		{
			m_get() ;
		}
		void await()
		{
			QEventLoop p ;

			m_function = [ & ](){ p.exit() ; } ;

			m_start() ;

			p.exec() ;
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
		std::function< void( void ) > m_get ;
	};

	class ThreadHelper_1 : public Thread
	{
	public:
		ThreadHelper_1( std::function< void ( void ) >&& function ) : m_function( std::move( function ) )
		{
		}
		future_1& Future( void )
		{
			m_future.setActions( [ this ](){ this->start() ; },
					     [ this ](){ this->deleteLater() ; },
					     [ this ](){ m_function() ; this->deleteLater() ; } ) ;
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
	 * Below APIs runs two tasks,the first one will run in a different thread and
	 * the second one will be run on the original thread after the completion of the
	 * first one.
	 */

	template< typename T >
	future<T>& run( std::function< T ( void ) > function )
	{
		auto t = new ThreadHelper<T>( std::move( function ) ) ;
		return t->Future() ;
	}

	static inline future_1& run( std::function< void( void ) > function )
	{
		auto t = new ThreadHelper_1( std::move( function ) ) ;
		return t->Future() ;
	}

	static inline void exec( std::function< void( void ) > function )
	{
		Task::run( std::move( function ) ).start() ;
	}

	/*
	 * Below APIs implements resumable functions where a function will be "blocked"
	 * waiting for the function to return without "hanging" the current thread.
	 *
	 * recommending reading up on C#'s await keyword to get a sense of what is being
	 * discussed below.
	 */

	static inline void await( Task::future_1& e )
	{
		e.await() ;
	}

	static inline void await( std::function< void( void ) > function )
	{
		Task::run( std::move( function ) ).await() ;
	}

	template< typename T >
	T await( std::function< T ( void ) > function )
	{
		return Task::run<T>( std::move( function ) ).await() ;
	}

	template< typename T >
	T await( Task::future<T>& e )
	{
		return e.await() ;
	}

	template< typename T >
	T await( std::future<T>&& t )
	{
		return Task::await<T>( [ & ](){ return t.get() ; } ) ;
	}
}

}

}

#if 0

/*
 * Examples on how to use the library
 */
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

auto _b = []( int r ){
	/*
	 *
	 * task _b does what task _b does here.
	 *
	 * r is a const reference to a value returned by _a
	 *
	 * This function body will run on the original thread
	 */
}

Task::run<int>( _a ).then( _b ) ;

alternatively,

Task::future<int>& e = Task::run( _a ) ;

e.then( _b ) ;

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
 * Task::await() is used to "block" without "hanging" the calling thread until the function returns.
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

alternatively,

Task::future<int>& e = Task::run<int>( _a ) ;

int r = e.await() ;

alternatively,

int r = Task::run<int>( _a ).await() ;

#endif

#endif //__TASK_H_INCLUDED__

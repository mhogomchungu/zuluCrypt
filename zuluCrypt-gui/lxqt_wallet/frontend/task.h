/*
 * copyright: 2014
 * name : Francis Banyikwa
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

/*
 * This library wraps a function into a future where the result of the function
 * can be retrieved through the future's 3 public methods:
 *
 * 1. .get() runs the wrapped function on the current thread.
 *
 * 2. .then() registers an event to be called when the wrapped function finishes
 *            and then runs the wrapped function in a different thread.
 *            The registered function will run in the current thread.
 *
 * 3. .await() suspends the calling function and then runs the wrapped function
 *             in a separate thread and then unsuspends the calling function when
 *             the wrapped function finish running.The suspension will be done
 *             without blocking the current thread leaving free to perform other tasks.
 *
 *             recommending reading up on C#'s await keyword to get a sense of how this feature works.
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
		virtual void run()
		{
		}
	};

	template< typename T >
	class future
	{
	public:
		future( std::function< void() > start,
			std::function< void() > cancel,
			std::function< void( T& ) > get ) :
			m_start ( std::move( start ) ),
			m_cancel( std::move( cancel ) ),
			m_get   ( std::move( get ) )
		{
		}
		void then( std::function< void( T ) > function )
		{
			m_function = std::move( function ) ;
			this->start() ;
		}
		T get()
		{
			T r ;
			m_get( r ) ;
			return r ;
		}
		T await()
		{
			QEventLoop p ;

			T q ;

			m_function = [ & ]( T r ){ q = std::move( r ) ; p.exit() ; } ;

			this->start() ;

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
		std::function< void( T ) > m_function = []( T t ){ Q_UNUSED( t ) ; } ;
		std::function< void() > m_start ;
		std::function< void() > m_cancel ;
		std::function< void( T& ) > m_get ;
	};

	template< typename T >
	class ThreadHelper : public Thread
	{
	public:
		ThreadHelper( std::function< T() > function ) :
			m_function( std::move( function ) ),
			m_future( [ this ](){ this->start() ; },
				  [ this ](){ this->deleteLater() ; },
				  [ this ]( T& r ){ r = m_function() ; this->deleteLater() ; } )
		{
		}
		future<T>& Future()
		{
			return m_future ;
		}
	private:
		~ThreadHelper()
		{
			m_future.run( std::move( m_result ) ) ;
		}
		void run()
		{
			m_result =  m_function() ;
		}
		std::function< T() > m_function ;
		future<T> m_future ;
		T m_result ;
	};

	template<>
	class future< void >
	{
	public:
		future( std::function< void() > start,
			std::function< void() > cancel,
			std::function< void() > get ) :
			m_start ( std::move( start ) ),
			m_cancel( std::move( cancel ) ),
			m_get   ( std::move( get ) )
		{
		}
		void then( std::function< void() > function )
		{
			m_function = std::move( function ) ;
			this->start() ;
		}
		void get()
		{
			m_get() ;
		}
		void await()
		{
			QEventLoop p ;

			m_function = [ & ](){ p.exit() ; } ;

			this->start() ;

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
		std::function< void() > m_function = [](){} ;
		std::function< void() > m_start ;
		std::function< void() > m_cancel ;
		std::function< void() > m_get ;
	};

	template<>
	class ThreadHelper< void > : public Thread
	{
	public:
		ThreadHelper( std::function< void() > function ) :
			m_function( std::move( function ) ),
			m_future( [ this ](){ this->start() ; },
				  [ this ](){ this->deleteLater() ; },
				  [ this ](){ m_function() ; this->deleteLater() ; } )
		{
		}
		future< void >& Future()
		{
			return m_future ;
		}
	private:
		~ThreadHelper()
		{
			m_future.run() ;
		}
		void run()
		{
			m_function() ;
		}
		std::function< void() > m_function ;
		future< void > m_future ;
	};

	/*
	 * Below API's wrappes a function around a future and then returns the future.
	 */
	template< typename T >
	future<T>& run( std::function< T() > function )
	{
		auto t = new ThreadHelper<T>( std::move( function ) ) ;
		return t->Future() ;
	}

	static inline future< void >& run( std::function< void() > function )
	{
		auto t = new ThreadHelper< void >( std::move( function ) ) ;
		return t->Future() ;
	}

	/*
	 * A few useful helper functions
	 */

	static inline void await( Task::future<void>& e )
	{
		e.await() ;
	}

	static inline void await( std::function< void() > function )
	{
		Task::run( std::move( function ) ).await() ;
	}

	template< typename T >
	T await( std::function< T() > function )
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

	/*
	 * This method runs its argument in a separate thread and does not offer
	 * continuation feature.Useful when wanting to just run a function in a
	 * different thread.
	 */
	static inline void exec( std::function< void() > function )
	{
		Task::run( std::move( function ) ).start() ;
	}
}

}

}

#if 0 // start example block

Examples on how to use the library

**********************************************************
* Example use cases on how to use Task::run().then() API
**********************************************************

templated version that passes a return value of one function to another function
---------------------------------------------------------------------------------

auto _a = [](){
	/*
	 * This task will run on a different thread
	 * This tasks returns a result
	 */
	return 0 ;
}

auto _b = []( int r ){
	/*
	 * This task will run on the original thread.
	 * This tasks takes an argument returned by task _a
	 */
}

Task::run<int>( _a ).then( _b ) ;

alternatively,

Task::future<int>& e = Task::run( _a ) ;

e.then( _b ) ;


Non templated version that does not pass around return value
----------------------------------------------------------------
auto _c = [](){
	/*
	 * This task will run on a different thread
	 * This tasks returns with no result
	 */
}

auto _d = [](){
	/*
	 * This task will run on the original thread.
	 * This tasks takes no argument
	 */
}

Task::run( _c ).then( _d ) ;

alternatively,

Task::future<void>& e = Task::run( _c ) ;

e.then( _d ) ;

**********************************************************
* Example use cases on how to use Task::run().await() API
**********************************************************

int r = Task::await<int>( _a ) ;

alternatively,

Task::future<int>& e = Task::run<int>( _a ) ;

int r = e.await() ;

alternatively,

int r = Task::run<int>( _a ).await() ;

#endif //end example block

#endif //__TASK_H_INCLUDED__

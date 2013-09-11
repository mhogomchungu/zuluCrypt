/*
 * copyright: 2013
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

#include "open_wallet_thread.h"

openWalletThread::openWalletThread( lxqt_wallet_t * wallet,QString password,QString walletName,QString applicationName )
{
	m_wallet          = wallet ;
	m_password        = password ;
	m_walletName      = walletName ;
	m_applicationName = applicationName ;
}

openWalletThread::openWalletThread( QString password,QString walletName,QString applicationName )
{
	m_password        = password ;
	m_walletName      = walletName ;
	m_applicationName = applicationName ;
}

openWalletThread::openWalletThread( int (*f)( const void * ),const void * schema )
{
	m_schema   = schema ;
	m_function = f ;
}

void openWalletThread::start( openWalletThread::action action )
{
	m_action = action ;
	QThreadPool::globalInstance()->start( this ) ;
}

void openWalletThread::run()
{
	if( m_action == openWalletThread::openInternal ){
		lxqt_wallet_error r = lxqt_wallet_open( m_wallet,m_password.toAscii().constData(),m_password.size(),
						     m_walletName.toAscii().constData(),m_applicationName.toAscii().constData() ) ;
		emit walletOpened( r == lxqt_wallet_no_error ) ;
	}else if( m_action == openWalletThread::openSecretService ){
		emit walletOpened( m_function( m_schema ) ) ;
	}else if( m_action == openWalletThread::createVolume ) {

		lxqt_wallet_error r = lxqt_wallet_create( m_password.toAscii().constData(),m_password.size(),
				    m_walletName.toAscii().constData(),m_applicationName.toAscii().constData() ) ;
		if( r != lxqt_wallet_no_error ){
			emit openWalletThreadResult( false ) ;
		}else{
			emit openWallet( m_password ) ;
		}
	}
}

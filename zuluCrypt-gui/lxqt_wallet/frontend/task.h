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

#ifndef TASK_H
#define TASK_H

#include <QRunnable>
#include <QThreadPool>
#include <QObject>
#include <QString>
#include <QStringList>

#include "../backend/lxqtwallet.h"

namespace lxqt{
	
namespace Wallet{
	
class Task : public QObject,public QRunnable
{
	Q_OBJECT
public:
	typedef enum{
		openInternal,
		openSecretService,
		createVolume
	}action ;
	Task( lxqt_wallet_t * wallet,QString password,QString walletName,QString applicationName ) ;
	Task( QString password,QString walletName,QString applicationName ) ;
	Task( int (*)( const void * ),const void * ) ;
	void start( lxqt::Wallet::Task::action ) ;
signals:
	void walletOpened( bool ) ;
	void taskResult( bool ) ;
	void openWallet( QString ) ;
private:
	void run( void ) ;
	lxqt_wallet_t * m_wallet ;
	QString m_password ;
	QString m_walletName ;
	QString m_applicationName ;
	lxqt::Wallet::Task::action m_action ;
	const void * m_schema ;
	int ( * m_function )( const void * ) ;
};

}

}
#endif // TASK_H

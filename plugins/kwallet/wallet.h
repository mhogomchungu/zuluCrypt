/*
 *
 *  Copyright (c) 2012
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

#ifndef WALLET_H
#define WALLET_H

#include <kwallet.h>
#include <QDebug>
#include <QObject>
#include <QFile>
#include <QString>
#include <QCoreApplication>
#include <QMap>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

#include "../../zuluCrypt-gui/zuluoptions.h"

using namespace KWallet ;

class wallet : public QObject
{
	Q_OBJECT
public:
	wallet( int,char * argv[] );
	~wallet();

private slots:
	void gotConnected( void ) ;
	void openWallet( void ) ;
	void walletOpened( bool ) ;
	void sendKey( void ) ;
private:
	void openConnection();
	void Exit( int ) ;
	void getKey( void ) ;
	bool m_walletOpened ;
	Wallet * m_wallet ;
	int m_argc ;
	QString m_device ;
	QString m_uuid ;
	int m_bufferSize ;
	QString m_sockpath ;
	QLocalServer * m_server ;
	QString m_key ;
	QLocalSocket * m_socket ;
};

#endif // WALLET_H

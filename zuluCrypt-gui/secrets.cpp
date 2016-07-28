/*
 *
 *  Copyright ( c ) 2016
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "secrets.h"
#include "utility.h"

#include <QEventLoop>

secrets::secrets( QWidget * parent ) : m_parent( parent )
{
}

void secrets::changeInternalWalletPassword( const QString& walletName,const QString& appName )
{
	auto wallet = this->walletBk( LXQt::Wallet::BackEnd::internal ) ;

	QEventLoop loop ;

	wallet->changeWalletPassWord( walletName,appName,[ & ]( bool e ){

		Q_UNUSED( e ) ;

		loop.exit() ;
	} ) ;

	loop.exec() ;
}

secrets::~secrets()
{
	delete m_internalWallet ;
}

secrets::wallet secrets::walletBk( LXQt::Wallet::BackEnd e )
{
	if( e == LXQt::Wallet::BackEnd::internal ){

		if( m_internalWallet == nullptr ){

			m_internalWallet = LXQt::Wallet::getWalletBackend( e ) ;

			m_internalWallet->setParent( m_parent ) ;
		}

		return m_internalWallet ;
	}else{
		return LXQt::Wallet::getWalletBackend( e ) ;
	}
}

QWidget * secrets::parent()
{
	return m_parent ;
}

void secrets::setParent( QWidget * w )
{
	m_parent = w ;
}

static void _delete( LXQt::Wallet::Wallet * w )
{
	if( w && w->backEnd() != LXQt::Wallet::BackEnd::internal ){

		w->deleteLater() ;
	}
}

secrets::wallet::wallet()
{
}

secrets::wallet::wallet( LXQt::Wallet::Wallet * w ) : m_wallet( w )
{
}

secrets::wallet::~wallet()
{
	_delete( m_wallet ) ;
}

secrets::wallet::wallet( secrets::wallet&& w )
{
	m_wallet = w.m_wallet ;
	w.m_wallet = nullptr ;
}

secrets::wallet& secrets::wallet::operator=( secrets::wallet&& w )
{
	if( m_wallet != w.m_wallet ){

		_delete( m_wallet ) ;
		m_wallet = w.m_wallet ;
		w.m_wallet = nullptr ;
	}

	return *this ;
}

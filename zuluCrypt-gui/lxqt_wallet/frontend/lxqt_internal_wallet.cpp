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

#include "lxqt_internal_wallet.h"

LxQt::Wallet::internalWallet::internalWallet() : m_wallet( 0 )
{
}

LxQt::Wallet::internalWallet::~internalWallet()
{
	lxqt_wallet_close( &m_wallet ) ;
}

bool LxQt::Wallet::internalWallet::openWallet()
{
	lxqt_wallet_error r = lxqt_wallet_open( &m_wallet,m_password.toLatin1().constData(),m_password.size(),
		      m_walletName.toLatin1().constData(),m_applicationName.toLatin1().constData() ) ;
	return r == lxqt_wallet_no_error ;
}

void LxQt::Wallet::internalWallet::taskResult( bool opened )
{
	emit passwordIsCorrect( opened ) ;
	if( opened ){
		emit getPassWord( m_password ) ;
		emit walletIsOpen( opened ) ;
	}
}

void LxQt::Wallet::internalWallet::setImage( const QString& image )
{
	m_image = image ;
	this->setWindowIcon( QIcon( image ) ) ;
}

bool LxQt::Wallet::internalWallet::openWallet( QString password )
{
	LxQt::Wallet::Task * t = new LxQt::Wallet::Task( &m_wallet,password,m_walletName,m_applicationName ) ;
	if( t ){
		m_password = password ;
		connect( t,SIGNAL( walletOpened( bool ) ),this,SLOT( taskResult( bool ) ) ) ;
		t->start( LxQt::Wallet::Task::openInternal ) ;
	}else{
		this->taskResult( false ) ;
	}
	return false ;
}

void LxQt::Wallet::internalWallet::cancelled()
{
	emit walletIsOpen( false ) ;
}

void LxQt::Wallet::internalWallet::taskResult_1( bool opened )
{
	if( opened ){
		this->taskResult( opened ) ;
	}else{
		/*
		 * passwordless opening failed,prompt a user for a password
		 */
		LxQt::Wallet::password_dialog * p = new LxQt::Wallet::password_dialog( this ) ;
		if( p ){
			connect( p,SIGNAL( password( QString ) ),this,SLOT( openWallet( QString ) ) ) ;
			connect( this,SIGNAL( passwordIsCorrect( bool ) ),p,SLOT( passwordIsCorrect( bool ) ) ) ;
			connect( p,SIGNAL( cancelled() ),this,SLOT( cancelled() ) ) ;
			p->ShowUI( m_walletName,m_displayApplicationName ) ;
		}else{
			this->cancelled() ;
		}
	}
}

void LxQt::Wallet::internalWallet::password( QString password,bool create )
{
	if( create ){
		LxQt::Wallet::Task * t = new LxQt::Wallet::Task( password,m_walletName,m_applicationName ) ;
		if( t ){
			connect( t,SIGNAL( taskResult( bool ) ),this,SLOT( taskResult( bool ) ) ) ;
			connect( t,SIGNAL( openWallet( QString ) ),this,SLOT( openWallet( QString ) ) ) ;
			t->start( LxQt::Wallet::Task::createVolume ) ;
		}else{
			this->taskResult( false ) ;
			emit walletIsOpen( false ) ;
		}
	}
}

void LxQt::Wallet::internalWallet::open( const QString& walletName,const QString& applicationName,
					 const QString& password,const QString& displayApplicationName )
{
	m_walletName      = walletName ;
	m_applicationName = applicationName ;
	m_password        = password ;

	if( m_applicationName.isEmpty() ){
		m_applicationName = m_walletName ;
	}

	if( displayApplicationName.isEmpty() ){
		m_displayApplicationName = m_applicationName ;
	}else{
		m_displayApplicationName = displayApplicationName ;
	}
	
	if( LxQt::Wallet::walletExists( LxQt::Wallet::internalBackEnd,m_walletName,m_applicationName ) ){
		if( m_password.isEmpty() ){
			/*
			 * to prevent an unnecessary prompt,try to open a wallet without a password and then
			 * prompt on failure,this will allow a silent opening of the wallet set without a password.
			 */
			QString passWordLessOpen ;
			LxQt::Wallet::Task * t = new LxQt::Wallet::Task( &m_wallet,passWordLessOpen,m_walletName,m_applicationName ) ;
			if( t ){
				connect( t,SIGNAL( walletOpened( bool ) ),this,SLOT( taskResult_1( bool ) ) ) ;
				t->start( LxQt::Wallet::Task::openInternal ) ;
			}else{
				this->taskResult_1( false ) ;
			}
		}else{
			this->openWallet( m_password ) ;
		}
	}else{
		LxQt::Wallet::changePassWordDialog * c = new LxQt::Wallet::changePassWordDialog( this,m_walletName,m_displayApplicationName ) ;
		if( c ){
			connect( c,SIGNAL( password( QString,bool ) ),this,SLOT( password( QString,bool ) ) ) ;
			c->ShowUI_1() ;
		}else{
			this->password( QString( "" ),false ) ;
		}
	}
}

void LxQt::Wallet::internalWallet::changeWalletPassWord( const QString& walletName,const QString& applicationName )
{
	LxQt::Wallet::changePassWordDialog * c = new LxQt::Wallet::changePassWordDialog( this,walletName,applicationName ) ;
	if( c ){
		connect( c,SIGNAL( walletpassWordChanged( bool ) ),m_interfaceObject,SLOT( walletpassWordChanged( bool ) ) ) ;
		c->ShowUI() ;
	}else{
		connect( this,SIGNAL( walletpassWordChanged( bool ) ),m_interfaceObject,SLOT( walletpassWordChanged( bool ) ) ) ;
		emit walletpassWordChanged( false ) ;
	}
}

QByteArray LxQt::Wallet::internalWallet::readValue( const QString& key )
{
	lxqt_wallet_key_values_t key_value ;
	if( lxqt_wallet_read_key_value( m_wallet,key.toLatin1().constData(),key.size() + 1,&key_value ) ){
		return QByteArray( key_value.key_value,key_value.key_value_size ) ;
	}else{
		return QByteArray() ;
	}
}

QVector<LxQt::Wallet::walletKeyValues> LxQt::Wallet::internalWallet::readAllKeyValues( void )
{
	QVector<LxQt::Wallet::walletKeyValues> w ;

	lxqt_wallet_iterator_t iter ;

	iter.iter_pos = 0 ;

	while( lxqt_wallet_iter_read_value( m_wallet,&iter ) ){
		LxQt::Wallet::walletKeyValues s( QByteArray( iter.entry.key,iter.entry.key_size - 1 ),
						 QByteArray( iter.entry.key_value,iter.entry.key_value_size ) ) ;
		w.append( s ) ;
	}
	return w ;
}

QStringList LxQt::Wallet::internalWallet::readAllKeys()
{
	QStringList l ;
	lxqt_wallet_iterator_t iter ;

	iter.iter_pos = 0 ;
	while( lxqt_wallet_iter_read_value( m_wallet,&iter ) ){
		l.append( QByteArray( iter.entry.key,iter.entry.key_size - 1 ) ) ;
	}
	return l ;
}

bool LxQt::Wallet::internalWallet::addKey( const QString& key,const QByteArray& value )
{
	/*
	 * For the key,we add +1 to the key size to include the '\0' character in the key to
	 * avoid possible collisions if our keys prefix match
	 */
	lxqt_wallet_error r = lxqt_wallet_add_key( m_wallet,key.toLatin1().constData(),key.size() + 1,value.constData(),value.size() ) ;
	return r == lxqt_wallet_no_error ;
}

void LxQt::Wallet::internalWallet::deleteKey( const QString& key )
{
	lxqt_wallet_delete_key( m_wallet,key.toLatin1().constData(),key.size() + 1 ) ;
}

int LxQt::Wallet::internalWallet::walletSize( void )
{
	return lxqt_wallet_wallet_size( m_wallet ) ;
}

void LxQt::Wallet::internalWallet::closeWallet( bool b )
{
	Q_UNUSED( b ) ;
	lxqt_wallet_close( &m_wallet ) ;
}

LxQt::Wallet::walletBackEnd LxQt::Wallet::internalWallet::backEnd()
{
	return LxQt::Wallet::internalBackEnd ;
}

bool LxQt::Wallet::internalWallet::walletIsOpened()
{
	return m_wallet != 0 ;
}

void LxQt::Wallet::internalWallet::setInterfaceObject( QWidget * interfaceObject )
{
	this->setParent( interfaceObject ) ;
	m_interfaceObject = interfaceObject ;
	connect( this,SIGNAL( walletIsOpen( bool ) ),m_interfaceObject,SLOT( walletIsOpen( bool ) ) ) ;
}

QObject * LxQt::Wallet::internalWallet::qObject()
{
	return static_cast< QObject *>( this ) ;
}

QString LxQt::Wallet::internalWallet::storagePath()
{
	return QString() ;
}

QStringList LxQt::Wallet::internalWallet::managedWalletList()
{
	char path[ 4096 ] ;
	lxqt_wallet_application_wallet_path( path,4096,m_applicationName.toLatin1().constData() ) ;
	QDir d( path ) ;
	QStringList l = d.entryList() ;
	l.removeOne( "." ) ;
	l.removeOne( ".." ) ;

	if( l.size() > 0 ){
		/*
		 * remove the extension part of a file name
		 */
		const QString& q = l.at( 0 ) ;
		l.replaceInStrings( q.mid( q.indexOf( "." ) ),"" ) ;
	}

	return l ;
}

QString LxQt::Wallet::internalWallet::localDefaultWalletName()
{
	return QString() ;
}

QString LxQt::Wallet::internalWallet::networkDefaultWalletName()
{
	return QString() ;
}

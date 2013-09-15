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

#ifndef LXQT_WALLET_INTERFACE_H
#define LXQT_WALLET_INTERFACE_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QWidget>
#include <QByteArray>
#include <QVector>
#include <QStringList>
#include <QDir>

namespace lxqt{

namespace Wallet{

class walletKeyValues{
public:
	QString key ;
	QByteArray value ;
};

typedef enum{
	internalBackEnd,
	kwalletBackEnd,
	secretServiceBackEnd
}walletBackEnd;

/*
 * forward declare the Wallet class
 */
class Wallet ;

/*
 * check if there is a support for a backend and return true if the back end is supported
 */
Q_DECL_EXPORT bool backEndIsSupported( lxqt::Wallet::walletBackEnd ) ;

/*
 * delete a wallet
 * KWallet backend does not use the applicationName argument
 */
Q_DECL_EXPORT bool deleteWallet( lxqt::Wallet::walletBackEnd,const QString& walletName,const QString& applicationName = QString() ) ;

/*
 * check if a particular wallet exists
 */
Q_DECL_EXPORT bool walletExists( lxqt::Wallet::walletBackEnd,const QString& walletName,const QString& applicationName = QString() ) ;

/*
 * get a pointer to a requested backend to be used to gain access to the API.It is advised to call
 * backEndIsSupported() to check if a backed is supported before calling this function.
 * 0 is returned if there is no support for requested backend.
 * A caller is responsible for the returned object and must delete it when done with it
 */
Q_DECL_EXPORT lxqt::Wallet::Wallet * getWalletBackend( lxqt::Wallet::walletBackEnd = lxqt::Wallet::internalBackEnd ) ;

/*
 * return a list of all wallets
 * returned value is undefined if the backend is not supported
 */
QStringList walletList( lxqt::Wallet::walletBackEnd ) ;

/*
 * Below class is the interface that implements various backends.
 * See example at the end of this header file to see an example of how to use the interface.
 */

class Q_DECL_EXPORT Wallet : public QWidget
{
public:
	Wallet() ;
	virtual ~Wallet() ;

	/*
	 * add an entry to the wallet
	 */
	virtual bool addKey( const QString& key,const QByteArray& value ) = 0 ;

	/*
	 * get a value through a key
	 */
	virtual QByteArray readValue( const QString& key ) = 0 ;

	/*
	 * get all keys and their respective values from the wallet
	 */
	virtual QVector<lxqt::Wallet::walletKeyValues> readAllKeyValues( void ) = 0 ;

	/*
	 * get all keys in the wallet
	 */
	virtual QStringList readAllKeys( void ) = 0 ;

	/*
	 * delete a key in a wallet
	 */
	virtual void deleteKey( const QString& key ) = 0 ;

	/*
	 * return the number of entries in the wallet
	 */
	virtual int walletSize( void ) = 0 ;

	/*
	 * close the a wallet
	 */
	virtual void closeWallet( bool ) = 0 ;

	/*
	 * return the backend in use
	 */
	virtual lxqt::Wallet::walletBackEnd backEnd( void ) = 0 ;

	/*
	 * check if a wallet is opened or not
	 * If the wallet is not open,secretService backend will block while the user is prompted for a key to inlock it
	 */
	virtual bool walletIsOpened( void ) = 0 ;

	/*
	 * return QObject pointer of the backend,not sure why you would want this as communication btw a backend and a user of the
	 * API is done through setInterfaceObject() method.
	 */
	virtual QObject * qObject( void ) = 0 ;

	/*
	 * Behavior of the method according to different back ends.
	 *
	 * secret service( gnome backend ):
	 *
	 * kwallet:
	 * walletName argument corresponds to the same thing in KWAllet API
	 * applicationName argument corresponds to password folder in KWallet API,default value will set passwordFolder to KDE's default.
	 * password argument is not used
	 *
	 * This back end requires an object to be passed using "setInterfaceObject()" method of this API and the object must have a slot named
	 * "void walletIsOpen(bool)".The slot will be called with "true" if the wallet was opened and with "false" otherwise.
	 *
	 * Calling this open() method will generate a KWallet GUI prompt for a password.
	 *
	 * internal:
	 * walletName argument is the name of the wallet to open.
	 * applicationName argument is the name of the program that owns the wallet.
	 *
	 * If password argument is given,it will be used to attempt to open the wallet
	 * If password argument is not given,a GUI window will be generated to ask the user for the password.
	 *
	 * This back end requires an object to be passed using "setInterfaceObject()" method of this API and the object must have a slot named
	 * "void walletIsOpen(bool)".The slot will be called with "true" if the wallet was opened and with "false" otherwise.
	 *
	 * Calling this open() method without a password will generate a GUI prompt for a password
	 */
	virtual void open( const QString& walletName,const QString& applicationName = QString(),const QString& password = QString() ) = 0 ;

	/*
	 * This method is used as a mean of communication between the backend and the user of the library.see open() method documentation above
	 * for a use case of this API
	 */
	virtual void setInterfaceObject( QWidget * ) = 0 ;

	/*
	 * this method returns PasswordFolder() in kwallet backend and is undefined in other backends
	 */
	virtual QString storagePath( void ) = 0 ;

	/*
	 * change the wallet key to newWalletKey
	 * internal backend will emit "walletpassWordChanged(bool)" to notify if the password was changed or not
	 * This method is undefined in secretService backend
	 */
	virtual void changeWalletPassWord( const QString& walletName,const QString& applicationName = QString() ) = 0 ;

	/*
	 * list all wallets managed by an application.
	 * This method is equivalent to kwallet's "folderList()"
	 * This method is undefined in secretService backend
	 */
	virtual QStringList managedWalletList( void ) = 0 ;
};

} // namespace lxqt

} // namespace wallet

#endif // LXQT_WALLET_INTERFACE_H

/*
 * An example use of the API that opens a wallet and then prints all keys and their respective values
 */

#if 0
void TestClass::walletIsOpen( bool walletIsOpen )
{
	if( walletIsOpen ){
		QVector<lxqt::Wallet::walletKeyValues> s = m_wallet->readAllKeyValues() ;
		size_t j = s.size() ;
		for( size_t i = 0 ; i < j ; i++ ){
			qDebug() << "key=" << s.at( i ).key << ":value=" << s.at( i ).value ;
		}
		m_wallet->closeWallet( false ) ;
	}else{
		qDebug() << "failed to open wallet" ;
	}

	/*
	 * Delete the wallet object when done with it
	 */
	m_wallet->deleteLater() ;
}

void TestClass::testWallet()
{
	/*
	 * open a default backend( internal one )
	 */
	m_wallet = lxqt::Wallet::getWalletBackend() ;

	/*
	 * set the QObject with "walletIsOpen(bool)" slot
	 */
	m_wallet->setInterfaceObject( this ) ;

	/*
	 * open a wallet.
	 * This method will return the result of the attempt though a slot on an object set above.
	 */
	m_wallet->open( "test_wallet_name","test_application_name" ) ;
}

#endif


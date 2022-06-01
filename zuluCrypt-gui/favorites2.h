#ifndef FAVORITES2_H
#define FAVORITES2_H
/*
 *
 *  Copyright (c) 2019
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

#include <QDialog>
#include <QTableWidget>
#include <QCloseEvent>

#include "utility.h"
#include "favorites.h"
#include "secrets.h"
#include "utility2.h"

#include <functional>
#include <memory>

namespace Ui {
class favorites2;
}

class favorites2 : public QDialog
{
	Q_OBJECT
public:
	class settings{
	public:
		class walletBackEnd
		{
		public:
			walletBackEnd( LXQt::Wallet::BackEnd s ) : m_valid( true ),m_storage( s )
			{
			}
			walletBackEnd() : m_valid( false )
			{
			}
			bool operator==( LXQt::Wallet::BackEnd s ) const
			{
				return m_valid && m_storage == s ;
			}
			bool operator==( const settings::walletBackEnd& other ) const
			{
				if( this->m_valid && other.m_valid ){

					return this->m_storage == other.m_storage ;
				}else{
					return false ;
				}
			}
			bool isValid() const
			{
				return m_valid ;
			}
			bool isInvalid() const
			{
				return !this->isValid() ;
			}
			LXQt::Wallet::BackEnd bk() const
			{
				return m_storage ;
			}
		private:
			bool m_valid ;
			LXQt::Wallet::BackEnd m_storage ;
		};

		settings() : m_settings( utility::settingsObject() )
		{
		}
		settings( QSettings& s ) : m_settings( s )
		{
		}
		bool autoMountFavoritesOnStartUp()
		{
			if( m_settings.contains( "AutoMountFavoritesOnStartUp" ) ){

				return m_settings.value( "AutoMountFavoritesOnStartUp" ).toBool() ;
			}else{
				bool s = false ;
				settings::autoMountFavoritesOnStartUp( s ) ;
				return s ;
			}
		}
		void autoMountFavoritesOnStartUp( bool e )
		{
			m_settings.setValue( "AutoMountFavoritesOnStartUp",e ) ;
		}
		void autoMountBackEnd( const settings::walletBackEnd& e )
		{
			m_settings.setValue( "AutoMountPassWordBackEnd",[ & ]()->QString{

				if( e.isInvalid() ){

					return "none" ;

				}else if( e == LXQt::Wallet::BackEnd::internal ){

					return "internal" ;

				}else if( e == LXQt::Wallet::BackEnd::libsecret ){

					return "libsecret" ;

				}else if( e == LXQt::Wallet::BackEnd::kwallet ){

					return "kwallet" ;

				}else if( e == LXQt::Wallet::BackEnd::osxkeychain ){

					return "osxkeychain" ;

				}else if( e == LXQt::Wallet::BackEnd::windows_dpapi ){

					return "windows_DPAPI" ;
				}else{
					return "none" ;
				}
			}() ) ;
		}
		settings::walletBackEnd autoMountBackEnd()
		{
			if( m_settings.contains( "AutoMountPassWordBackEnd" ) ){

				auto e = m_settings.value( "AutoMountPassWordBackEnd" ).toString() ;

				if( e == "libsecret" ){

					return LXQt::Wallet::BackEnd::libsecret ;

				}else if( e == "kwallet" ){

					return LXQt::Wallet::BackEnd::kwallet ;

				}else if( e == "internal" ){

					return LXQt::Wallet::BackEnd::internal ;

				}else if( e == "osxkeychain" ){

					return LXQt::Wallet::BackEnd::osxkeychain ;

				}else if( e == "windows_DPAPI" ){

					return LXQt::Wallet::BackEnd::windows_dpapi ;
				}else{
					return settings::walletBackEnd() ;
				}
			}else{
				m_settings.setValue( "AutoMountPassWordBackEnd",QString( "none" ) ) ;
				return settings::walletBackEnd() ;
			}
		}
	private:
		QSettings& m_settings ;
	};

	static Task::future< void >& deleteKey( secrets::wallet&,const QString& id ) ;

	static Task::future< bool >& addKey( secrets::wallet&,
					     const QString& id,
					     const QString& key ) ;

	static favorites2& instance( QWidget * parent,
				     secrets& wallet,
				     std::function< void() > function )
	{
		return *( new favorites2( parent,
					  wallet,
					  std::move( function ) ) ) ;
	}
	favorites2( QWidget * parent,
		    secrets& wallet,
		    std::function< void() > function ) ;
	~favorites2() ;
private :
	void nobackendSet() ;
	void showContextMenu( QTableWidgetItem * item,bool itemClicked ) ;
	void addkeyToWallet() ;
	void deleteKeyFromWallet( const QString& ) ;
	void walletBkChanged( LXQt::Wallet::BackEnd ) ;
	void setControlsAvailability( bool,bool clearTable ) ;
	void tabChanged( int ) ;
	void updateVolumeList( const QString& ) ;
	void updateVolumeList( size_t ) ;
	void showUpdatedEntry( const favorites::entry& ) ;
	void setCommand( QLineEdit * ) ;
	void removeEntryFromFavoriteList( void ) ;
	void add( void ) ;
	void cancel( void ) ;
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	void itemClicked( QTableWidgetItem * current,bool ) ;
	void itemClicked( QTableWidgetItem * current ) ;
	void shortcutPressed( void ) ;
	void devicePathTextChange( QString ) ;
	void ShowUI() ;
	void HideUI( void ) ;
	QStringList readAllKeys() ;
	QString getExistingFile( const QString& ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::favorites2 * m_ui ;
	secrets& m_secrets ;
	QWidget * m_parentWidget ;
	QMenu m_volPathFav ;

	int m_editRow ;
	bool m_reverseMode = false ;
	bool m_volumeNeedNoPassword = false ;
	bool m_mountReadOnly = false ;
	bool m_editMode = false ;
	settings m_settings ;
	std::function< void() > m_function ;
	QString m_cipherPath ;

	class wallet{

	public:
		void operator=( secrets::wallet s )
		{
			m_w.reset( new w{ std::move( s ) } ) ;
		}
		LXQt::Wallet::Wallet * operator->()
		{
			return m_w.get()->wallet.operator->() ;
		}
		secrets::wallet& get()
		{
			return m_w.get()->wallet ;
		}
		template< typename ... Args >
		void open( Args&& ... args )
		{
			m_w->wallet.open( std::forward< Args >( args ) ... ) ;
		}
		operator bool()
		{
			auto s = m_w.get() ;

			if( s == nullptr ){

				return false ;
			}else{
				return s->wallet.operator bool() ;
			}
		}
	private:
		struct w{
			secrets::wallet wallet ;
		} ;
		std::unique_ptr< w > m_w = nullptr ;

	} m_wallet ;
} ;

#endif // FAVORITES2_H

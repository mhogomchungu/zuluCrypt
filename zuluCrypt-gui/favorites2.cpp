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

#include "favorites2.h"
#include "ui_favorites2.h"

#include "tablewidget.h"
#include "utility.h"
#include "dialogmsg.h"
#include "openvolume.h"

#include <QFileDialog>

#define COMMENT "-zuluCrypt_Comment_ID"

Task::future< void >& favorites2::deleteKey( secrets::wallet& wallet,const QString& id )
{
	return Task::run( [ &wallet,id ](){

		wallet->deleteKey( id ) ;
	} ) ;
}

Task::future< bool >& favorites2::addKey( secrets::wallet& wallet,
					  const QString& id,
					  const QString& key )
{
	return Task::run( [ &wallet,id,key ](){

		if( wallet->addKey( id,key ) ){

			return true ;
		}else{
			utility::debug() << "Failed To Add Key To Wallet" ;

			return false ;
		}
	} ) ;
}

favorites2::favorites2( QWidget * parent,
			secrets& wallet,
			std::function< void() > function ) :
	QDialog ( parent ),
	m_ui( new Ui::favorites2 ),
	m_secrets( wallet ),
	m_settings(),
	m_function( std::move( function ) )
{
	m_ui->setupUi( this ) ;

	this->installEventFilter( this ) ;

	this->setFont( parent->font() ) ;

	this->setFixedSize( this->size() ) ;

	m_ui->tableWidget->horizontalHeader()->setStretchLastSection( true ) ;
	m_ui->tableWidget->setMouseTracking( true ) ;

	m_ui->tableWidgetWallet->horizontalHeader()->setStretchLastSection( true ) ;

	m_ui->tableWidgetWallet->setMouseTracking( true ) ;

	m_ui->tableWidgetWallet->setContextMenuPolicy( Qt::CustomContextMenu ) ;
	m_ui->tableWidget->setContextMenuPolicy( Qt::CustomContextMenu ) ;

	m_ui->tableWidget->setColumnWidth( 0,400 ) ;

	connect( m_ui->tableWidget,&QTableWidget::customContextMenuRequested,[ this ]( QPoint s ){

		Q_UNUSED( s )

		auto item = m_ui->tableWidget->currentItem() ;

		if( item ){

			QMenu m ;

			auto ac = m.addAction( tr( "Remove" ) ) ;

			connect( ac,&QAction::triggered,[ this ](){

				this->removeEntryFromFavoriteList() ;
			} ) ;

			m.exec( QCursor::pos() ) ;
		}
	} ) ;

	connect( m_ui->tableWidgetWallet,&QTableWidget::customContextMenuRequested,[ this ]( QPoint s ){

		Q_UNUSED( s )

		auto item = m_ui->tableWidgetWallet->currentItem() ;

		if( item ){

			this->showContextMenu( item,true ) ;
		}
	} ) ;

	connect( m_ui->tableWidgetWallet,&QTableWidget::currentItemChanged,[]( QTableWidgetItem * c,QTableWidgetItem * p ){

		tablewidget::selectRow( c,p ) ;
	} ) ;

	m_ui->tableWidgetWallet->setContextMenuPolicy( Qt::CustomContextMenu ) ;

	m_ui->pbFile->setIcon( QIcon( ":/file.png" ) ) ;
	m_ui->pbPartition->setIcon( QIcon( ":/partition.png" ) ) ;
	m_ui->pbFolder->setIcon( QIcon( ":/folder.png" ) ) ;

	connect( m_ui->pbVolumePathFromFileSystem,&QPushButton::clicked,[ this ](){

		auto e = this->getExistingFile( tr( "Path To An Encrypted Volume" ) ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditVolumePath->setText( utility::pathToUUID( e ) ) ;
		}
	} ) ;

	connect( m_ui->pbFolder,&QPushButton::clicked,[ this ](){

		auto e = this->getExistingFolder( tr( "Path To An Encrypted Volume" ) ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditVolumeID->setText( e ) ;

			auto a = utility::split( e,'/' ).last() ;

			m_ui->lineEditMountPath->setText( a ) ;
		}
	} ) ;

	connect( m_ui->pbFile,&QPushButton::clicked,[ this ](){

		auto e = this->getExistingFile( tr( "Path To An Encrypted Volume" ) ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditVolumeID->setText( e ) ;

			auto a = utility::split( e,'/' ).last() ;

			m_ui->lineEditMountPath->setText( a ) ;
		}
	} ) ;

	connect( m_ui->pbAdd,&QPushButton::clicked,[ this ](){

		favorites::entry e ;

		e.volumePath = m_ui->lineEditVolumeID->text() ;
		e.mountPointPath = m_ui->lineEditMountPath->text() ;
		e.mountOptions = m_ui->lineEditMountOptions->text() ;

		if( !e.volumePath.isEmpty() && !e.mountPointPath.isEmpty() ){

			favorites::instance().add( e ) ;

			this->showUpdatedEntry( e ) ;

			m_ui->lineEditVolumeID->clear() ;
			m_ui->lineEditMountPath->clear() ;
			m_ui->lineEditMountOptions->clear() ;
		}
	} ) ;

	connect( m_ui->pbPartition,&QPushButton::clicked,[ this ](){

		openvolume::instance( this,true ).ShowAllPartitions( [ this ]( const QString& e ){

			m_ui->lineEditVolumeID->setText( e ) ;

			auto a = utility::split( m_ui->lineEditVolumeID->text(),'/' ).last() ;
			m_ui->lineEditMountPath->setText( a ) ;
		} ) ;
	} ) ;

	connect( m_ui->tabWidget,&QTabWidget::currentChanged,[ this ]( int index ){

		this->tabChanged( index ) ;
	} ) ;

	connect( m_ui->pbAddKeyToWallet,&QPushButton::clicked,[ this ](){

		this->addkeyToWallet() ;
	} ) ;

	connect( &m_volPathFav,&QMenu::triggered,[ this ]( QAction * ac ){

		m_ui->lineEditVolumePath->setText( ac->objectName() ) ;
	} ) ;

	connect( m_ui->pbVolumePathFromFavorites,&QPushButton::clicked,[ this ](){

		m_volPathFav.exec() ;
	} ) ;

	using bk = LXQt::Wallet::BackEnd ;

	m_ui->rbNone->setEnabled( true ) ;

	auto _set_supported = []( QRadioButton * rb,LXQt::Wallet::BackEnd e ){

		rb->setEnabled( LXQt::Wallet::backEndIsSupported( e ) ) ;
	} ;

	_set_supported( m_ui->rbInternalWallet,bk::internal ) ;
	_set_supported( m_ui->rbKWallet,bk::kwallet ) ;
	_set_supported( m_ui->rbLibSecret,bk::libsecret ) ;

	auto walletBk = m_settings.autoMountBackEnd() ;

	if( walletBk == bk::internal ){

		m_ui->label_22->setEnabled( true ) ;
		m_ui->pbChangeWalletPassword->setEnabled( [](){

			auto a = utility::walletName() ;
			auto b = utility::applicationName() ;
			return LXQt::Wallet::walletExists( LXQt::Wallet::BackEnd::internal,a,b ) ;
		}() ) ;

		m_ui->label_22->setText( tr( "Change Internal Wallet Password" ) ) ;

		m_ui->rbInternalWallet->setChecked( true ) ;

	}else if( walletBk == bk::libsecret ){

		m_ui->label_22->setEnabled( false ) ;
		m_ui->pbChangeWalletPassword->setEnabled( false ) ;

		m_ui->label_22->setText( tr( "Not Applicable" ) ) ;

		m_ui->rbLibSecret->setChecked( true ) ;

	}else if( walletBk == bk::kwallet ){

		m_ui->label_22->setEnabled( false ) ;
		m_ui->pbChangeWalletPassword->setEnabled( false ) ;

		m_ui->label_22->setText( tr( "Not Applicable" ) ) ;

		m_ui->rbKWallet->setChecked( true ) ;
	}else{
		m_ui->rbNone->setChecked( true ) ;
		this->nobackendSet() ;
	}

	connect( m_ui->rbInternalWallet,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_ui->label_22->setText( tr( "Change Internal Wallet Password" ) ) ;
			m_ui->label_22->setEnabled( true ) ;
			m_ui->pbChangeWalletPassword->setEnabled( true ) ;
			this->walletBkChanged( bk::internal ) ;
			m_settings.autoMountBackEnd( bk::internal ) ;

			m_ui->pbChangeWalletPassword->setEnabled( [](){

				auto a = utility::walletName() ;
				auto b = utility::applicationName() ;

				return LXQt::Wallet::walletExists( LXQt::Wallet::BackEnd::internal,a,b ) ;
			}() ) ;
		}
	} ) ;

	connect( m_ui->rbKWallet,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_ui->label_22->setText( tr( "Not Applicable" ) ) ;

			m_ui->label_22->setEnabled( false ) ;
			m_ui->pbChangeWalletPassword->setEnabled( false ) ;
			this->walletBkChanged( bk::kwallet ) ;
			m_settings.autoMountBackEnd( bk::kwallet ) ;
		}
	} ) ;

	connect( m_ui->rbLibSecret,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_ui->label_22->setText( tr( "Not Applicable" ) ) ;
			m_ui->label_22->setEnabled( false ) ;
			m_ui->pbChangeWalletPassword->setEnabled( false ) ;
			this->walletBkChanged( bk::libsecret ) ;
			m_settings.autoMountBackEnd( bk::libsecret ) ;
		}
	} ) ;

	connect( m_ui->rbNone,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			this->nobackendSet() ;
		}
	} ) ;

	connect( m_ui->pbClose,&QPushButton::clicked,[ this ](){

		this->HideUI() ;
	} ) ;

	connect( m_ui->pbClose_2,&QPushButton::clicked,[ this ](){

		this->HideUI() ;
	} ) ;

	connect( m_ui->tableWidget,&QTableWidget::itemClicked,[]( QTableWidgetItem * item ){

		Q_UNUSED( item )
	} ) ;

	connect( m_ui->tableWidget,&QTableWidget::currentItemChanged,[]( QTableWidgetItem * x,QTableWidgetItem * y ){

		tablewidget::selectRow( x,y ) ;
	} ) ;

	m_ui->labelPassword->setVisible( false ) ;
	m_ui->pbVisiblePassword->setVisible( false ) ;

	connect( m_ui->pbVisiblePassword,&QPushButton::clicked,[ this ](){

		m_ui->labelPassword->clear() ;
		m_ui->labelPassword->setVisible( false ) ;
		m_ui->pbVisiblePassword->setVisible( false ) ;
	} ) ;

	m_ui->cbShowPassword->setChecked( true ) ;

	m_ui->lineEditPassword->setEchoMode( QLineEdit::Password ) ;

	connect( m_ui->cbShowPassword,&QCheckBox::stateChanged,[ this ]( int s ){

		if( s ){

			m_ui->lineEditPassword->setEchoMode( QLineEdit::Password ) ;
		}else{
			m_ui->lineEditPassword->setEchoMode( QLineEdit::Normal ) ;
		}
	} ) ;

	connect( m_ui->pbChangeWalletPassword,&QPushButton::clicked,[ this ](){

		auto a = utility::walletName() ;
		auto b = utility::applicationName() ;

		if( m_ui->rbInternalWallet->isChecked() ){

			this->hide() ;

			m_secrets.changeInternalWalletPassword( a,b,[ this ]( bool s ){

				if( s ){

					this->walletBkChanged( LXQt::Wallet::BackEnd::internal ) ;
				}

				this->show() ;
			} ) ;
		}
	} ) ;

	this->updateVolumeList( 0 ) ;

	this->addAction( [ this ](){

		auto ac = new QAction( this ) ;

		ac->setShortcuts( { Qt::Key_Enter,Qt::Key_Return,Qt::Key_Menu } ) ;

		connect( ac,&QAction::triggered,[ this ](){

			this->shortcutPressed() ;
		} ) ;

		return ac ;
	}() ) ;

	this->ShowUI() ;
}

favorites2::~favorites2()
{
	delete m_ui ;
}

void favorites2::nobackendSet()
{
	m_ui->label_22->setText( tr( "Not Applicable" ) ) ;
	m_ui->label_22->setEnabled( false ) ;
	m_ui->pbChangeWalletPassword->setEnabled( false ) ;
	this->setControlsAvailability( false,true ) ;
	m_settings.autoMountBackEnd( settings::walletBackEnd() ) ;
}

void favorites2::setCommand( QLineEdit * lineEdit )
{
	auto path = this->getExistingFile( tr( "Select A Command Executable" ) ) ;

	if( !path.isEmpty() ){

		lineEdit->setText( path ) ;
	}
}

void favorites2::addkeyToWallet()
{
	auto a = m_ui->lineEditVolumePath->text() ;
	auto b = m_ui->lineEditPassword->text() ;

	if( a.isEmpty() || b.isEmpty() ){

		m_ui->labelPassword->setText( tr( "Atleast One Required Field Is Empty" ) ) ;
		m_ui->labelPassword->setVisible( true ) ;
		m_ui->pbVisiblePassword->setVisible( true ) ;

		return ;
	}

	m_ui->pbAddKeyToWallet->setEnabled( false ) ;

	m_ui->tableWidgetWallet->setEnabled( false ) ;
	m_ui->pbAddKeyToWallet->setEnabled( false ) ;

	favorites2::addKey( m_wallet.get(),a,b ).then( [ this,a ]( bool s ){

		m_ui->tableWidgetWallet->setEnabled( true ) ;
		m_ui->pbAddKeyToWallet->setEnabled( true ) ;

		if( s ){

			tablewidget::addRow( m_ui->tableWidgetWallet,{ a } ) ;

			m_ui->lineEditPassword->clear() ;
			m_ui->lineEditPassword->setEchoMode( QLineEdit::Password ) ;
			m_ui->lineEditVolumePath->clear() ;
			m_ui->lineEditVolumePath->setFocus() ;
		}

		m_ui->pbAddKeyToWallet->setEnabled( true ) ;
	} ) ;
}

void favorites2::deleteKeyFromWallet( const QString& id )
{
	m_ui->tableWidgetWallet->setEnabled( false ) ;
	m_ui->pbAddKeyToWallet->setEnabled( false ) ;
	favorites2::deleteKey( m_wallet.get(),id ).await() ;
	m_ui->pbAddKeyToWallet->setEnabled( true ) ;
	m_ui->tableWidgetWallet->setEnabled( true ) ;
}

QStringList favorites2::readAllKeys()
{
	QStringList a ;

	const auto s = Task::await( [ & ](){ return m_wallet->readAllKeys() ; } ) ;

	for( const auto& it : s ){

		if( !it.endsWith( COMMENT ) ){

			a.append( it ) ;
		}
	}

	return a ;
}

void favorites2::showContextMenu( QTableWidgetItem * item,bool itemClicked )
{
	QMenu m ;

	connect( m.addAction( tr( "Delete Entry" ) ),&QAction::triggered,[ this ](){

		auto table = m_ui->tableWidgetWallet ;

		auto row = table->currentRow() ;

		if( row != -1 ){

			auto a = table->item( row,0 )->text() ;
			this->deleteKeyFromWallet( a ) ;
			tablewidget::deleteRow( table,row ) ;
		}
	} ) ;

	connect( m.addAction( tr( "Show Password" ) ),&QAction::triggered,[ this ](){

		auto table = m_ui->tableWidgetWallet ;

		auto row = table->currentRow() ;

		if( row != -1 ){

			auto a = Task::await( [ & ]()->QString{

				return m_wallet->readValue( table->item( row,0 )->text() ) ;
			} ) ;

			if( !a.isEmpty() ){

				auto e = QObject::tr( "The Password Is \"%1\"" ).arg( a ) ;

				m_ui->labelPassword->setText( e ) ;
				m_ui->labelPassword->setVisible( true ) ;
				m_ui->pbVisiblePassword->setVisible( true ) ;
			}
		}
	} ) ;

	m.addAction( tr( "Close Menu" ) ) ;

	m.setFont( this->font() ) ;

	if( itemClicked ){

		m.exec( QCursor::pos() ) ;
	}else{
		auto p = this->pos() ;

		auto x = p.x() + 100 + m_ui->tableWidgetWallet->columnWidth( 0 ) ;
		auto y = p.y() + 50 + m_ui->tableWidgetWallet->rowHeight( 0 ) * item->row() ;

		p.setX( x ) ;
		p.setY( y ) ;
		m.exec( p ) ;
	}
}

template< typename Function >
static auto _hide_ui( Function hide,LXQt::Wallet::BackEnd bk )
{
	return [ hide = std::move( hide ),bk ](){

		using w = LXQt::Wallet::BackEnd ;

		if( bk == w::internal || bk == w::windows_dpapi ){

			hide() ;
		}
	} ;
}

void favorites2::walletBkChanged( LXQt::Wallet::BackEnd bk )
{
	this->setControlsAvailability( true,true ) ;

	m_wallet = m_secrets.walletBk( bk ) ;

	m_wallet.open( [ this ](){

		for( const auto& it : this->readAllKeys() ){

			tablewidget::addRow( m_ui->tableWidgetWallet,{ it } ) ;
		}

	},_hide_ui( [ this ](){ this->hide() ; },bk ),[ this ]( bool opened ){

		if( opened ){

			for( const auto& it : this->readAllKeys() ){

				tablewidget::addRow( m_ui->tableWidgetWallet,{ it } ) ;
			}
		}else{
			m_ui->lineEditVolumePath->clear() ;

			this->setControlsAvailability( false,true ) ;
		}

		this->show() ;
	} ) ;
}

void favorites2::setControlsAvailability( bool e,bool m )
{
	if( m ){

		tablewidget::clearTable( m_ui->tableWidgetWallet ) ;
	}

	m_ui->label_21->setEnabled( e ) ;
	m_ui->label_23->setEnabled( e ) ;
	m_ui->tableWidgetWallet->setEnabled( e ) ;
	m_ui->pbAddKeyToWallet->setEnabled( e ) ;
	m_ui->lineEditVolumePath->setEnabled( e ) ;
	m_ui->lineEditPassword->setEnabled( e ) ;
	m_ui->pbVolumePathFromFavorites->setEnabled( e ) ;
	m_ui->cbShowPassword->setEnabled( e ) ;
	m_ui->pbVolumePathFromFileSystem->setEnabled( e ) ;
}

void favorites2::tabChanged( int index )
{
	if( index == 1 ){

		auto walletBk = m_settings.autoMountBackEnd() ;

		if( walletBk.isValid() ){

			this->walletBkChanged( walletBk.bk() ) ;
		}

		auto m = m_ui->pbVolumePathFromFavorites->menu() ;

		if( m ){

			m->deleteLater() ;
		}

		if( m_ui->tableWidget->rowCount() ){

			m = new QMenu( this ) ;

			for( const auto& it : tablewidget::columnEntries( m_ui->tableWidget,0 ) ){

				m->addAction( it )->setObjectName( it ) ;
			}

			connect( m,&QMenu::triggered,[ this ]( QAction * ac ){

				auto m = ac->objectName() ;

				m_ui->lineEditVolumePath->setText( utility::pathToUUID( m ) ) ;
			} ) ;

			m_ui->pbVolumePathFromFavorites->setMenu( m ) ;
		}
	}
}

template< typename M >
static void _updateList( QTableWidget * table,const QFont& font,const M& m )
{
	tablewidget::clearTable( table ) ;

	favorites::instance().entries( [ & ]( size_t index,const favorites::entry& e ){

		const auto& a = e.volumePath ;
		const auto& b = e.mountPointPath ;

		tablewidget::addRow( table,{ a,b },font ) ;
		tablewidget::setRowToolTip( table,static_cast< int >( index ),a ) ;
	} ) ;

	tablewidget::selectRow( table,m ) ;
}

void favorites2::updateVolumeList( const QString& volPath )
{
	_updateList( m_ui->tableWidget,this->font(),volPath ) ;
}

void favorites2::updateVolumeList( size_t row )
{
	_updateList( m_ui->tableWidget,this->font(),static_cast< int >( row ) ) ;
}

void favorites2::showUpdatedEntry( const favorites::entry& e )
{
	this->updateVolumeList( e.volumePath ) ;

	m_ui->tabWidget->setCurrentIndex( 0 ) ;
}

void favorites2::removeEntryFromFavoriteList()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		auto row = table->currentRow() ;

		auto& favorites = favorites::instance() ;

		const auto& aa = favorites.readFavorite( row ) ;

		if( aa.has_value() ){

			favorites::instance().removeFavoriteEntry( aa.value() ) ;

			this->updateVolumeList( 0 ) ;
		}else{
			utility::debug() << "Warning: favorites2::removeEntryFromFavoriteList() out of range" ;
		}
	}
}

void favorites2::cancel()
{
	this->HideUI() ;
}

void favorites2::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectRow( current,previous ) ;
}

void favorites2::itemClicked( QTableWidgetItem * current,bool clicked )
{
	Q_UNUSED( current )
	Q_UNUSED( clicked )
}

void favorites2::itemClicked( QTableWidgetItem * current )
{
	this->itemClicked( current,true ) ;
}

void favorites2::shortcutPressed()
{
	this->itemClicked( m_ui->tableWidget->currentItem(),false ) ;
}

void favorites2::devicePathTextChange( QString )
{
}

void favorites2::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

void favorites2::HideUI()
{
	this->hide() ;
	m_function() ;
	this->deleteLater() ;
}

QString favorites2::getExistingFolder( const QString& r )
{
	auto e = QFileDialog::getExistingDirectory( this,r,QDir::homePath() ) ;

	while( true ){

		if( e.endsWith( '/' ) ){

			e.truncate( e.length() - 1 ) ;
		}else{
			break ;
		}
	}

	return e ;
}

QString favorites2::getExistingFile( const QString& r )
{
	auto e = QFileDialog::getOpenFileName( this,r,QDir::homePath() ) ;

	while( true ){

		if( e.endsWith( '/' ) ){

			e.truncate( e.length() - 1 ) ;
		}else{
			break ;
		}
	}

	return e ;
}

void favorites2::ShowUI()
{
	m_ui->tabWidget->setCurrentIndex( 0 ) ;

	m_ui->tableWidget->setFocus() ;

	this->show() ;
	this->raise() ;
	this->activateWindow() ;
}

bool favorites2::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

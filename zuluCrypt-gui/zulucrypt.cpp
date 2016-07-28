/*
 *
 *  Copyright ( c ) 2011-2015
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

#include "zulucrypt.h"
#include "ui_zulucrypt.h"
#include "utility.h"

#include <QProcess>
#include <QStringList>
#include <QMenu>
#include <QCursor>
#include <QByteArray>
#include <QColor>
#include <QBrush>
#include <iostream>
#include <QMessageBox>
#include <QFontDialog>
#include <QMetaType>
#include <QDebug>
#include <QKeySequence>
#include <QTranslator>

#include <QTableWidgetItem>
#include <QFont>
#include <QWidget>
#include <QList>
#include <QMessageBox>
#include <QCloseEvent>
#include <QUrl>
#include <QMimeData>

#include "../zuluCrypt-cli/constants.h"
#include "../zuluMount-gui/oneinstance.h"
#include "password_dialog.h"
#include "openvolume.h"
#include "luksaddkey.h"
#include "luksdeletekey.h"
#include "createvolume.h"
#include "createfile.h"
#include "createkeyfile.h"
#include "favorites.h"
#include "cryptoinfo.h"
#include "erasedevice.h"
#include "managevolumeheader.h"
#include "cryptfiles.h"
#include "dialogmsg.h"
#include "managesystemvolumes.h"
#include "walletconfig.h"
#include "tablewidget.h"
#include "utility.h"
#include "task.h"
#include "contactinfo.h"

#include "veracrypt_support.h"
#include "pdf_path.h"

#include "checkforupdates.h"

#include <memory>

zuluCrypt::zuluCrypt( QWidget * parent ) :
	QMainWindow( parent ),
	m_mountInfo( monitor_mountinfo::instance( this,false,[ this ](){ this->quitApplication() ; } ) )
{
}

void zuluCrypt::setLocalizationLanguage( bool translate )
{
	if( translate ){

		utility::setLocalizationLanguage( translate,nullptr,"zuluCrypt-gui" ) ;
	}else{
		utility::setLocalizationLanguage( translate,m_language_menu,"zuluCrypt-gui" ) ;
	}
}

void zuluCrypt::languageMenu( QAction * ac )
{
	utility::languageMenu( this,m_language_menu,ac,"zuluCrypt-gui" ) ;

	m_ui->retranslateUi( this ) ;
}

void zuluCrypt::setUpApp( const QString& volume )
{
	this->setLocalizationLanguage( true ) ;

	m_ui = new Ui::zuluCrypt ;

	this->setupUIElements() ;
	this->setupConnections() ;
	this->initFont() ;
	this->initKeyCombo() ;
	this->initTray() ;
	this->info() ;
	this->autoUpdateCheck() ;
	this->setLocalizationLanguage( false ) ;
	this->updateVolumeList( volume ) ;

	m_mountInfo.start() ;
}

void zuluCrypt::updateVolumeList( const QString& volume )
{
	m_ui->tableWidget->setEnabled( false ) ;

	emit updateVolumeListSignal( volume,Task::await< QString >( [](){

		utility::Task::waitForOneSecond() ;

		auto r = utility::Task( QString( "%1 -L" ).arg( ZULUCRYPTzuluCrypt ) ) ;

		if( r.success() ){

			return r.output() ;
		}else{
			return QByteArray() ;
		}
	} ) ) ;
}

void zuluCrypt::updateVolumeList( QString volume,QString r )
{
	tablewidget::clearTable( m_ui->tableWidget ) ;

	if( !r.isEmpty() ){

		for( const auto& it : utility::split( r,'\n' ) ){

			auto z = utility::split( it,'\t' ) ;

			if( z.size() >= 3 ){

				const auto& q = z.at( 2 ) ;

				if( q.startsWith( "crypto_LUKS" ) ){

					z.replace( 2,"luks" ) ;
				}else{
					auto e = q ;

					e.remove( "crypto_" ) ;

					z.replace( 2,e.toLower() ) ;
				}

				tablewidget::addRow( m_ui->tableWidget,z ) ;
			}
		}
	}

	m_ui->tableWidget->setEnabled( true ) ;
	m_ui->tableWidget->setFocus() ;

	if( !volume.isEmpty() ){

		this->ShowPasswordDialog( volume,volume.split( "/" ).last() ) ;
	}
}

void zuluCrypt::initKeyCombo()
{
	this->addAction( [ this ](){

		auto ac = new QAction( this ) ;

		ac->setShortcuts( { Qt::Key_Menu,Qt::CTRL + Qt::Key_M } ) ;

		connect( ac,SIGNAL( triggered() ),this,SLOT( menuKeyPressed() ) ) ;

		return ac ;
	}() ) ;
}

void zuluCrypt::initFont()
{
	this->setUserFont( utility::getFont( this ) ) ;
}

void zuluCrypt::raiseWindow()
{
	if( m_startHidden ){

		m_trayIcon.setVisible( true ) ;
	}else{
		this->setVisible( true ) ;
		this->show() ;
		this->raise() ;
		this->setWindowState( Qt::WindowActive ) ;
	}
}

void zuluCrypt::raiseWindow( QString device )
{
	this->setVisible( true ) ;
	this->show() ;
	this->raise() ;
	this->setWindowState( Qt::WindowActive ) ;
	this->ShowPasswordDialog( device,device.split( "/" ).last() ) ;
}

void zuluCrypt::start()
{
	/*
	 * Entry point is here, the "instance" class checks if there is already a running zuluCrypt-gui process.
	 * If yes,this instance tells the existing instance to get focus and then it exits.
	 *
	 * If no,then it knowns it is the only running instance and it calls "setUpApp() to set up the GUI and
	 * runs.
	 */

	auto l         = QCoreApplication::arguments() ;

	auto e         = utility::cmdArgumentValue( l,"-d" ) ;

	m_openPath     = utility::cmdArgumentValue( l,"-m","xdg-open" ) ;
	m_startHidden  = l.contains( "-e" ) ;

	m_env          = utility::cmdArgumentValue( l,"-z","" ) ;

	utility::setUID( utility::cmdArgumentValue( l,"-K","-1" ).toInt() ) ;

	oneinstance::instance( this,"zuluCrypt-gui.socket","raiseWindow",e,[ this,e ]( QObject * instance ){

		this->setUpApp( e ) ;

		connect( instance,SIGNAL( raise() ),this,SLOT( raiseWindow() ) ) ;
		connect( instance,SIGNAL( raiseWithDevice( QString ) ),this,SLOT( raiseWindow( QString ) ) ) ;
	} ) ;
}

void zuluCrypt::initTray()
{
	utility::setIconMenu( "zuluCrypt",m_ui->actionSelect_Icons,this,[ this ]( const QString& e ){

		utility::setIcons( "zuluCrypt",e ) ;

		this->setIcons() ;
	} ) ;

	this->setIcons() ;

	utility::showTrayIcon( m_ui->actionTray_icon,&m_trayIcon ) ;
}

void zuluCrypt::trayProperty()
{
	m_ui->actionTray_icon->setEnabled( false ) ;

	utility::trayProperty( &m_trayIcon ) ;

	m_ui->actionTray_icon->setEnabled( true ) ;
}

void zuluCrypt::setupUIElements()
{
	m_ui->setupUi( this ) ;

	m_secrets.setParent( this ) ;

	m_trayIcon.setParent( this ) ;

	auto trayMenu = new QMenu( this ) ;

	trayMenu->setFont( this->font() ) ;

	trayMenu->addAction( tr( "Quit" ),this,SLOT( closeApplication() ) ) ;

	m_trayIcon.setContextMenu( trayMenu ) ;

	const auto f = utility::getWindowDimensions( "zuluCrypt" ) ;

	const auto e = f.data() ;

	this->window()->setGeometry( *( e + 0 ),*( e + 1 ),*( e + 2 ),*( e + 3 ) ) ;

	const auto table = m_ui->tableWidget ;

	table->setColumnWidth( 0,*( e + 4 ) ) ;
	table->setColumnWidth( 1,*( e + 5 ) ) ;
	table->setColumnWidth( 2,*( e + 6 ) ) ;
}

void zuluCrypt::setIcons()
{
	const auto& icon = utility::getIcon( "zuluCrypt" ) ;

	m_trayIcon.setIcon( icon ) ;

	this->setWindowIcon( icon ) ;
}

void zuluCrypt::itemEntered( QTableWidgetItem * item )
{
	auto m_point = item->tableWidget()->item( item->row(),1 )->text() ;

	if( !m_point.isEmpty() ){

		item->setToolTip( utility::shareMountPointToolTip( m_point ) ) ;
	}
}

void zuluCrypt::setupConnections()
{
	m_ui->tableWidget->setMouseTracking( true ) ;

	connect( this,SIGNAL( updateVolumeListSignal( QString,QString ) ),this,SLOT( updateVolumeList( QString,QString ) ),Qt::QueuedConnection ) ;

	connect( m_ui->tableWidget,SIGNAL( itemEntered( QTableWidgetItem * ) ),this,SLOT( itemEntered( QTableWidgetItem * ) ) ) ;
	connect( m_ui->actionErase_data_on_device,SIGNAL( triggered() ),this,SLOT( ShowEraseDataDialog() ) ) ;
	connect( m_ui->actionPartitionOpen,SIGNAL( triggered() ),this,SLOT( ShowOpenPartition() ) ) ;
	connect( m_ui->actionFileOpen,SIGNAL( triggered() ),this,SLOT( ShowPasswordDialog() ) ) ;
	connect( m_ui->actionFileCreate,SIGNAL( triggered() ),this,SLOT( ShowCreateFile() ) ) ;
	connect( m_ui->actionManage_names,SIGNAL( triggered() ),this,SLOT( ShowFavoritesEntries() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->actionCreatekeyFile,SIGNAL( triggered() ),this,SLOT( ShowCreateKeyFile() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;
	connect( m_ui->actionAbout,SIGNAL( triggered() ),this,SLOT( aboutMenuOption() ) ) ;
	connect( m_ui->actionAddKey,SIGNAL( triggered() ),this,SLOT( ShowAddKey() ) ) ;
	connect( m_ui->actionDeleteKey,SIGNAL( triggered() ),this,SLOT( ShowDeleteKey() ) ) ;
	connect( m_ui->actionPartitionCreate,SIGNAL( triggered() ),this,SLOT( ShowNonSystemPartitions() ) ) ;
	connect( m_ui->actionFonts,SIGNAL( triggered() ),this,SLOT( fonts() ) ) ;
	connect( m_ui->menuFavorites,SIGNAL( aboutToShow() ),this,SLOT( readFavorites() ) ) ;
	connect( m_ui->menuFavorites,SIGNAL( aboutToHide() ),this,SLOT( favAboutToHide() ) ) ;
	connect( m_ui->actionTray_icon,SIGNAL( triggered() ),this,SLOT( trayProperty() ) ) ;
	connect( &m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),this,SLOT( trayClicked( QSystemTrayIcon::ActivationReason ) ) ) ;
	connect( m_ui->menuFavorites,SIGNAL( triggered( QAction * ) ),this,SLOT( favClicked( QAction * ) ) ) ;
	connect( m_ui->action_close,SIGNAL( triggered() ),this,SLOT( closeApplication() ) ) ;
	connect( m_ui->action_update_volume_list,SIGNAL( triggered() ),this,SLOT( updateVolumeList() ) ) ;
	connect( m_ui->actionMinimize_to_tray,SIGNAL( triggered() ),this,SLOT( minimizeToTray() ) ) ;
	connect( m_ui->actionClose_all_opened_volumes,SIGNAL( triggered() ),this,SLOT( closeAllVolumes() ) ) ;
	connect( m_ui->actionEncrypt_file,SIGNAL( triggered() ),this,SLOT( encryptFile() ) ) ;
	connect( m_ui->actionDecrypt_file,SIGNAL( triggered() ),this,SLOT( decryptFile() ) ) ;
	connect( m_ui->actionLuks_header_backup,SIGNAL( triggered() ),this,SLOT( HelpLuksHeaderBackUp() ) ) ;
	connect( m_ui->actionManage_system_partitions,SIGNAL( triggered() ),this,SLOT( ShowManageSystemPartitions() ) ) ;
	connect( m_ui->actionManage_non_system_partitions,SIGNAL( triggered() ),this,SLOT( ShowManageNonSystemPartitions() ) ) ;
	connect( m_ui->actionChange_internal_wallet_password,SIGNAL( triggered() ),this,SLOT( changePassWordOfInternalWallet() ) ) ;
	connect( m_ui->actionManage_volumes_in_kde_wallet,SIGNAL( triggered() ),this,SLOT( manageVolumesInKDEWallet() ) ) ;
	connect( m_ui->actionManage_volumes_in_gnome_wallet,SIGNAL( triggered() ),this,SLOT( manageVolumesInGNOMEWallet() ) ) ;
	connect( m_ui->actionManage_volumes_in_internal_wallet,SIGNAL( triggered() ),this,SLOT( manageVolumesInInternalWallet() ) ) ;
	connect( m_ui->actionOpen_zuluCrypt_pdf,SIGNAL( triggered() ),this,SLOT( openpdf() ) ) ;
	connect( m_ui->actionCheck_For_Update,SIGNAL( triggered() ),this,SLOT( updateCheck() ) ) ;
	connect( m_ui->actionContact_Info,SIGNAL( triggered() ),this,SLOT( cinfo() ) ) ;

	connect( this,SIGNAL( closeVolume( QTableWidgetItem *,int ) ),this,SLOT( closeAll( QTableWidgetItem *,int ) ) ) ;

	m_autoOpenMountPoint = utility::autoOpenFolderOnMount( "zuluCrypt-gui" ) ;

	m_ui->actionAuto_Open_Mount_Point->setCheckable( true ) ;
	m_ui->actionAuto_Open_Mount_Point->setChecked( m_autoOpenMountPoint ) ;

	connect( m_ui->actionAuto_Open_Mount_Point,SIGNAL( toggled( bool ) ),this,SLOT( autoOpenMountPoint( bool ) ) ) ;

	m_ui->actionRestore_header->setText( tr( "Restore Volume Header" ) ) ;
	m_ui->actionBackup_header->setText( tr( "Backup Volume Header" ) ) ;

	connect( m_ui->actionBackup_header,SIGNAL( triggered() ),this,SLOT( volumeHeaderBackUp() ) ) ;
	connect( m_ui->actionRestore_header,SIGNAL( triggered() ),this,SLOT( volumeRestoreHeader() ) ) ;

	if( utility::runningInMixedMode() ){

		m_ui->actionManage_volumes_in_gnome_wallet->setEnabled( false ) ;
		m_ui->actionManage_volumes_in_kde_wallet->setEnabled( false ) ;
	}else{
		using wbe = LXQt::Wallet::BackEnd ;

		auto a = LXQt::Wallet::backEndIsSupported( wbe::libsecret ) ;
		auto b = LXQt::Wallet::backEndIsSupported( wbe::kwallet ) ;

		m_ui->actionManage_volumes_in_gnome_wallet->setEnabled( a ) ;
		m_ui->actionManage_volumes_in_kde_wallet->setEnabled( b ) ;
	}

	connect( m_ui->menuOptions,SIGNAL( aboutToShow() ),this,SLOT( optionMenuAboutToShow() ) ) ;

	connect( &m_mountInfo,SIGNAL( gotEvent() ),this,SLOT( updateVolumeList() ) ) ;

	m_language_menu = [ this ](){

		auto m = new QMenu( tr( "Select Language" ),this ) ;

		connect( m,SIGNAL( triggered( QAction * ) ),
			 this,SLOT( languageMenu( QAction * ) ) ) ;

		m_ui->actionSelect_Language->setMenu( m ) ;

		return m ;
	}() ;

	m_ui->actionManage_system_partitions->setEnabled( utility::userIsRoot() ) ;
	m_ui->actionManage_non_system_partitions->setEnabled( utility::userIsRoot() ) ;

	m_ui->actionVeracrypt_container_in_a_file->setEnabled( true ) ;
	m_ui->actionVeracrypt_container_in_a_partition->setEnabled( true ) ;

	this->setAcceptDrops( true ) ;
}

void zuluCrypt::autoOpenMountPoint( bool e )
{
	m_autoOpenMountPoint = e ;

	utility::autoOpenFolderOnMount( "zuluCrypt-gui",e ) ;
}

void zuluCrypt::optionMenuAboutToShow()
{
	auto a = utility::walletName() ;
	auto b = utility::applicationName() ;
	auto c = LXQt::Wallet::walletExists( LXQt::Wallet::BackEnd::internal,a,b ) ;

	m_ui->actionChange_internal_wallet_password->setEnabled( c ) ;
}

void zuluCrypt::updateCheck()
{
	checkForUpdates::instance( this ) ;
}

void zuluCrypt::cinfo()
{
	contactInfo::instance( this ) ;
}

void zuluCrypt::autoUpdateCheck()
{
	checkForUpdates::instance( this,"zuluCrypt" ) ;
}

void zuluCrypt::info()
{
	cryptoinfo::instance( this,utility::homePath() + "/.zuluCrypt/doNotshowWarning.option",QString() ) ;
}

static void _walletconfig( QWidget * widget,secrets& s,LXQt::Wallet::BackEnd e )
{
	walletconfig::instance( widget ).ShowUI( s.walletBk( e ) ) ;
}

void zuluCrypt::manageVolumesInGNOMEWallet()
{
	_walletconfig( this,m_secrets,LXQt::Wallet::BackEnd::libsecret ) ;
}

void zuluCrypt::manageVolumesInInternalWallet()
{
	_walletconfig( this,m_secrets,LXQt::Wallet::BackEnd::internal ) ;
}

void zuluCrypt::manageVolumesInKDEWallet()
{
	_walletconfig( this,m_secrets,LXQt::Wallet::BackEnd::kwallet ) ;
}

void zuluCrypt::failedToOpenWallet()
{
	//DialogMsg msg( this ) ;
	//msg.ShowUIOK( tr( "ERROR" ),tr( "could not open selected wallet" ) ) ;
}

void zuluCrypt::changePassWordOfInternalWallet()
{
	auto a = utility::walletName() ;
	auto b = utility::applicationName() ;

	m_secrets.changeInternalWalletPassword( a,b ) ;
}

void zuluCrypt::permissionExplanation()
{
	DialogMsg m( this ) ;
	m.ShowPermissionProblem( QString() ) ;
}

void zuluCrypt::ShowManageSystemPartitions()
{
	QDir d ;
	QFile f ;

	/*
	 * move config files to /etc/zuluCrypt/
	 */
	d.mkdir( "/etc/zuluCrypt" ) ;
	f.rename( "/etc/zuluCrypt-system","/etc/zuluCrypt/system_volumes.list" ) ;

	f.rename( "/etc/zuluCrypt-nonsystem","/etc/zuluCrypt/nonsystem_volumes.list" ) ;

	manageSystemVolumes::instance( this ).ShowUI( "/etc/zuluCrypt/system_volumes.list" ) ;
}

void zuluCrypt::ShowManageNonSystemPartitions()
{
	QDir d ;
	QFile f ;

	/*
	 * move config files to /etc/zuluCrypt/
	 */

	d.mkdir( "/etc/zuluCrypt" ) ;
	f.rename( "/etc/zuluCrypt-system","/etc/zuluCrypt/system_volumes.list" ) ;

	f.rename( "/etc/zuluCrypt-nonsystem","/etc/zuluCrypt/nonsystem_volumes.list" ) ;

	manageSystemVolumes::instance( this ).ShowUI( "/etc/zuluCrypt/nonsystem_volumes.list" ) ;
}

void zuluCrypt::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectRow( current,previous ) ;

	if( m_ui->tableWidget->rowCount() > 12 ){

		m_ui->tableWidget->setColumnWidth( 2,70 ) ;
	}else{
		m_ui->tableWidget->setColumnWidth( 2,95 ) ;
	}
}

void zuluCrypt::closeAllVolumes()
{
	m_ui->tableWidget->setEnabled( false ) ;

	m_mountInfo.announceEvents( false ) ;

	Task::await( [ this ](){

		utility::Task::waitForOneSecond() ; // for ui effect

		auto table = m_ui->tableWidget ;

		auto volumeCount = table->rowCount() ;

		if( volumeCount > 0 ){

			QVector< QTableWidgetItem * > tableItems( volumeCount ) ;

			auto it = tableItems.data() ;

			for( int i = 0 ; i < volumeCount ; i++ ){

				*( it + i ) = table->item( i,0 ) ;
			}

			auto exe = utility::appendUserUID( "%1 -q -d \"%2\"" ) ;

			for( int i = tableItems.count() - 1 ; i >= 0 ; i-- ){

				auto e = *( it + i ) ;

				auto device = e->text().replace( "\"","\"\"\"" ) ;

				auto r = utility::Task( exe.arg( ZULUCRYPTzuluCrypt,device ) ) ;

				emit closeVolume( e,r.exitCode() ) ;

				utility::Task::waitForOneSecond() ; ; // for ui effect
			}
		}

	} ) ;

	m_mountInfo.announceEvents( true ) ;

	m_ui->tableWidget->setEnabled( true ) ;
}

void zuluCrypt::closeAll( QTableWidgetItem * item,int st )
{
	if( st ){

		this->closeStatusErrorMessage( st ) ;
	}else{
		this->removeRowFromTable( item->row() ) ;
	}
}

void zuluCrypt::removeRowFromTable( int x )
{
	tablewidget::deleteRow( m_ui->tableWidget,x ) ;
}

void zuluCrypt::minimizeToTray()
{
	if( m_ui->actionTray_icon->isChecked() ){

		this->hide() ;
	}else{
		m_ui->actionTray_icon->setChecked( true ) ;
		this->trayProperty() ;
		this->hide() ;
	}
}

void zuluCrypt::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	if( m_trayIcon.isVisible() ){

		this->hide() ;
	}else{
		this->hide() ;

		this->closeApplication() ;
	}
}

void zuluCrypt::dragEnterEvent( QDragEnterEvent * e )
{
	e->accept() ;
}

void zuluCrypt::dropEvent( QDropEvent * e )
{
	for( const auto& it : e->mimeData()->urls() ){

		const auto& e = it.path() ;

		if( utility::pathPointsToAFile( e ) ){

			this->ShowPasswordDialog( e,e.split( "/" ).last() ) ;
		}
	}
}

void zuluCrypt::quitApplication()
{
	m_trayIcon.hide() ;
	this->hide() ;
	QCoreApplication::quit() ;
}

void zuluCrypt::closeApplication()
{
	m_mountInfo.stop()() ;
}

void zuluCrypt::trayClicked( QSystemTrayIcon::ActivationReason e )
{
	if( e == QSystemTrayIcon::Trigger ){

		if( this->isVisible() ){

			this->hide() ;
		}else{
			this->show() ;
		}
	}
}

void zuluCrypt::fonts()
{
	int size = 11 ;
	bool ok ;
	QFont Font = QFontDialog::getFont( &ok,this->font(),this ) ;

	if( ok ){

		int k = Font.pointSize() ;

		if( k > size ){

			k = size ;
			Font.setPointSize( k ) ;
			UIMessage( tr( "INFO" ),tr( "Resetting font size to %1 because larger font sizes do not fit" ).arg( QString::number( size ) ) ) ;
		}

		this->setUserFont( Font ) ;

		utility::saveFont( Font ) ;
	}
}

void zuluCrypt::setUserFont( QFont Font )
{
	this->setFont( Font ) ;

	m_ui->tableWidget->horizontalHeaderItem( 0 )->setFont( Font ) ;
	m_ui->tableWidget->horizontalHeaderItem( 1 )->setFont( Font ) ;
	m_ui->tableWidget->horizontalHeaderItem( 2 )->setFont( Font ) ;

	m_ui->actionAbout->setFont( Font ) ;
	m_ui->actionAddKey->setFont( Font ) ;
	m_ui->actionCreatekeyFile->setFont( Font ) ;
	m_ui->actionDeleteKey->setFont( Font ) ;
	m_ui->actionFavorite_volumes->setFont( Font ) ;
	m_ui->actionFileCreate->setFont( Font ) ;
	m_ui->actionFileOpen->setFont( Font ) ;
	m_ui->actionFonts->setFont( Font ) ;
	m_ui->actionInfo->setFont( Font ) ;
	m_ui->actionManage_favorites->setFont( Font ) ;
	m_ui->actionPartitionCreate->setFont( Font ) ;
	m_ui->actionPartitionOpen->setFont( Font ) ;
	m_ui->actionSelect_random_number_generator->setFont( Font ) ;
	m_ui->actionTray_icon->setFont( Font ) ;
	m_ui->menuFavorites->setFont( Font ) ;
	m_ui->actionManage_names->setFont( Font ) ;
	m_ui->actionBackup_header->setFont( Font ) ;
	m_ui->actionRestore_header->setFont( Font ) ;
	m_ui->actionEncrypt_file->setFont( Font ) ;
	m_ui->actionDecrypt_file->setFont( Font ) ;
	m_ui->menu_zc->setFont( Font ) ;
	m_ui->actionPermission_problems->setFont( Font ) ;
	m_ui->actionLuks_header_backup->setFont( Font ) ;
	m_ui->actionManage_system_partitions->setFont( Font ) ;
	m_ui->actionManage_non_system_partitions->setFont( Font ) ;
	m_ui->actionManage_volumes_in_gnome_wallet->setFont( Font ) ;
	m_ui->actionManage_volumes_in_internal_wallet->setFont( Font ) ;
	m_ui->actionManage_volumes_in_kde_wallet->setFont( Font ) ;
	m_ui->actionUse_kde_default_wallet->setFont( Font ) ;
	m_ui->actionChange_internal_wallet_password->setFont( Font ) ;
	m_ui->actionVeracrypt_container_in_a_file->setFont( Font ) ;
	m_ui->actionVeracrypt_container_in_a_partition->setFont( Font ) ;
	m_ui->actionOpen_zuluCrypt_pdf->setFont( Font ) ;
	m_ui->actionCheck_For_Update->setFont( Font ) ;
	m_ui->actionContact_Info->setFont( Font ) ;
	m_ui->actionSelect_Language->setFont( Font ) ;
	m_ui->actionAuto_Open_Mount_Point->setFont( Font ) ;
	m_ui->actionSelect_Icons->setFont( Font ) ;
}

void zuluCrypt::aboutMenuOption( void )
{
	utility::licenseInfo( this ) ;
}

void zuluCrypt::HelpLuksHeaderBackUp()
{
	QString msg = tr( "\nLUKS,TrueCrypt and VeraCrypt based encrypted volumes have what is called a \"volume header\".\n\n\
A volume header is responsible for storing information necessary to open a header using encrypted volume and any damage \
to it will makes it impossible to open the volume causing permanent loss of encrypted data.\n\n\
The damage to the header is usually caused by accidental formatting of the device or use of \
some buggy partitioning tools or wrongly reassembled logical volumes.\n\n\
Having a backup of the volume header is strongly advised because it is the only way the encrypted data will be accessible \
again after the header is restored if the header on the volume get corrupted.\n\n" ) ;

	DialogMsg m( this ) ;
	m.ShowUIInfo( tr( "Important Information On Volume Header Backup" ),false,msg ) ;
}

void zuluCrypt::volume_property()
{
	m_ui->tableWidget->setEnabled( false ) ;

	auto item = m_ui->tableWidget->currentItem() ;

	if( item == nullptr ){

		return ;
	}

	auto x = m_ui->tableWidget->item( item->row(),0 )->text() ;

	x.replace( "\"","\"\"\"" ) ;

	Task::run<QString>( [ x ](){

		auto e = utility::appendUserUID( "%1 -s -d \"%2\"" ) ;

		auto r = utility::Task( e.arg( ZULUCRYPTzuluCrypt,x ) ) ;

		if( r.success() ){

			auto data = r.output() ;

			return QString( " %1" ).arg( QString( data.mid( data.indexOf( '\n' ) + 2 ) ) ) ;
		}else{
			return QString() ;
		}

	} ).then( [ this ]( const QString& r ){

		DialogMsg msg( this ) ;

		if( r.isEmpty() ){

			msg.ShowUIOK( tr( "ERROR!" ),tr( "Volume is not open or was opened by a different user" ) ) ;
		}else{
			msg.ShowUIVolumeProperties( tr( "Volume Properties" ),r ) ;
		}

		m_ui->tableWidget->setEnabled( true ) ;
	} ) ;
}

void zuluCrypt::favAboutToHide()
{
}

void zuluCrypt::favClicked( QAction * ac )
{
	auto r = ac->text() ;

	r.remove( "&" ) ;

	auto _show_dialog = [ this ]( const QStringList& e ){

		if( e.size() > 1 ){

			const auto& first = e.first() ;

			if( !utility::pathPointsToAFolder( first ) ){

				this->ShowPasswordDialog( first,e.at( 1 ) ) ;
			}
		}
	} ;

	auto e = utility::favoriteClickedOption( r ) ;

	if( e == 1 ){

		favorites::instance( this,false ) ;

	}else if( e == 2 ){

		for( const auto& it : utility::readFavorites() ){

			_show_dialog( utility::split( it,'\t' ) ) ;
		}
	}else{
		_show_dialog( utility::split( r,'\t' ) ) ;
	}
}

void zuluCrypt::readFavorites()
{
	utility::readFavorites( m_ui->menuFavorites,false,false ) ;
}

void zuluCrypt::addToFavorite()
{
	auto item = m_ui->tableWidget->currentItem() ;
	auto x = m_ui->tableWidget->item( item->row(),0 )->text() ;
	auto y = x.split( "/" ).last() ;
	utility::addToFavorite( x,y ) ;
}

void zuluCrypt::menuKeyPressed()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		this->itemClicked( m_ui->tableWidget->currentItem(),false ) ;
	}
}

void zuluCrypt::openFolder()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		auto item = table->currentItem() ;
		this->openFolder( table->item( item->row(),1 )->text() ) ;
	}
}

void zuluCrypt::openSharedFolder()
{
	this->openFolder( m_sharedMountPoint ) ;
}

void zuluCrypt::openFolder( const QString& path )
{
	auto x = tr( "WARNING!" ) ;
	auto y = tr( "Could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_openPath ) ;

	utility::openPath( path,m_openPath,m_env,this,x,y ) ;
}

void zuluCrypt::openpdf()
{
	auto x = tr( "WARNING!" ) ;
	auto y = tr( "Failed to open zuluCrypt.pdf,make sure your system can open pdf files using \"%1\" tool and try again" ).arg( m_openPath ) ;

	QString e = PDF_PATH ;

	if( utility::pathExists( e ) ){

		utility::openPath( e,m_openPath,m_env,this,x,y ) ;
	}else{
		e += ".gz" ;

		if( utility::pathExists( e ) ){

			utility::openPath( e,m_openPath,m_env,this,x,y ) ;
		}else{
			utility::openPath( PDF_PATH,m_openPath,m_env,this,x,y ) ;
		}
	}
}

void zuluCrypt::itemClicked( QTableWidgetItem * it )
{
	this->itemClicked( it,true ) ;
}

void zuluCrypt::itemClicked( QTableWidgetItem * item,bool clicked )
{
	QMenu m ;
	m.setFont( this->font() ) ;
	connect( m.addAction( tr( "Close" ) ),SIGNAL( triggered() ),this,SLOT( close() ) ) ;

	m.addSeparator() ;

	connect( m.addAction( tr( "Properties" ) ) ,SIGNAL( triggered() ),this,SLOT( volume_property() ) ) ;

	m.addSeparator() ;

	auto m_point = item->tableWidget()->item( item->row(),1 )->text() ;

	m_sharedMountPoint = utility::sharedMountPointPath( m_point ) ;

	if( m_sharedMountPoint.isEmpty() ){

		connect( m.addAction( tr( "Open Folder" ) ) ,SIGNAL( triggered() ),this,SLOT( openFolder() ) ) ;
	}else{
		connect( m.addAction( tr( "Open Private Folder" ) ),SIGNAL( triggered() ),
			 this,SLOT( openFolder() ) ) ;
		connect( m.addAction( tr( "Open Shared Folder" ) ),SIGNAL( triggered() ),
			 this,SLOT( openSharedFolder() ) ) ;
	}

	m.addSeparator() ;

	if( m_ui->tableWidget->item( item->row(),2 )->text() == "luks" ){

		m.addSeparator() ;

		connect( m.addAction( tr( "Add Key" ) ),SIGNAL( triggered() ),this,SLOT( luksAddKeyContextMenu() ) ) ;
		connect( m.addAction( tr( "Remove Key" ) ),SIGNAL( triggered() ),this,SLOT( luksDeleteKeyContextMenu() ) ) ;

		m.addSeparator() ;

		connect( m.addAction( tr( "Backup LUKS Header" ) ),SIGNAL( triggered() ),this,SLOT( luksHeaderBackUpContextMenu() ) ) ;
	}

	m.addSeparator() ;

	auto volume_id = m_ui->tableWidget->item( item->row(),0 )->text() + "\t" ;

	QFile f( utility::homePath() + "/.zuluCrypt/favorites" ) ;

	decltype( f.readAll() ) data ;

	if( f.open( QIODevice::ReadOnly ) ){

		data = f.readAll() ;
	}

	auto ac = m.addAction( tr( "Add To Favorite" ) ) ;

	if( data.contains( volume_id.toLatin1() ) ){

		ac->setEnabled( false ) ;
	}else{
		ac->setEnabled( true ) ;
		ac->connect( ac,SIGNAL( triggered() ),this,SLOT( addToFavorite() ) ) ;
	}
	if( clicked ){

		m.exec( QCursor::pos() ) ;
	}else{
		int x = m_ui->tableWidget->columnWidth( 0 ) ;
		int y = m_ui->tableWidget->rowHeight( item->row() ) * item->row() + 20 ;

		m.addSeparator() ;
		m.addAction( tr( "Cancel" ) ) ;
		m.exec( m_ui->tableWidget->mapToGlobal( QPoint( x,y ) ) ) ;
	}
}

void zuluCrypt::setDefaultWallet()
{
	m_ui->actionUse_kde_default_wallet->setEnabled( false ) ;
	m_ui->actionUse_kde_default_wallet->setEnabled( true ) ;
}

void zuluCrypt::luksAddKeyContextMenu( void )
{
	auto item = m_ui->tableWidget->currentItem() ;

	this->ShowAddKeyContextMenu( m_ui->tableWidget->item( item->row(),0 )->text() ) ;
}

void zuluCrypt::luksDeleteKeyContextMenu( void )
{
	auto item = m_ui->tableWidget->currentItem() ;

	this->ShowDeleteKeyContextMenu( m_ui->tableWidget->item( item->row(),0 )->text() ) ;
}

void zuluCrypt::UIMessage( QString title,QString message )
{
	DialogMsg msg( this ) ;
	msg.ShowUIOK( title,message ) ;
}

void zuluCrypt::closeStatusErrorMessage( int st )
{
	switch ( st ) {

		case 0 :break ;
		case 1 :UIMessage( tr( "ERROR!" ),tr( "Close failed, volume is not open or was opened by a different user" ) ) ;			break ;
		case 2 :UIMessage( tr( "ERROR!" ),tr( "Close failed, one or more files in the volume are in use." ) ) ;					break ;
		case 3 :UIMessage( tr( "ERROR!" ),tr( "Close failed, volume does not have an entry in /etc/mtab" ) ) ;					break ;
		case 4 :UIMessage( tr( "ERROR!" ),tr( "Close failed, could not get a lock on /etc/mtab~" ) ) ;						break ;
		case 5 :UIMessage( tr( "ERROR!" ),tr( "Close failed, volume is unmounted but could not close mapper,advice to close it manually" ) ) ;	break ;
		case 6 :UIMessage( tr( "ERROR!" ),tr( "Close failed, could not resolve full path of device\n" ) ) ;               			break ;
		case 7 :UIMessage( tr( "ERROR!" ),tr( "Close failed, shared mount point appear to be busy\n" ) ) ;					break ;
		case 8 :UIMessage( tr( "ERROR!" ),tr( "Close failed, shared mount point appear to belong to a different user or multiple mount points detected\n" ) ) ;			break ;
		case 9 :UIMessage( tr( "ERROR!" ),tr( "Close failed, shared mount point appear to be in an ambiguous state,advice to unmount manually" ) ) ;break ;
		case 10:UIMessage( tr( "ERROR!" ),tr( "Close failed, multiple mount points for the volume detected" ) ) ;                               break ;

		case 110:UIMessage( tr( "ERROR!" ),tr( "Close failed, could not find any partition with the presented UUID" ) ) ;			break ;
		default:UIMessage( tr( "ERROR!" ),tr( "Unrecognized error with status number %1 encountered" ).arg( st ) ) ;
	}
}

void zuluCrypt::close()
{
	m_ui->tableWidget->setEnabled( false ) ;

	auto item = m_ui->tableWidget->currentItem() ;

	auto path = m_ui->tableWidget->item( item->row(),0 )->text().replace( "\"","\"\"\"" ) ;

	auto r = Task::await<int>( [ this,item,path ](){

		auto exe = utility::appendUserUID( "%1 -q -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt,path ) ;

		//utility::Task::waitForOneSecond() ; //for UI effect

		return utility::Task( exe ).exitCode() ;
	} ) ;

	m_ui->tableWidget->setEnabled( true ) ;

	this->closeStatusErrorMessage( r ) ;
}

void zuluCrypt::volumeRestoreHeader()
{
	managevolumeheader::instance( this ).restoreHeader() ;
}

void zuluCrypt::volumeHeaderBackUp()
{
	managevolumeheader::instance( this ).backUpHeader() ;
}

void zuluCrypt::luksHeaderBackUpContextMenu()
{
	auto item = m_ui->tableWidget->currentItem() ;

	auto device = m_ui->tableWidget->item( item->row(),0 )->text() ;

	managevolumeheader::instance( this ).backUpHeader( device ) ;
}

void zuluCrypt::ShowAddKeyContextMenu( QString key )
{
	luksaddkey::instance( this ).ShowUI( key ) ;
}

void zuluCrypt::ShowAddKey()
{
	luksaddkey::instance( this ).ShowUI() ;
}

void zuluCrypt::ShowDeleteKeyContextMenu( QString key )
{
	luksdeletekey::instance( this ).ShowUI( key ) ;
}

void zuluCrypt::ShowDeleteKey()
{
	luksdeletekey::instance( this ).ShowUI() ;
}

void zuluCrypt::ShowCreateKeyFile()
{
	createkeyfile::instance( this ) ;
}

void zuluCrypt::ShowFavoritesEntries()
{
	favorites::instance( this,false ) ;
}

void zuluCrypt::ShowCreateFile()
{
	createfile::instance( this,[ this ]( const QString& file ){

		if( utility::pathExists( file ) ){

			createvolume::instance( this ).ShowFile( file ) ;
		}
	} ) ;
}

void zuluCrypt::ShowNonSystemPartitions()
{
	openvolume::instance( this ).ShowNonSystemPartitions( [ this ]( const QString& e ){

		createvolume::instance( this ).ShowPartition( e ) ;
	} ) ;
}

void zuluCrypt::ShowOpenPartition()
{
	openvolume::instance( this ).showEncryptedOnly().ShowAllPartitions( [ this ]( const QString& e ){

		this->setUpPasswordDialog().ShowUI( e ) ;
	} ) ;
}

passwordDialog& zuluCrypt::setUpPasswordDialog()
{
	return passwordDialog::instance( m_ui->tableWidget,this,m_secrets,[ this ]( const QString& path ){

		if( m_autoOpenMountPoint ){

			this->openFolder( path ) ;
		}
	 } ) ;
}

void zuluCrypt::ShowPasswordDialog()
{
	this->setUpPasswordDialog().ShowUI() ;
}

void zuluCrypt::ShowPasswordDialog( QString x,QString y )
{
	if( x.endsWith( ".zc" ) || x.endsWith( ".zC" ) ){

		this->decryptFile( x ) ;
	}else{
		this->setUpPasswordDialog().ShowUI( x,y ) ;
	}
}

void zuluCrypt::ShowEraseDataDialog()
{
	erasedevice::instance( this ).ShowUI() ;
}

void zuluCrypt::encryptFile()
{
	cryptfiles::instance( this ).encrypt() ;
}

void zuluCrypt::decryptFile( void )
{
	cryptfiles::instance( this ).decrypt() ;
}

void zuluCrypt::decryptFile( const QString& e )
{
	cryptfiles::instance( this ).decrypt( e ) ;
}

zuluCrypt::~zuluCrypt()
{
	auto q = m_ui->tableWidget ;

	const auto& r = this->window()->geometry() ;

	utility::setWindowDimensions( "zuluCrypt",{ r.x(),
						    r.y(),
						    r.width(),
						    r.height(),
						    q->columnWidth( 0 ),
						    q->columnWidth( 1 ),
						    q->columnWidth( 2 ) } ) ;

	delete m_ui ;
}

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
#include "lxqt_wallet/frontend/lxqt_wallet.h"
#include "contactinfo.h"

#include "veracrypt_support.h"
#include "pdf_path.h"

#include "checkforupdates.h"

#include <memory>

zuluCrypt::zuluCrypt( QWidget * parent ) :QMainWindow( parent ),m_trayIcon( 0 )
{
}

void zuluCrypt::setLocalizationLanguage()
{
	QTranslator * translator = new QTranslator( this ) ;

	const char * app = "zuluCrypt-gui" ;

	QByteArray r = utility::localizationLanguage( app ).toLatin1() ;

	if( r == "en_US" ){
		/*
		 *english_US language,its the default and hence dont load anything
		 */
	}else{
		translator->load( r.constData(),utility::localizationLanguagePath( app ) ) ;
		QCoreApplication::installTranslator( translator ) ;
	}
}

void zuluCrypt::setUpApp( const QString& volume )
{
	this->setLocalizationLanguage() ;
	m_ui = new Ui::zuluCrypt ;

	this->setupUIElements() ;
	this->setupConnections() ;
	this->initFont() ;
	this->initKeyCombo() ;
	this->initTray() ;
	this->updateVolumeList( volume ) ;
	this->info() ;
	this->autoUpdateCheck() ;
}

void zuluCrypt::updateVolumeList( const QString& volume )
{
	m_ui->tableWidget->setEnabled( false ) ;

	Task::run<QString>( [](){

		auto r = utility::Task( QString( "%1 -L" ).arg( ZULUCRYPTzuluCrypt ) ) ;

		if( r.success() ){
			return QString( r.output() ) ;
		}else{
			return QString() ;
		}

	} ).then( [ &,volume ]( const QString& r ){

		if( !r.isEmpty() ){

			QStringList l = utility::split( r,'\n' ) ;
			QStringList z ;

			for( const auto& it : l ){

				z = utility::split( it,'\t' ) ;

				if( z.size() >= 3 ){

					const QString& q = z.at( 2 ) ;

					if( q.startsWith( "crypto_LUKS" ) ){

						z.replace( 2,"luks" ) ;
					}else{
						QString e( q ) ;

						e.remove( "crypto_" ) ;

						z.replace( 2,e.toLower() ) ;
					}

					tablewidget::addRowToTable( m_ui->tableWidget,z ) ;
				}
			}
		}

		m_ui->tableWidget->setEnabled( true ) ;
		m_ui->tableWidget->setFocus() ;

		if( !volume.isEmpty() ){
			QString y = volume.split( "/" ).last() ;
			this->ShowPasswordDialog( volume,y ) ;
		}
	} ) ;
}

void zuluCrypt::updateVolumeListAction()
{
	QTableWidget * table = m_ui->tableWidget ;
	if( table->isEnabled() ){
		while( table->rowCount() > 0 ){
			table->removeRow( 0 ) ;
		}
		this->updateVolumeList() ;
	}
}

void zuluCrypt::initKeyCombo()
{
	QAction * rca = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Menu ) ;
	keys.append( Qt::CTRL + Qt::Key_M ) ;
	rca->setShortcuts( keys ) ;
	connect( rca,SIGNAL( triggered() ),this,SLOT( menuKeyPressed() ) ) ;
	this->addAction( rca ) ;
}

void zuluCrypt::initFont()
{
	this->setUserFont( utility::getFont( this ) ) ;
}

void zuluCrypt::raiseWindow()
{
	if( m_startHidden ){
		m_trayIcon->setVisible( true ) ;
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
	QString y = device.split( "/" ).last() ;
	this->ShowPasswordDialog( device,y ) ;
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

	QStringList l  = QCoreApplication::arguments() ;

	QString e      = utility::cmdArgumentValue( l,"-d" ) ;

	m_openPath     = utility::cmdArgumentValue( l,"-m","xdg-open" ) ;
	m_startHidden  = l.contains( "-e" ) ;

	m_env          = utility::cmdArgumentValue( l,"-z","" ) ;

	utility::setUID( utility::cmdArgumentValue( l,"-K","-1" ).toInt() ) ;

	new oneinstance( this,"zuluCrypt-gui.socket","raiseWindow",e,[ this,e ]( QObject * instance ){

		this->setUpApp( e ) ;

		connect( instance,SIGNAL( raise() ),this,SLOT( raiseWindow() ) ) ;
		connect( instance,SIGNAL( raiseWithDevice( QString ) ),this,SLOT( raiseWindow( QString ) ) ) ;
	} ) ;
}

void zuluCrypt::initTray()
{
	QString home = QDir::homePath() + "/.zuluCrypt/" ;
	QDir d( home ) ;
	if( !d.exists() ){
		d.mkdir( home ) ;
	}
	QFile f( QDir::homePath() + "/.zuluCrypt/tray" ) ;
	if( !f.exists() ){
		f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
		f.write( "1" ) ;
		f.close() ;
	}

	f.open( QIODevice::ReadOnly ) ;
	QByteArray c = f.readAll() ;
	f.close() ;

	m_ui->actionTray_icon->setCheckable( true ) ;

	if( c.at( 0 ) == '1' ){
		m_ui->actionTray_icon->setChecked( true ) ;
		m_trayIcon->show() ;
	}else{
		m_ui->actionTray_icon->setChecked( false ) ;
		m_trayIcon->hide() ;
	}
}

void zuluCrypt::setupUIElements()
{
	m_ui->setupUi( this ) ;

	this->setWindowIcon( QIcon( ":/zuluCrypt.png" ) ) ;

	m_trayIcon = new QSystemTrayIcon( this ) ;
	m_trayIcon->setIcon( QIcon( ":/zuluCrypt.png" ) ) ;

	QMenu * trayMenu = new QMenu( this ) ;

	trayMenu->addAction( tr( "Quit" ),this,SLOT( closeApplication() ) ) ;

	m_trayIcon->setContextMenu( trayMenu ) ;

	QVector<int> f = utility::getWindowDimensions( "zuluCrypt" ) ;

	int * e = f.data() ;

	this->window()->setGeometry( *( e + 0 ),*( e + 1 ),*( e + 2 ),*( e + 3 ) ) ;

	QTableWidget * table = m_ui->tableWidget ;

	table->setColumnWidth( 0,*( e + 4 ) ) ;
	table->setColumnWidth( 1,*( e + 5 ) ) ;
	table->setColumnWidth( 2,*( e + 6 ) ) ;
}

void zuluCrypt::itemEntered( QTableWidgetItem * item )
{
	int row = item->row() ;
	QTableWidget * table = item->tableWidget() ;
	QString m_point = table->item( row,1 )->text() ;
	if( !m_point.isEmpty() ){
		item->setToolTip( utility::shareMountPointToolTip( m_point ) ) ;
	}
}

void zuluCrypt::setupConnections()
{
	m_ui->tableWidget->setMouseTracking( true ) ;
	connect( m_ui->tableWidget,SIGNAL( itemEntered( QTableWidgetItem * ) ),this,SLOT( itemEntered( QTableWidgetItem * ) ) ) ;
	connect( m_ui->actionErase_data_on_device,SIGNAL( triggered() ),this,SLOT( ShowEraseDataDialog() ) ) ;
	connect( m_ui->actionPartitionOpen,SIGNAL( triggered() ),this,SLOT( ShowOpenPartition() ) ) ;
	connect( m_ui->actionFileOpen,SIGNAL( triggered() ),this,SLOT( ShowPasswordDialog() ) ) ;
	connect( m_ui->actionVeracrypt_container_in_a_file,SIGNAL( triggered() ),this,SLOT( ShowVeraPasswordDialog() ) ) ;
	connect( m_ui->actionVeracrypt_container_in_a_partition,SIGNAL( triggered() ),this,SLOT( ShowVeraOpenPartition() ) ) ;
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
	connect( m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),this,SLOT( trayClicked( QSystemTrayIcon::ActivationReason ) ) ) ;
	connect( m_ui->menuFavorites,SIGNAL( triggered( QAction * ) ),this,SLOT( favClicked( QAction * ) ) ) ;
	connect( m_ui->action_close,SIGNAL( triggered() ),this,SLOT( closeApplication() ) ) ;
	connect( m_ui->action_update_volume_list,SIGNAL( triggered() ),this,SLOT( updateVolumeListAction() ) ) ;
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

	m_ui->actionRestore_header->setText( tr( "Restore Volume Header" ) ) ;
	m_ui->actionBackup_header->setText( tr( "Backup Volume Header" ) ) ;

	connect( m_ui->actionBackup_header,SIGNAL( triggered() ),this,SLOT( volumeHeaderBackUp() ) ) ;
	connect( m_ui->actionRestore_header,SIGNAL( triggered() ),this,SLOT( volumeRestoreHeader() ) ) ;

	if( utility::runningInMixedMode() ){

		m_ui->actionManage_volumes_in_gnome_wallet->setEnabled( false ) ;
		m_ui->actionManage_volumes_in_kde_wallet->setEnabled( false ) ;
	}else{
		m_ui->actionManage_volumes_in_gnome_wallet->setEnabled( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::secretServiceBackEnd ) ) ;
		m_ui->actionManage_volumes_in_kde_wallet->setEnabled( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::kwalletBackEnd ) ) ;
	}

	connect( m_ui->menuOptions,SIGNAL( aboutToShow() ),this,SLOT( optionMenuAboutToShow() ) ) ;

	m_ui->actionManage_system_partitions->setEnabled( utility::userIsRoot() ) ;
	m_ui->actionManage_non_system_partitions->setEnabled( utility::userIsRoot() ) ;

#if VERACRYPT_SUPPORT

	m_ui->actionVeracrypt_container_in_a_file->setEnabled( true ) ;
	m_ui->actionVeracrypt_container_in_a_partition->setEnabled( true ) ;
#else
	m_ui->actionVeracrypt_container_in_a_file->setEnabled( false ) ;
	m_ui->actionVeracrypt_container_in_a_partition->setEnabled( false ) ;
#endif

	this->setAcceptDrops( true ) ;
}

void zuluCrypt::optionMenuAboutToShow()
{
	bool b = LxQt::Wallet::walletExists( LxQt::Wallet::internalBackEnd,utility::walletName(),utility::applicationName() ) ;
	m_ui->actionChange_internal_wallet_password->setEnabled( b ) ;
}

void zuluCrypt::openpdf()
{
	bool failed = Task::await< bool >( [ this ](){ return utility::Task( m_openPath + PDF_PATH ).failed() ; } ) ;

	if( failed ){

		DialogMsg msg( this ) ;

		msg.ShowUIOK( tr( "WARNING!" ),tr( "Failed to open zuluCrypt.pdf,make sure your system can open pdf files using \"%1\" tool and try again" ).arg( m_openPath ) ) ;
	}
}

void zuluCrypt::updateCheck()
{
	checkForUpdates::checkForUpdate( this ) ;
}

void zuluCrypt::cinfo()
{
	new contactInfo( this ) ;
}

void zuluCrypt::autoUpdateCheck()
{
	checkForUpdates::autoCheckForUpdate( this,"zuluCrypt" ) ;
}

void zuluCrypt::info()
{
	QString e = QDir::homePath() + "/.zuluCrypt/doNotshowWarning.option" ;

	auto info = new cryptoinfo( this,e,QString() ) ;

	info->Show() ;
}

void zuluCrypt::manageVolumesInGNOMEWallet()
{
	walletconfig::instance( this )->ShowUI( LxQt::Wallet::secretServiceBackEnd ) ;
}

void zuluCrypt::manageVolumesInInternalWallet()
{
	walletconfig::instance( this )->ShowUI( LxQt::Wallet::internalBackEnd ) ;
}

void zuluCrypt::manageVolumesInKDEWallet()
{
	walletconfig::instance( this )->ShowUI( LxQt::Wallet::kwalletBackEnd ) ;
}

void zuluCrypt::failedToOpenWallet()
{
	//DialogMsg msg( this ) ;
	//msg.ShowUIOK( tr( "ERROR" ),tr( "could not open selected wallet" ) ) ;
}

void zuluCrypt::changePassWordOfInternalWallet()
{
	m_wallet = LxQt::Wallet::getWalletBackend() ;
	m_wallet->setInterfaceObject( this ) ;
	m_wallet->changeWalletPassWord( utility::walletName(),utility::applicationName() ) ;
}

void zuluCrypt::walletIsOpen( bool b )
{
	Q_UNUSED( b ) ;
}

void zuluCrypt::walletpassWordChanged( bool b )
{
	Q_UNUSED( b ) ;
	m_wallet->deleteLater() ;
	m_wallet = nullptr ;
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

	manageSystemVolumes::instance( this )->ShowUI( "/etc/zuluCrypt/system_volumes.list" ) ;
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

	manageSystemVolumes::instance( this )->ShowUI( "/etc/zuluCrypt/nonsystem_volumes.list" ) ;
}

void zuluCrypt::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;

	int rowCount = m_ui->tableWidget->rowCount() ;

	if( rowCount > 12 ){

		m_ui->tableWidget->setColumnWidth( 2,70 ) ;
	}else{
		m_ui->tableWidget->setColumnWidth( 2,95 ) ;
	}
}

void zuluCrypt::closeAllVolumes()
{
	m_ui->tableWidget->setEnabled( false ) ;

	Task::await( [ this ](){

		utility::Task::waitForOneSecond() ; // for ui effect

		QTableWidget * table = m_ui->tableWidget ;

		int volumeCount = table->rowCount() ;

		if( volumeCount > 0 ){

			QVector< QTableWidgetItem * > tableItems( volumeCount ) ;
			QTableWidgetItem ** it = tableItems.data() ;

			for( int i = 0 ; i < volumeCount ; i++ ){
				*( it + i ) = table->item( i,0 ) ;
			}

			QString exe = utility::appendUserUID( "%1 -q -d \"%2\"" ) ;

			for( int i = tableItems.count() - 1 ; i >= 0 ; i-- ){

				QTableWidgetItem * e = *( it + i ) ;

				QString device = e->text().replace( "\"","\"\"\"" ) ;

				auto r = utility::Task( exe.arg( ZULUCRYPTzuluCrypt,device ) ) ;

				emit closeVolume( e,r.exitCode() ) ;

				utility::Task::waitForOneSecond() ; ; // for ui effect
			}
		}

	} ) ;

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

void zuluCrypt::minimizeToTray()
{
	if( m_ui->actionTray_icon->isChecked() ){
		this->hide() ;
	}else{
		m_ui->actionTray_icon->setChecked( true ) ;
		trayProperty() ;
		this->hide() ;
	}
}

void zuluCrypt::closeEvent( QCloseEvent * e )
{
	if( m_trayIcon->isVisible() ){
		this->hide() ;
		e->ignore() ;
	}else{
		this->hide() ;
		e->accept() ;
	}
}

void zuluCrypt::dragEnterEvent( QDragEnterEvent * e )
{
	e->accept() ;
}

void zuluCrypt::dropEvent( QDropEvent * e )
{
	const QMimeData * m = e->mimeData() ;
	QList<QUrl> l = m->urls() ;

	for( const auto& it : l ){

		const QString& e = it.path() ;
		if( utility::pathPointsToAFile( e ) ){

			this->ShowPasswordDialog( e,e.split( "/" ).last() ) ;
		}
	}
}

void zuluCrypt::closeApplication()
{
	m_trayIcon->hide() ;
	this->hide() ;
	QCoreApplication::quit() ;
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

void zuluCrypt::trayProperty()
{
	m_ui->actionTray_icon->setEnabled( false ) ;
	QFile f( QDir::homePath() + "/.zuluCrypt/tray" ) ;
	f.open( QIODevice::ReadOnly ) ;
	QByteArray c = f.readAll() ;
	f.close() ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	QByteArray data ;
	if( c.at( 0 ) == '1' ){
		data.append( '0' ) ;
		f.write( data ) ;
		m_trayIcon->hide() ;
	}else{
		data.append( '1' ) ;
		f.write( data ) ;
		m_trayIcon->show() ;
	}
	f.close() ;
	m_ui->actionTray_icon->setEnabled( true ) ;
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
	m.ShowUIInfo( tr( "Important Information On Volume Header Backup" ),msg ) ;
}

void zuluCrypt::removeRowFromTable( int x )
{
	tablewidget::deleteRowFromTable( m_ui->tableWidget,x ) ;
}

void zuluCrypt::volume_property()
{
	m_ui->tableWidget->setEnabled( false ) ;

	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	QString x = m_ui->tableWidget->item( item->row(),0 )->text() ;

	x.replace( "\"","\"\"\"" ) ;

	Task::run<QString>( [ x ](){

		QString e = utility::appendUserUID( "%1 -s -d \"%2\"" ) ;

		auto r = utility::Task( e.arg( ZULUCRYPTzuluCrypt,x ) ) ;

		if( r.success() ){

			QByteArray data = r.output() ;

			return QString( " %1" ).arg( QString( data.mid( data.indexOf( '\n' ) + 2 ) ) ) ;
		}else{
			return QString() ;
		}

	} ).then( [ this ]( const QString& r ){

		DialogMsg msg( this ) ;

		if( r.isEmpty() ){

			msg.ShowUIOK( tr( "ERROR!"),tr( "Volume is not open or was opened by a different user" ) ) ;
		}else{
			msg.ShowUIVolumeProperties( tr( "Volume Properties" ),r ) ;
		}

		m_ui->tableWidget->setEnabled( true ) ;
	} ) ;
}

void zuluCrypt::favAboutToHide()
{
}

void zuluCrypt::favClicked( QAction * e )
{
	auto r = e->text() ;

	r.remove( "&" ) ;

	auto l = r.split( "\t" ) ;

	this->ShowPasswordDialog( l.at( 0 ),l.at( 1 ) ) ;
}

void zuluCrypt::readFavorites()
{
	QAction * ac ;
	m_ui->menuFavorites->clear() ;
	QStringList l = utility::readFavorites() ;
	if( !l.isEmpty() ){
		l.removeLast() ;
		for( const auto& it : l ){
			ac = new QAction( it,m_ui->menuFavorites ) ;
			m_ui->menuFavorites->addAction( ac ) ;
		}
	}else{
		ac = new QAction( tr( "List Is Empty" ),m_ui->menuFavorites ) ;
		ac->setEnabled( false ) ;
		m_ui->menuFavorites->addAction( ac ) ;
	}
}

void zuluCrypt::addToFavorite()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	QString x = m_ui->tableWidget->item( item->row(),0 )->text() ;
	QString y = x.split( "/" ).last() ;
	utility::addToFavorite( x,y ) ;
}

void zuluCrypt::menuKeyPressed()
{
	itemClicked( m_ui->tableWidget->currentItem(),false ) ;
}

void zuluCrypt::openFolder()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	this->openFolder( m_ui->tableWidget->item( item->row(),1 )->text() ) ;
}

void zuluCrypt::openFolder( const QString& path )
{
	utility::openMountPoint( path,m_openPath,m_env ).then( [ this ]( bool failed ){

		if( failed ){

			DialogMsg msg( this ) ;
			msg.ShowUIOK( tr( "WARNING!" ),tr( "Could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_openPath ) ) ;
		}
	} ) ;
}

void zuluCrypt::itemClicked( QTableWidgetItem * it )
{
	itemClicked( it,true ) ;
}

void zuluCrypt::itemClicked( QTableWidgetItem * item,bool clicked )
{
	QMenu m ;
	m.setFont( this->font() ) ;
	connect( m.addAction( tr( "Close" ) ),SIGNAL( triggered() ),this,SLOT( close() ) ) ;

	m.addSeparator() ;

	connect( m.addAction( tr( "Properties" ) ) ,SIGNAL( triggered() ),this,SLOT( volume_property() ) ) ;

	m.addSeparator() ;

	connect( m.addAction( tr( "Open Folder" ) ) ,SIGNAL( triggered() ),this,SLOT( openFolder() ) ) ;

	m.addSeparator() ;

	if( m_ui->tableWidget->item( item->row(),2 )->text() == "luks" ){
		m.addSeparator() ;
		connect( m.addAction( tr( "Add Key" ) ),SIGNAL( triggered() ),this,SLOT( luksAddKeyContextMenu() ) ) ;
		connect( m.addAction( tr( "Remove Key" ) ),SIGNAL( triggered() ),this,SLOT( luksDeleteKeyContextMenu() ) ) ;
		m.addSeparator() ;
		connect( m.addAction( tr( "Backup LUKS Header" ) ),SIGNAL( triggered() ),this,SLOT( luksHeaderBackUpContextMenu() ) ) ;
	}

	m.addSeparator() ;

	QString volume_id = m_ui->tableWidget->item( item->row(),0 )->text() + "\t" ;

	QFile f( QDir::homePath() + "/.zuluCrypt/favorites" ) ;

	QByteArray data ;

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

void zuluCrypt::closeStatus( int st )
{
	m_ui->tableWidget->setEnabled( true ) ;

	if( st == 0 ){

		this->removeRowFromTable( m_ui->tableWidget->currentItem()->row() ) ;
	}else{
		this->closeStatusErrorMessage( st ) ;
	}
}

void zuluCrypt::closeStatusErrorMessage( int st )
{
	switch ( st ) {
		case 1 :UIMessage( tr( "ERROR!" ),tr( "Close failed, volume is not open or was opened by a different user" ) ) ;			break ;
		case 2 :UIMessage( tr( "ERROR!" ),tr( "Close failed, one or more files in the volume are in use." ) ) ;					break ;
		case 3 :UIMessage( tr( "ERROR!" ),tr( "Close failed, volume does not have an entry in /etc/mtab" ) ) ;					break ;
		case 4 :UIMessage( tr( "ERROR!" ),tr( "Close failed, could not get a lock on /etc/mtab~" ) ) ;						break ;
		case 5 :UIMessage( tr( "ERROR!" ),tr( "Close failed, volume is unmounted but could not close mapper,advice to close it manually" ) ) ;	break ;
		case 6 :UIMessage( tr( "ERROR!" ),tr( "Close failed, could not resolve full path of device\n" ) ) ;               			break ;
		case 7 :UIMessage( tr( "ERROR!" ),tr( "Close failed, shared mount point appear to be busy\n" ) ) ;					break ;
		case 8 :UIMessage( tr( "ERROR!" ),tr( "Close failed, shared mount point appear to belong to a different user\n" ) ) ;			break ;
		case 9 :UIMessage( tr( "ERROR!" ),tr( "Close failed, shared mount point appear to be in an ambiguous state,advice to unmount manually" )) ;break ;
		case 110:UIMessage( tr( "ERROR!" ),tr( "Close failed, could not find any partition with the presented UUID" ) ) ;			break ;
		default:UIMessage( tr( "ERROR!" ),tr( "Unrecognized error with status number %1 encountered" ).arg( st ) ) ;
	}
}

void zuluCrypt::close()
{
	m_ui->tableWidget->setEnabled( false ) ;

	this->closeStatus( Task::await<int>( [ this ](){

		auto item = m_ui->tableWidget->currentItem() ;

		auto path = m_ui->tableWidget->item( item->row(),0 )->text().replace( "\"","\"\"\"" ) ;

		auto exe = utility::appendUserUID( "%1 -q -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt,path ) ;

		utility::Task::waitForOneSecond() ; //for UI effect

		return utility::Task( exe ).exitCode() ;

	} ) ) ;

	m_ui->tableWidget->setEnabled( true ) ;
}

void zuluCrypt::volumeRestoreHeader()
{
	managevolumeheader::instance( this )->restoreHeader() ;
}

void zuluCrypt::volumeHeaderBackUp()
{
	managevolumeheader::instance( this )->backUpHeader() ;
}

void zuluCrypt::luksHeaderBackUpContextMenu()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;

	QString device = m_ui->tableWidget->item( item->row(),0 )->text() ;

	managevolumeheader::instance( this )->backUpHeader( device ) ;
}

void zuluCrypt::ShowAddKeyContextMenu( QString key )
{
	luksaddkey::instance( this )->ShowUI( key ) ;
}

void zuluCrypt::ShowAddKey()
{
	luksaddkey::instance( this )->ShowUI() ;
}

void zuluCrypt::ShowDeleteKeyContextMenu( QString key )
{
	luksdeletekey::instance( this )->ShowUI( key ) ;
}

void zuluCrypt::ShowDeleteKey()
{
	luksdeletekey::instance( this )->ShowUI() ;
}

void zuluCrypt::ShowCreateKeyFile()
{
	new createkeyfile( this ) ;
}

void zuluCrypt::ShowFavoritesEntries()
{
	new favorites( this ) ;
}

void zuluCrypt::ShowCreateFile()
{
	new createfile( this,[ this ]( const QString& file ){

		if( utility::pathExists( file ) ){

			createvolume::instance( this )->ShowFile( file ) ;
		}
	} ) ;
}

void zuluCrypt::ShowNonSystemPartitions()
{
	openvolume::instance( this )->ShowNonSystemPartitions( [ this ]( const QString& e ){

		createvolume::instance( this )->ShowPartition( e ) ;
	} ) ;
}

void zuluCrypt::ShowOpenPartition()
{
	openvolume::instance( this )->showEncryptedOnly().ShowAllPartitions( [ this ]( const QString& e ){

		this->setUpPasswordDialog()->ShowUI( e ) ; ;
	} ) ;
}

void zuluCrypt::ShowVeraPasswordDialog()
{
	this->setUpPasswordDialog()->ShowVeraUI() ;
}

void zuluCrypt::ShowVeraOpenPartition()
{
	openvolume::instance( this )->showEncryptedOnly().ShowAllPartitions( [ this ]( const QString& e ){

		this->setUpPasswordDialog()->ShowVeraUI( e ) ;
	} ) ;
}

passwordDialog * zuluCrypt::setUpPasswordDialog()
{
	return new passwordDialog( m_ui->tableWidget,this,[ this ]( const QString& path ){

		this->openFolder( path ) ;
	} ) ;
}

void zuluCrypt::ShowPasswordDialog()
{
	this->setUpPasswordDialog()->ShowUI() ;
}

void zuluCrypt::ShowPasswordDialog( QString x,QString y )
{
	if( x.endsWith( ".zc" ) || x.endsWith( ".zC" ) ){

		this->decryptFile( x ) ;
	}else{
		this->setUpPasswordDialog()->ShowUI( x,y ) ;
	}
}

void zuluCrypt::ShowEraseDataDialog()
{
	new erasedevice( this ) ;
}

void zuluCrypt::encryptFile()
{
	cryptfiles::instance( this )->encrypt() ;
}

void zuluCrypt::decryptFile( void )
{
	cryptfiles::instance( this )->decrypt() ;
}

void zuluCrypt::decryptFile( const QString& e )
{
	cryptfiles::instance( this )->decrypt( e ) ;
}

zuluCrypt::~zuluCrypt()
{
	QVector<int> e ;

	auto q = m_ui->tableWidget ;

	const QRect& r = this->window()->geometry() ;

	e << r.x()
	  << r.y()
	  << r.width()
	  << r.height()
	  << q->columnWidth( 0 )
	  << q->columnWidth( 1 )
	  << q->columnWidth( 2 ) ;

	utility::setWindowDimensions( e,"zuluCrypt" ) ;

	delete m_ui ;
	delete m_trayIcon ;
}

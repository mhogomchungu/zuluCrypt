/*
 *
 *  Copyright (c) 2012-2015
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

#include "zulumount.h"
#include "ui_zulumount.h"
#include <QDebug>

#include <QMainWindow>

#include <QTableWidgetItem>
#include <QDir>
#include <QIcon>
#include <QAction>
#include <QKeySequence>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QFont>
#include <QCursor>
#include <QAction>
#include <QFileDialog>
#include <QUrl>
#include <QTranslator>
#include <QMimeData>
#include <QFile>
#include <QShortcut>

#include <utility>
#include <initializer_list>

#include <unistd.h>
#include <sys/vfs.h>

#include "keydialog.h"
#include "../zuluCrypt-gui/dialogmsg.h"
#include "../zuluCrypt-gui/tablewidget.h"
#include "mountpartition.h"
#include "oneinstance.h"
#include "../zuluCrypt-gui/utility.h"
#include "siritask.h"
#include "zulumounttask.h"
#include "task.hpp"
#include "../zuluCrypt-gui/favorites2.h"
#include "bin_path.h"

#include <memory>

static bool _encrypted_folder( const QString& e,bool f = true )
{
	return volumeProperty::encryptedFolder( e,f ) ;
}

zuluMount::zuluMount( QWidget * parent ) :
	QWidget( parent ),
	m_secrets( this ),
	m_mountInfo( this,true,[](){ QCoreApplication::quit() ; } ),
	m_events( this,m_mountInfo.stop() ),
	m_signalHandler( this )
{
	utility::mainWindowWidget( this ) ;

	m_signalHandler.setAction( [ this ]( systemSignalHandler::signal s ){

		Q_UNUSED( s )

		this->emergencyQuitApplication() ;
	} ) ;
}

void zuluMount::helperStarted( bool start,const QString& volume )
{
	if( !start ){

		DialogMsg( this ).ShowUIOK( tr( "ERROR" ),utility::failedToStartzuluPolkit() ) ;

		return this->closeApplication() ;
	}

	this->setLocalizationLanguage( true ) ;

	m_ui = new Ui::zuluMount ;
	m_ui->setupUi( this ) ;

	m_ui->pbunlockcryptfs->setMinimumHeight( 31 ) ;
	m_ui->pbmenu->setMinimumHeight( 31 ) ;
	m_ui->pbmount->setMinimumHeight( 31 ) ;
	m_ui->pbupdate->setMinimumHeight( 31 ) ;
	m_ui->pbfavorites->setMinimumHeight( 31 ) ;

	auto f = utility::getWindowDimensions( "zuluMount" ) ;

	auto e = f.data() ;

	this->window()->setGeometry( *( e + 0 ),*( e + 1 ),*( e + 2 ),*( e + 3 ) ) ;

	auto table = m_ui->tableWidget ;

	table->setColumnWidth( 0,*( e + 4 ) ) ;
	table->setColumnWidth( 1,*( e + 5 ) ) ;
	table->setColumnWidth( 2,*( e + 6 ) ) ;
	table->setColumnWidth( 4,*( e + 7 ) ) ;
	table->setColumnWidth( 5,*( e + 8 ) ) ;

	m_ui->tableWidget->hideColumn( 3 ) ;

#if QT_VERSION < QT_VERSION_CHECK( 5,0,0 )
	m_ui->tableWidget->verticalHeader()->setResizeMode( QHeaderView::ResizeToContents ) ;
#else
	m_ui->tableWidget->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;
#endif
	m_ui->tableWidget->verticalHeader()->setMinimumSectionSize( 30 ) ;

	m_ui->tableWidget->setMouseTracking( true ) ;

	connect( m_ui->tableWidget,SIGNAL( itemEntered( QTableWidgetItem * ) ),
		 this,SLOT( itemEntered( QTableWidgetItem * ) ) ) ;

	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;

	connect( m_ui->pbmount,SIGNAL( clicked() ),
		 this,SLOT( pbMount() ) ) ;

	connect( m_ui->pbupdate,SIGNAL( clicked()),
		 this,SLOT( pbUpdate() ) ) ;

	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),
		 this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;

	connect( m_ui->pbunlockcryptfs,SIGNAL( clicked() ),
		 this,SLOT( unlockCryptFs() ) ) ;

	connect( this,SIGNAL( unlistVolume( QString ) ),
		 this,SLOT( removeVolume( QString ) ) ) ;

	m_ui->tableWidget->setContextMenuPolicy( Qt::CustomContextMenu ) ;

	connect( m_ui->tableWidget,&QTableWidget::customContextMenuRequested,[ this ]( QPoint s ){

		Q_UNUSED( s )

		auto item = m_ui->tableWidget->currentItem() ;

		if( item ){

			this->showContextMenu( item,true ) ;
		}
	} ) ;

	this->setUpShortCuts() ;

	this->setUpFont() ;

	const auto& icon = utility::getIcon( "zuluMount" ) ;

	this->setAcceptDrops( true ) ;
	this->setWindowIcon( icon ) ;

	m_trayIcon.setParent( this ) ;
	m_trayIcon.setIcon( icon ) ;

	auto trayMenu = new QMenu( this ) ;

	trayMenu->setFont( this->font() ) ;

	m_autoMount = this->autoMount() ;
	m_autoOpenFolderOnMount = this->autoOpenFolderOnMount() ;

	trayMenu->addAction( [ this ](){

		auto ac = new QAction( tr( "Do Not Minimize To Tray" ),this ) ;

		m_actionPair.append( { ac,"Do Not Minimize To Tray" } ) ;

		ac->setCheckable( true ) ;
		ac->setChecked( utility::doNotMinimizeToTray() ) ;

		connect( ac,&QAction::triggered,[ ac ](){

			auto s = !utility::doNotMinimizeToTray() ;

			ac->setChecked( s ) ;
			utility::setDoNotMinimizeToTray( s ) ;
		} ) ;

		m_autoMountAction = ac ;

		return ac ;
	}() ) ;

	trayMenu->addAction( [ this ](){

		auto ac = new QAction( tr( "Automount Volumes" ),this ) ;

		m_actionPair.append( { ac,"Automount Volumes" } ) ;

		ac->setCheckable( true ) ;
		ac->setChecked( m_autoMount ) ;

		connect( ac,SIGNAL( toggled( bool ) ),this,SLOT( autoMountToggled( bool ) ) ) ;

		m_autoMountAction = ac ;

		return ac ;
	}() ) ;

	trayMenu->addAction( [ this ](){

		auto ac = new QAction( tr( "Auto Open Mount Point" ),this ) ;

		m_actionPair.append( { ac,"Auto Open Mount Point" } ) ;

		ac->setCheckable( true ) ;
		ac->setChecked( m_autoOpenFolderOnMount ) ;

		connect( ac,SIGNAL( toggled( bool ) ),this,SLOT( autoOpenFolderOnMount( bool ) ) ) ;

		return ac ;
	}() ) ;

	trayMenu->addAction( [ this ](){

		auto ac = new QAction( tr( "Clear Dead Mount Points" ),this ) ;

		m_actionPair.append( { ac,"Clear Dead Mount Points" } ) ;

		connect( ac,&QAction::triggered,[ this ](){

			this->disableAll() ;

			utility::Task::run( zuluMountPath" --clear-dead-mount-points" ).await() ;

			this->enableAll() ;
		} ) ;

		return ac ;
	}() ) ;

	trayMenu->addAction( [ this ](){

		auto ac = new QAction( tr( "Unmount All" ),this ) ;

		m_actionPair.append( { ac,"Unmount All" } ) ;

		connect( ac,SIGNAL( triggered() ),this,SLOT( unMountAll() ) ) ;

		return ac ;
	}() ) ;

	m_favorite_menu = [ this ](){

		auto m = new QMenu( tr( "Favorites" ),this ) ;

		m_menuPair.append( { m,"Favorites" } ) ;

		m->setFont( this->font() ) ;

		connect( m,SIGNAL( triggered( QAction * ) ),
			 this,SLOT( favoriteClicked( QAction * ) ) ) ;

		connect( m,SIGNAL( aboutToShow() ),
			 this,SLOT( showFavorites() ) ) ;

		m_ui->pbfavorites->setMenu( m ) ;

		return m ;
	}() ;

	m_not_hidden_volume_menu = [ this,trayMenu ](){

		auto m = trayMenu->addMenu( tr( "Hide Volume From View" ) ) ;

		m_menuPair.append( { m,"Hide Volume From View" } ) ;

		m->setFont( this->font() ) ;

		connect( m,SIGNAL( triggered( QAction * ) ),
			 this,SLOT( removeVolumeFromVisibleVolumeList( QAction * ) ) ) ;

		connect( m,SIGNAL( aboutToShow() ),
			 this,SLOT( showVisibleVolumeList() ) ) ;

		return m ;
	}() ;

	m_hidden_volume_menu = [ this,trayMenu ](){

		auto m = trayMenu->addMenu( tr( "Unhide Volume From View" ) ) ;

		m_menuPair.append( { m,"Unhide Volume From View" } ) ;

		m->setFont( this->font() ) ;

		connect( m,SIGNAL( triggered( QAction * ) ),
			 this,SLOT( removeVolumeFromHiddenVolumeList( QAction * ) ) ) ;

		connect( m,SIGNAL( aboutToShow() ),
			 this,SLOT( showHiddenVolumeList() ) ) ;

		return m ;
	}() ;

	m_language_menu = [ this,trayMenu ](){

		auto m = trayMenu->addMenu( tr( "Select Language" ) ) ;

		m_menuPair.append( { m,"Select Language" } ) ;

		connect( m,SIGNAL( triggered( QAction * ) ),this,SLOT( languageMenu( QAction * ) ) ) ;

		return m ;
	}() ;

	trayMenu->addMenu( [ this ](){

		auto ac = new QAction( tr( "Select Icons" ),this ) ;

		m_actionPair.append( { ac,"Select Icons" } ) ;

		utility::setIconMenu( "zuluMount",ac,this,[ this ]( const QString& e ){

			utility::setIcons( "zuluMount",e ) ;

			this->setIcons() ;
		} ) ;

		this->setIcons() ;

		return ac->menu() ;
	}() ) ;

	trayMenu->addAction( [ this ](){

		auto ac = new QAction( tr( "About" ),this ) ;

		m_actionPair.append( { ac,"About" } ) ;

		connect( ac,SIGNAL( triggered() ),this,SLOT( licenseInfo() ) ) ;

		return ac ;
	}() ) ;

	auto _addQuitAction = [ this ](){

		auto ac = new QAction( tr( "Quit" ),this ) ;

		m_actionPair.append( { ac,"Quit" } ) ;

		connect( ac,SIGNAL( triggered() ),this,SLOT( closeApplication() ) ) ;

		return ac ;
	} ;

	trayMenu->addAction( _addQuitAction() ) ;

	connect( &m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		 this,SLOT( slotTrayClicked( QSystemTrayIcon::ActivationReason ) ) ) ;

	m_trayIcon.setContextMenu( [ this,&_addQuitAction ](){

		auto m = new QMenu( this ) ;

		m->addAction( [ this ](){

			auto ac = new QAction( tr( "Show/Hide" ),this ) ;

			m_actionPair.append( { ac,"Show/Hide" } ) ;

			connect( ac,SIGNAL( triggered() ),this,SLOT( showTrayGUI() ) ) ;

			return ac ;
		}() ) ;

		m->addAction( _addQuitAction() ) ;

		return m ;
	}() ) ;

	m_ui->pbmenu->setMenu( trayMenu ) ;

	this->setLocalizationLanguage( false ) ;

	utility::showTrayIcon( nullptr,this,true ) ;

	m_powerOff = utility::powerOffCommand() ;

	this->disableAll() ;

	this->startAutoMonitor() ;

	this->updateVolumeList( zuluMountTask::updateVolumeList().await() ) ;

	if( volume.isEmpty() ) {

		this->enableAll() ;
	}else{
		this->showMoungDialog( volume ) ;
	}

	if( !m_startHidden ){

		this->raiseWindow() ;
	}

	auto debugWindowShortCut = new QShortcut( this ) ;
	debugWindowShortCut->setKey(Qt::CTRL | Qt::SHIFT | Qt::Key_D ) ;
	connect( debugWindowShortCut,&QShortcut::activated,[ this ](){

		m_debugWindow.Show() ;
	} ) ;
}

void zuluMount::showTrayIcon( bool e )
{
	Q_UNUSED( e )
	m_trayIcon.show() ;
}

void zuluMount::showTrayGUI()
{
	this->slotTrayClicked( QSystemTrayIcon::Trigger ) ;
}

void zuluMount::setIcons()
{
	const auto& icon = utility::getIcon( "zuluMount" ) ;

	m_trayIcon.setIcon( icon ) ;

	this->setWindowIcon( icon ) ;
}

void zuluMount::licenseInfo()
{
	utility::licenseInfo( this ) ;
}

void zuluMount::removeVolumeFromHiddenVolumeList( QAction * ac )
{
	this->disableAll() ;

	auto e = ac->text() ;
	e.remove( "&" ) ;

	zuluMountTask::removeVolumeFromHiddenVolumeList( e ) ;

	this->updateList( zuluMountTask::getVolumeProperties( e ).await() ) ;

	this->enableAll() ;
}

static void _manage_volume_list( QMenu * menu,const QStringList& l )
{
	menu->clear() ;

	if( l.isEmpty() ){

		menu->addAction( [ &menu ](){

			auto ac = new QAction( QObject::tr( "List Is Empty" ),menu ) ;

			ac->setEnabled( false ) ;

			return ac ;
		}() ) ;
	}else{
		for( const auto& it : l ){

			menu->addAction( new QAction( it,menu ) ) ;
		}
	}
}

void zuluMount::showHiddenVolumeList()
{
	_manage_volume_list( m_hidden_volume_menu,zuluMountTask::hiddenVolumeList() ) ;
}

void zuluMount::showVisibleVolumeList()
{
	_manage_volume_list( m_not_hidden_volume_menu,
			     tablewidget::columnEntries( m_ui->tableWidget ) ) ;
}

void zuluMount::removeVolumeFromVisibleVolumeList( QAction * ac )
{
	auto e = ac->text().remove( "&" ) ;

	auto table = m_ui->tableWidget ;

	zuluMountTask::addVolumeToHiddenVolumeList( e ) ;

	this->disableAll() ;

	tablewidget::selectRow( table,e ) ;
	utility::Task::suspendForOneSecond() ;
	tablewidget::deleteRow( table,e ) ;

	tablewidget::selectLastRow( table ) ;

	this->enableAll() ;
}

QString zuluMount::resolveFavoriteMountPoint( const QString& m )
{
	QString s ;

	favorites::instance().entries( [ & ]( const favorites::entry& e ){

		if( e.volumePath == m ){

			s = e.mountPointPath ;

			return true ;
		}

		return false ;
	} ) ;

	return s ;
}

void zuluMount::favoriteClicked( QAction * ac )
{
	auto e = ac->text() ;
	e.remove( "&" ) ;

	auto r = utility::favoriteClickedOption( e ) ;

	if( r == 1 ){

		favorites2::instance( this,m_secrets,[](){} ) ;

	}else if( r == 2 ){

		favorites::instance().entries( [ & ]( const favorites::entry& e ){

			this->showMoungDialog( e.volumePath,e.mountPointPath ) ;
		} ) ;
	}else{
		this->showMoungDialog( e,this->resolveFavoriteMountPoint( e ) ) ;
	}
}

void zuluMount::showFavorites()
{
	utility::readFavorites( m_favorite_menu,true ) ;
}

void zuluMount::setLocalizationLanguage( bool translate )
{
	utility::setLocalizationLanguage( translate,m_language_menu,"zuluMount-gui" ) ;
}

void zuluMount::languageMenu( QAction * ac )
{
	utility::languageMenu( this,m_language_menu,ac,"zuluMount-gui" ) ;

	m_ui->retranslateUi( this ) ;

	for( auto& it : m_actionPair ){

		it.first->setText( tr( it.second ) ) ;
	}

	for( auto& it : m_menuPair ){

		it.first->setTitle( tr( it.second ) ) ;
	}
}

void zuluMount::autoOpenFolderOnMount( bool e )
{
	m_autoOpenFolderOnMount = e ;

	utility::autoOpenFolderOnMount( "zuluMount-gui",e ) ;
}

bool zuluMount::autoOpenFolderOnMount( void )
{
	return utility::autoOpenFolderOnMount( "zuluMount-gui" ) ;
}

void zuluMount::startAutoMonitor()
{
	m_mountInfo.start() ;
	m_events.start() ;
}

/*
 * Below two functions should be the only functions that closes the application
 */
void zuluMount::closeApplication()
{
	m_secrets.close() ;
	utility::quitHelper() ;
	m_events.stop() ;
}

void zuluMount::closeApplication( int s )
{
	m_secrets.close() ;
	Q_UNUSED( s )
	m_events.stop() ;
}

void zuluMount::autoMountVolume( volumeProperty * q )
{
	std::unique_ptr< volumeProperty > r( q ) ;

	if( r && r->isValid() ){

		auto& p = *r ;

		if( p.encryptedVolume() ){

			this->addEntryToTable( true,p ) ;
		}else{
			if( m_autoMount ){

				this->disableAll() ;

				mountPartition::instance( this,m_ui->tableWidget,[](){},[ this ]( const QString& e ){

					this->openMountPointPath( e ) ;

				} ).AutoMount( p.setMountPoint( this->resolveFavoriteMountPoint( p.volumeName() ) ) ) ;

			}else{
				this->addEntryToTable( false,p ) ;
			}
		}
	}
}

void zuluMount::volumeRemoved( QString volume )
{
	if( !volume.isEmpty() ){

		auto table = m_ui->tableWidget ;

		auto row = tablewidget::columnHasEntry( table,volume ) ;

		if( row != -1 ){

			tablewidget::deleteRow( table,row ) ;
			/*
			* see if a user just removed the device without properly closing it/unmounting it
			* and try to do so for them
			*/

			Task::exec( [ volume ](){ zuluMountTask::checkUnMount( volume ) ; } ) ;

			this->enableAll() ;
		}
	}
}

void zuluMount::removeVolume( QString volume )
{
	if( volume.isEmpty() ){

		tablewidget::selectLastRow( m_ui->tableWidget ) ;
		this->enableAll() ;
	}else{
		tablewidget::deleteRow( m_ui->tableWidget,volume ) ;
	}
}

void zuluMount::itemEntered( QTableWidgetItem * item )
{
	auto row = item->row() ;
	auto table = item->tableWidget() ;
	auto m_point = table->item( row,1 )->text() ;

	using string_t = decltype( table->item( row,3 )->text() ) ;

	string_t x = table->item( row,3 )->text() ;
	string_t z ;
	string_t y ;

	if( m_point == "/" ){
		/*
		 * we dont check if root path is publicly shared because the path it will produce (/run/media/public/)
		 * will always return true,a solution is to examine /proc/self/mountinfo and thats work for another day
		 */
		if( x == "Nil" ){
			x.clear() ;
		}
		z += tr( "LABEL=\"%1\"" ).arg( x ) ;

	}else if( m_point == "Nil" ){

		/*
		 * volume is not mounted,cant know its LABEL value
		 */

		x.clear() ;

		z += tr( "LABEL=\"%1\"" ).arg( x ) ;
	}else{
		if( x == "Nil" ){
			x.clear() ;
		}

		y = utility::shareMountPointToolTip( m_point ) ;

		if( y.isEmpty() ){

			z += tr( "LABEL=\"%1\"" ).arg( x ) ;
		}else{
			z += tr( "LABEL=\"%1\"\n%2" ).arg( x,y ) ;
		}
	}
	item->setToolTip( z ) ;
}

void zuluMount::raiseWindow( const QString& volume )
{
	this->setVisible( true ) ;
	this->raise() ;
	this->show() ;
	this->setWindowState( Qt::WindowActive ) ;

	if( !volume.isEmpty() ){

		this->showMoungDialog( volume ) ;
	}
}

void zuluMount::polkitFailedWarning()
{
	DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "zuluMount Failed To Connect To zuluPolkit.\nPlease Report This Serious Bug." ) ) ;
}

void zuluMount::Show()
{
	auto l = QCoreApplication::arguments() ;

	m_startHidden  = l.contains( "-e" ) ;
	m_folderOpener = utility::cmdArgumentValue( l,"-m",utility::fileManager() ) ;

	auto s = utility::socketPath() ;

	if( utility::configDirectoriesAreNotWritable( this ) ){

		return this->closeApplication() ;
	}

	utility::polkitFailedWarning( [ this ](){

		QMetaObject::invokeMethod( this,"polkitFailedWarning",Qt::QueuedConnection ) ;
	} ) ;

	if( utility::libCryptSetupLibraryNotFound() ){

		auto a = tr( "Cryptsetup library could not be found and zuluCrypt will most likely not work as expected." ) ;
		auto b = tr( "\n\nPlease recompile zuluCrypt to force it to re-discover the new library" ) ;

		DialogMsg( this ).ShowUIOK( tr( "ERROR" ),a + b ) ;
	}

	oneinstance::instance( this,
			       s + "/zuluMount-gui.socket",
			       utility::cmdArgumentValue( l,"-d" ),
			       [ this ]( const QString& e ){ utility::startHelperExecutable( this,
											     e,
											     "zuluMount",
											     "helperStarted",
											     "closeApplication" ) ; },
			       [ this ]( int s ){ this->closeApplication( s ) ;	},
			       [ this ]( const QString& e ){ this->raiseWindow( e ) ; } ) ;
}

void zuluMount::cryfsVolumeProperties()
{
	this->disableAll() ;

	auto mountPath = [ this ](){

		auto table = m_ui->tableWidget ;

		auto row = table->currentRow() ;

		if( row >= 0 ){

			return table->item( row,1 )->text() ;
		}else{
			return QString() ;
		}
	}() ;

	DialogMsg msg( this ) ;

	struct statfs vfs ;

	if( Task::await( [ & ](){ return statfs( mountPath.toLatin1().constData(),&vfs ) ; } ) ){

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed To Read Volume Properties" ) ) ;

		return this->enableAll() ;
	}

	msg.ShowUIInfo( tr( "INFORMATION" ),true,[ & ](){

		return QString( [ this ](){

			auto l = { tr( "Block Size: %1" ),
				   tr( "Used Blocks: %2" ),
				   tr( "Free Blocks: %3" ),
				   tr( "Total Blocks %4" ),
				   tr( "Used Space: %5" ),
				   tr( "Free Space: %6" ),
				   tr( "Total Space: %7" ),
				   tr( "Used %: %8" ) } ;

			QString e ;

			for( const auto& it : l ){

				e += it + "\n\n" ;
			}

			e.truncate( e.size() - 2 ) ;

			return e ;

		}() ).arg( [ & ](){

			return utility::prettyfySpaceUsage( vfs.f_bsize ) ;

		}(),[ & ](){

			return QString::number( vfs.f_blocks - vfs.f_bavail ) ;

		}(),[ & ](){

			return QString::number( vfs.f_bfree ) ;

		}(),[ & ](){

			return QString::number( vfs.f_blocks ) ;

		}(),[ & ](){

			auto s = vfs.f_bsize * ( vfs.f_blocks - vfs.f_bavail ) ;

			return utility::prettyfySpaceUsage( s ) ;

		}(),[ & ](){

			return utility::prettyfySpaceUsage( vfs.f_bsize * vfs.f_bavail ) ;

		}(),[ & ](){

			return utility::prettyfySpaceUsage( vfs.f_bsize * vfs.f_blocks ) ;

		}(),[ & ]()->QString{

			if( vfs.f_bfree == 0 ){

				return "100%" ;
			}else{
				quint64 s = vfs.f_blocks - vfs.f_bavail ;

				auto e = double( s ) / double( vfs.f_blocks ) ;

				return QString::number( e * 100,'g',2 ) + "%" ;
			}
		}() ) ;
	}() ) ;

	this->enableAll() ;
}

static void _volume_properties( const QString& cmd,const QString& arg,
				QTableWidget * table,QWidget * w )
{
	auto exe = utility::executableFullPath( cmd ) ;

	auto path = [ table ](){

		auto row = table->currentRow() ;

		if( row < 0 ){

			return QString() ;
		}else{
			return utility::Task::makePath( table->item( row,0 )->text() ) ;
		}
	}() ;

	if( exe.isEmpty() ){

		DialogMsg( w ).ShowUIOK( QObject::tr( "ERROR" ),
					 QObject::tr( "Failed To Find %1 Executable" ).arg( cmd ) ) ;
	}else{
		auto e = utility::Task::run( exe + arg + path,utility::Task::USEPOLKIT::False ).await() ;

		if( e.success() ){

			auto s = e.stdOut() ;

			if( cmd == "gocryptfs" ){

				s.replace( "Creator:      ","Creator: " ).replace( "\n","\n\n" ) ;
			}

			DialogMsg( w ).ShowUIInfo( QObject::tr( "INFORMATION" ),true,s ) ;
		}else{
			DialogMsg( w ).ShowUIOK( QObject::tr( "ERROR" ),
						 QObject::tr( "Failed To Get Volume Properties" ) ) ;
		}
	}
}

void zuluMount::encfsProperties()
{
	this->disableAll() ;

	_volume_properties( "encfsctl"," ",m_ui->tableWidget,this ) ;

	this->enableAll() ;
}

void zuluMount::securefsProperties()
{
	this->disableAll() ;

	_volume_properties( "securefs"," info ",m_ui->tableWidget,this ) ;

	this->enableAll() ;
}

void zuluMount::gocryptfsProperties()
{
	this->disableAll() ;

	_volume_properties( "gocryptfs"," -info ",m_ui->tableWidget,this ) ;

	this->enableAll() ;
}

static QString _fs_properties( const QString& m )
{
	QFile f( "/proc/self/mountinfo" ) ;

	f.open( QIODevice::ReadOnly ) ;

	auto s = " " + m + " " ;

	for( const auto& it : utility::split( f.readAll() ) ){

		if( it.contains( s ) ){

			return utility::split( it,' ' ).last() ;
		}
	}

	return QString() ;
}

void zuluMount::ecryptfsProperties()
{
	this->disableAll() ;

	auto s = [ this ](){

		auto table = m_ui->tableWidget ;

		auto item = table->currentItem() ;

		if( item ){

			return table->item( item->row(),1 )->text() ;
		}else{
			return QString() ;
		}
	}() ;

	DialogMsg( this ).ShowUIInfo( tr( "INFORMATION" ),true,[ & ](){

		auto e = _fs_properties( s ) ;

		e.replace( ",","\n\n" ) ;
		e.replace( "ro\n\n","mode=read only\n\n" ) ;
		e.replace( "rw\n\n","mode=read and write\n\n" ) ;
		e.replace( "="," = " ) ;

		return e ;
	}() ) ;

	this->enableAll() ;
}

void zuluMount::showContextMenu( QTableWidgetItem * item,bool itemClicked )
{
	QMenu m ;

	m.setFont( this->font() ) ;

	auto _text = [ this ]( int row,int column ){

		return m_ui->tableWidget->item( row,column )->text() ;
	} ;

	auto row = item->row() ;

	auto device = _text( row,0 ) ;

	auto mt = _text( row,1 ) ;

	auto fs = _text( row,2 ) ;

	auto _properties_menu = [ & ]( const QString& ff,bool addSeparator ){

		auto fs = ff.toLower() ;

		if( _encrypted_folder( fs ) ){

			if( fs == "cryfs" ){

				connect( m.addAction( tr( "Properties" ) ),SIGNAL( triggered() ),
					 this,SLOT( cryfsVolumeProperties() ) ) ;

			}else if( fs == "gocryptfs" ){

				connect( m.addAction( tr( "Properties" ) ),SIGNAL( triggered() ),
					 this,SLOT( gocryptfsProperties() ) ) ;

			}else if( fs == "securefs" ){

				connect( m.addAction( tr( "Properties" ) ),SIGNAL( triggered() ),
					 this,SLOT( securefsProperties() ) ) ;

			}else if( fs == "encfs" ){

				connect( m.addAction( tr( "Properties" ) ),SIGNAL( triggered() ),
					 this,SLOT( encfsProperties() ) ) ;

			}else if( fs == "ecryptfs" ){

				connect( m.addAction( tr( "Properties" ) ),SIGNAL( triggered() ),
					 this,SLOT( ecryptfsProperties() ) ) ;
			}else{
				m.addAction( tr( "Properties" ) )->setEnabled( false ) ;
			}
		}else{
			auto ac = m.addAction( tr( "Properties" ) ) ;

			//ac->setEnabled( fs != "crypto_BitLocker" ) ;

			connect( ac,SIGNAL( triggered() ),
				 this,SLOT( volumeProperties() ) ) ;
		}

		if( addSeparator ){

			m.addSeparator() ;
		}
	} ;

	if( mt == "Nil" ){

		connect( m.addAction( tr( "Mount" ) ),SIGNAL( triggered() ),this,SLOT( slotMount() ) ) ;
	}else{
		auto mp   = QString( "/run/media/private/%1/" ).arg( utility::userName() ) ;
		auto mp_1 = QString( "/home/%1/" ).arg( utility::userName() ) ;

		if( mt.startsWith( mp ) || mt.startsWith( mp_1 ) ){

			m_sharedFolderPath = utility::sharedMountPointPath( mt ) ;

			if( m_sharedFolderPath.isEmpty() ){

				connect( m.addAction( tr( "Open Folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenFolder() ) ) ;
			}else{
				connect( m.addAction( tr( "Open Private Folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenFolder() ) ) ;
				connect( m.addAction( tr( "Open Shared Folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenSharedFolder() ) ) ;
			}

			m.addSeparator() ;

			_properties_menu( fs,true ) ;

			connect( m.addAction( tr( "Unmount" ) ),SIGNAL( triggered() ),this,SLOT( pbUmount() ) ) ;

			if( !m_powerOff.isEmpty() && !_encrypted_folder( fs ) && ( device.startsWith( "/dev/sd" ) || device.startsWith( "/dev/hd" ) ) ){

				connect( m.addAction( tr( "Unmount + Power Down" ) ),SIGNAL( triggered() ),this,SLOT( pbUmount_powerDown() ) ) ;
			}
		}else{
			m_sharedFolderPath = utility::sharedMountPointPath( mt ) ;

			if( m_sharedFolderPath.isEmpty() ){

				if( utility::pathIsReadable( mt ) ){

					connect( m.addAction( tr( "Open Folder" ) ),SIGNAL( triggered() ),
						 this,SLOT( slotOpenFolder() ) ) ;

					m.addSeparator() ;

					_properties_menu( fs,false ) ;
				}else{
					_properties_menu( fs,false ) ;
				}
			}else{
				connect( m.addAction( tr( "Open Shared Folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenSharedFolder() ) ) ;

				m.addSeparator() ;

				_properties_menu( fs,true ) ;
			}
		}
	}

	m.addSeparator() ;
	m.addAction( tr( "Close Menu" ) ) ;

	if( itemClicked ){

		m.exec( QCursor::pos() ) ;
	}else{
		auto p = this->pos() ;
		auto x = p.x() + 100 + m_ui->tableWidget->columnWidth( 0 ) ;
		auto y = p.y() + 50 + m_ui->tableWidget->rowHeight( 0 ) * item->row() ;
		p.setX( x ) ;
		p.setY( y ) ;
		m.exec( p ) ;
	}
}

void zuluMount::itemClicked( QTableWidgetItem * item )
{
	this->showContextMenu( item,true ) ;
}

void zuluMount::defaultButton()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		auto row = m_ui->tableWidget->currentRow() ;
		auto mt = m_ui->tableWidget->item( row,1 )->text() ;

		if( mt == "Nil" ){

			this->slotMount() ;
		}else{
			this->showContextMenu( m_ui->tableWidget->currentItem(),false ) ;
		}
	}
}

void zuluMount::slotOpenSharedFolder()
{
	this->openMountPoint( m_sharedFolderPath ) ;
}

void zuluMount::slotOpenFolder()
{
	auto item = m_ui->tableWidget->currentItem() ;
	auto path = m_ui->tableWidget->item( item->row(),1 )->text() ;

	this->openMountPoint( path ) ;
}

void zuluMount::openMountPoint( const QString& m_point )
{
	auto x = tr( "Warning" ) ;
	auto y = tr( "Could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_folderOpener ) ;

	utility::openPath( m_point,m_folderOpener,this,x,y ) ;
}

void zuluMount::openMountPointPath( QString m )
{
	if( m_autoOpenFolderOnMount ){

		this->openMountPoint( m ) ;
	}
}

void zuluMount::volumeProperties()
{
	this->disableAll() ;

	auto volume     = m_ui->tableWidget->item( m_ui->tableWidget->currentRow(),0 )->text() ;
	auto volumeType = m_ui->tableWidget->item( m_ui->tableWidget->currentRow(),2 )->text() ;

	auto r = zuluMountTask::volumeProperties( volume,volumeType ).await() ;

	DialogMsg msg( this ) ;

	if( r.isEmpty() ){

		msg.ShowUIOK( tr( "ERROR" ),
			      tr( "Could not get volume properties.\nvolume is not open or was opened by a different user" ) ) ;
	}else{
		auto i = r.indexOf( "\n" ) ;

		if( i != -1 ){
			msg.ShowUIVolumeProperties( tr( "Volume Properties" ),r.mid( i + 1 ) ) ;
		}else{
			msg.ShowUIOK( tr( "ERROR" ),
				      tr( "Could not get volume properties.\nvolume is not open or was opened by a different user" ) ) ;
		}
	}

	this->enableAll() ;
}

void zuluMount::setUpShortCuts()
{
	auto _addAction = [ this ]( std::initializer_list<QKeySequence> s,const char * slot ){

		auto ac = new QAction( this ) ;

		ac->setShortcuts( s ) ;

		connect( ac,SIGNAL( triggered() ),this,slot ) ;

		return ac ;
	} ;

	this->addAction( _addAction( { Qt::Key_Enter,Qt::Key_Return },SLOT( defaultButton() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_M },SLOT( pbMount() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_U },SLOT( pbUmount() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_R },SLOT( pbUpdate() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_C },SLOT( closeApplication() ) ) ) ;
}

void zuluMount::setUpFont()
{
	this->setFont( utility::getFont( this ) ) ;
}

void zuluMount::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	this->hide() ;

	if( utility::doNotMinimizeToTray() ){

		this->closeApplication() ;
	}
}

void zuluMount::slotTrayClicked( QSystemTrayIcon::ActivationReason e )
{
	if( e == QSystemTrayIcon::Trigger ){

		if( this->isVisible() ){

			this->hide() ;
		}else{
			this->show() ;
		}
	}
}

void zuluMount::autoMountToggled( bool opt )
{
	m_autoMount = opt ;
}

void zuluMount::dragEnterEvent( QDragEnterEvent * e )
{
	e->accept() ;
}

void zuluMount::dropEvent( QDropEvent * e )
{
	for( const auto& it : e->mimeData()->urls() ){

		this->showMoungDialog( it.path() ) ;
	}
}

void zuluMount::mount( const volumeProperty& entry )
{
	this->disableAll() ;

	if( entry.encryptedVolume() ){

		keyDialog::instance( this,m_ui->tableWidget,m_secrets,entry,[ this ](){

			this->enableAll() ;

		},[ this ]( const QString& e ){

			this->openMountPointPath( e ) ;

		} ).ShowUI() ;
	}else{
		mountPartition::instance( this,m_ui->tableWidget,[ this ](){

			this->enableAll() ;

		},[ this ]( const QString& e ){

			this->openMountPointPath( e ) ;

		} ).ShowUI( entry ) ;
	}
}

void zuluMount::slotMount()
{
	auto table = m_ui->tableWidget ;
	auto row = table->currentRow() ;

	this->mount( tablewidget::rowEntries( table,row ) ) ;
}

void zuluMount::showMoungDialog( const volumeProperty& v )
{
	if( v.isEmpty() ){

		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),
			      tr( "Permission to access the volume was denied\nor\nthe volume is not supported\n(LVM/MDRAID signatures found)" ) ) ;
		this->enableAll() ;
	}else{
		this->mount( v ) ;
	}
}

void zuluMount::showMoungDialog( const QString& volume,const QString& m_point )
{
	if( !volume.isEmpty() ){

		if( utility::pathPointsToAFolder( volume ) ){

			this->mount( { volume,m_point,"cryptfs","Nil","Nil","Nil" } ) ;
		}else{
			this->disableAll() ;

			this->showMoungDialog( zuluMountTask::getVolumeProperties( volume ).await().setMountPoint( m_point ) ) ;
		}
	}
}

void zuluMount::pbMount()
{
	this->disableAll() ;

	auto path = QFileDialog::getOpenFileName( this,tr( "Select An Image File To Mount" ),utility::homePath() ) ;

	if( path.isEmpty() ){

		this->enableAll() ;
	}else{
		this->showMoungDialog( path ) ;
	}
}

void zuluMount::unlockCryptFs()
{
	this->disableAll() ;

	auto path = QFileDialog::getExistingDirectory( this,tr( "Select An Encrypted Volume Directory" ),
						       utility::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( path.isEmpty() ){

		this->enableAll() ;
	}else{
		while( true ){

			if( path.endsWith( '/' ) ){

				path.truncate( path.length() - 1 ) ;
			}else{
				break ;
			}
		}

		this->showMoungDialog( path ) ;
	}
}

QFont zuluMount::getSystemVolumeFont()
{
	auto f = this->font() ;
	f.setItalic( !f.italic() ) ;
	f.setBold( !f.bold() ) ;
	return f ;
}

void zuluMount::addEntryToTable( bool systemVolume,const QStringList& l )
{
	if( systemVolume ){

		tablewidget::addRow( m_ui->tableWidget,l,this->getSystemVolumeFont() ) ;
	}else{
		tablewidget::addRow( m_ui->tableWidget,l ) ;
	}
}

void zuluMount::addEntryToTable( bool systemVolume,const volumeProperty& e )
{
	this->addEntryToTable( systemVolume,e.entryList() ) ;
}

void zuluMount::removeEntryFromTable( QString volume )
{
	auto table = m_ui->tableWidget ;

	auto r = tablewidget::columnHasEntry( table,volume ) ;

	if( r != -1 ){

		tablewidget::deleteRow( table,r ) ;
		this->enableAll() ;
	}else{
		this->pbUpdate() ;
	}
}

void zuluMount::volumeMiniProperties( bool valid,volumeProperty * e )
{
	std::unique_ptr< volumeProperty > volumeInfo( e ) ;

	this->disableAll() ;

	if( valid ){

		this->updateList( *volumeInfo ) ;
		this->enableAll() ;
	}else{
		this->pbUpdate() ;
	}
}

void zuluMount::volumeMiniProperties( volumeProperty * volumeInfo )
{
	this->volumeMiniProperties( volumeInfo,volumeInfo ) ;
}

void zuluMount::volumeMiniProperties_0( volumeProperty * volumeInfo )
{
	this->volumeMiniProperties( volumeInfo && volumeInfo->mounted(),volumeInfo ) ;
}

void zuluMount::updateList( const volumeProperty& entry )
{
	if( entry.isValid() ){

		auto table = m_ui->tableWidget ;

		auto row = tablewidget::columnHasEntry( table,entry.volumeName() ) ;

		if( row == -1 ){

			row = tablewidget::addRow( table ) ;
		}
		if( entry.isSystem() ){

			tablewidget::updateRow( table,entry.entryList(),row,this->getSystemVolumeFont() ) ;
		}else{
			tablewidget::updateRow( table,entry.entryList(),row,this->font() ) ;
		}

		tablewidget::selectRow( table,row ) ;
	}
}

bool zuluMount::errorNotFound( int r )
{
	DialogMsg m( this ) ;

	switch ( r ) {

		case 0 :break ;
		case 1 :m.ShowUIOK( tr( "ERROR!" ),tr( "Volume is not open or was opened by a different user" ) ) ;					break ;
		case 2 :m.ShowUIOK( tr( "ERROR!" ),tr( "One or more files in the volume are in use." ) ) ;						break ;
		case 3 :m.ShowUIOK( tr( "ERROR!" ),tr( "Volume does not have an entry in /etc/mtab" ) ) ;						break ;
		case 4 :m.ShowUIOK( tr( "ERROR!" ),tr( "Could not get a lock on /etc/mtab~" ) ) ;							break ;
		case 5 :m.ShowUIOK( tr( "ERROR!" ),tr( "Volume is unmounted but could not close mapper,advice to close it manually" ) ) ;		break ;
		case 6 :m.ShowUIOK( tr( "ERROR!" ),tr( "Could not resolve full path of device\n" ) ) ;							break ;
		case 7 :m.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point appear to be busy\n" ) ) ;							break ;
		case 8 :m.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point appear to belong to a different user or multiple mount points detected\n" ) ) ; break ;
		case 9 :m.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point appear to be in an ambiguous state,advice to unmount manually" ) ) ;		break ;
		case 10:m.ShowUIOK( tr( "ERROR!" ),tr( "Multiple mount points for the volume detected" ) ) ;						break ;
		case 100 :m.ShowUIOK( tr( "ERROR!" ),tr( "Device does not appear to be mounted" ) ) ;							break ;
		case 101 :m.ShowUIOK( tr( "ERROR!" ),tr( "Only root user of members of group \"zulumount\" can unmount this volume" ) ) ;               break ;
		case 107 :m.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point appear to be busy" ) ) ;							break ;
		case 108 :m.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point appear to belong to a different user" ) ) ;				break ;
		case 109 :m.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point appear to be in an ambiguous state,advice to unmount manually" ) ) ;       break ;
		case 103 :m.ShowUIOK( tr( "ERROR!" ),tr( "Device does not exist" ) ) ;									break ;
		case 104 :m.ShowUIOK( tr( "ERROR!" ),tr( "Failed to unmount,the mount point and/or one or more files are in use" ) ) ;                  break ;
		case 105 :m.ShowUIOK( tr( "ERROR!" ),tr( "Failed to unmount,could not get a lock on /etc/mtab~" ) ) ;					break ;
		case 106 :m.ShowUIOK( tr( "ERROR!" ),tr( "Failed to unmount the partition" ) ) ;							break ;
		case 111 :m.ShowUIOK( tr( "ERROR!" ),tr( "Failed to unmount,multiple mount points for the volume detected" ) ) ;			break ;
		case 110 :m.ShowUIOK( tr( "ERROR!" ),tr( "Close failed, could not find any partition with the presented UUID" ) ) ;			break ;
		default: return true ;
	}

	return false ;
}

void zuluMount::unmount( const QString& e )
{
	this->disableAll() ;

	auto row = m_ui->tableWidget->currentRow() ;

	auto path      = m_ui->tableWidget->item( row,0 )->text() ;
	auto mountPath = m_ui->tableWidget->item( row,1 )->text() ;
	auto type      = m_ui->tableWidget->item( row,2 )->text() ;

	if( !type.startsWith( "crypto_" ) ){

		utility::Task::suspendForOneSecond() ;
	}

	if( _encrypted_folder( type ) ){

		if( siritask::encryptedFolderUnMount( path,mountPath,type ).await() ){

			siritask::deleteMountFolder( mountPath ) ;
		}else{
			DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Failed to unmount volume" ) ) ;
			this->enableAll() ;
		}
	}else{
		auto r = zuluMountTask::unmountVolume( path,type,e ).await() ;

		if( r.failed() ){

			if( this->errorNotFound( r.exitCode() ) ){

				DialogMsg m( this ) ;

				QString z = r.stdOut() ;

				z.replace( tr( "ERROR: " ),"" ) ;
				z.replace( "ERROR: ","" ) ;

				m.ShowUIOK( tr( "ERROR" ),z ) ;
			}

			this->enableAll() ;
		}
	}
}

void zuluMount::pbUmount()
{
	this->unmount() ;
}

void zuluMount::pbUmount_powerDown()
{
	this->unmount( m_powerOff ) ;
}

void zuluMount::emergencyQuitApplication()
{
	this->hide() ;

	auto table = m_ui->tableWidget ;

	m_removeAllVolumes = true ;

	auto paths       = tablewidget::columnEntries( table,0 ) ;
	auto mountPoints = tablewidget::columnEntries( table,1 ) ;
	auto fileSystems = tablewidget::columnEntries( table,2 ) ;

	auto mountPath = utility::mountPath( QString() ) ;
	auto homeMountPath = utility::homeMountPath( QString() ) ;

	for( int i = 0 ; i < paths.size() ; i++ ){

		const auto& a = paths.at( i ) ;
		const auto& b = mountPoints.at( i ) ;
		const auto& c = fileSystems.at( i ) ;

		if( utility::startsWithAtLeastOne( b,mountPath,homeMountPath ) ){

			if( _encrypted_folder( c ) ){

				if( siritask::encryptedFolderUnMount( a,b,c ).await() ){

					siritask::deleteMountFolder( b ) ;
				}
			}else{
				zuluMountTask::unmountVolume( a,c ).await() ;
			}
		}
	}

	this->closeApplication() ;
}

void zuluMount::unMountAll()
{
	this->disableAll() ;

	auto table = m_ui->tableWidget ;

	m_removeAllVolumes = true ;

	auto paths       = tablewidget::columnEntries( table,0 ) ;
	auto mountPoints = tablewidget::columnEntries( table,1 ) ;
	auto fileSystems = tablewidget::columnEntries( table,2 ) ;

	auto mountPath = utility::mountPath( QString() ) ;
	auto homeMountPath = utility::homeMountPath( QString() ) ;

	int r = paths.size() - 1 ;

	if( r < 0 ){

		utility::Task::suspendForOneSecond() ;
	}else{
		do{
			const auto& a = paths.at( r ) ;
			const auto& b = mountPoints.at( r ) ;
			const auto& c = fileSystems.at( r ) ;

			if( utility::startsWithAtLeastOne( b,mountPath,homeMountPath ) ){

				if( _encrypted_folder( c ) ){

					if( siritask::encryptedFolderUnMount( a,b,c ).await() ){

						siritask::deleteMountFolder( b ) ;
					}
				}else{
					zuluMountTask::unmountVolume( a,c ).await() ;
				}

				utility::Task::suspendForOneSecond() ;
			}

			r-- ;

		}while( r >= 0 ) ;

		utility::Task::suspendForOneSecond() ;
	}

	this->enableAll_1() ;
}

void zuluMount::pbUpdate()
{
	this->disableAll() ;

	auto r = zuluMountTask::updateVolumeList().await() ;

	this->updateVolumeList( r ) ;

	this->removeDisappearedEntries( r ) ;
}

void zuluMount::updateVolumeList( const QVector< volumeProperty >& r )
{
	if( r.isEmpty() ){

		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),
			      tr( "Reading partition properties took longer than expected and operation was terminated,click refresh to try again" ) ) ;

		this->enableAll() ;
	}else{
		for( const auto& it : r ){

			if( it.isValid() ){

				this->updateList( it ) ;
			}
		}
	}
}

void zuluMount::removeDisappearedEntries( const QVector< volumeProperty >& entries )
{
	/*
	 * Below routine removes an entries from the table if they are found not to be
	 * present on the the list of volumes we just received.This is necessary
	 * for example to remove no longer valid options like a removed cdrom
	 */

	if( entries.isEmpty() ){

		return this->enableAll() ;
	}

	auto table = m_ui->tableWidget ;

	auto l = tablewidget::columnEntries( table,0 ) ;

	auto _hasNoEntry = [ & ]( const QString& volume ){

		for( const auto& it : entries ){

			if( it.volumeName() == volume ){

				if( _encrypted_folder( it.fileSystem(),false ) ){

					return false ;
				}
				if( it.volumeSize() == "Nil" ){

					return true ;
				}else{
					return false ;
				}
			}
		}

		return true ;
	} ;

	decltype( l ) z ;

	for( const auto& it : l ){

		if( _hasNoEntry( it ) ){

			z.append( it ) ;
		}
	}

	auto _done = [ this ](){

		tablewidget::selectLastRow( m_ui->tableWidget ) ;
		this->enableAll() ;
	} ;

	if( z.isEmpty() ){

		_done() ;
	}else{
		for( const auto& e : z ){

			tablewidget::selectRow( table,e ) ;
			utility::Task::suspendForOneSecond() ;
			tablewidget::deleteRow( table,e ) ;
		}

		utility::Task::suspendForOneSecond() ;
		_done() ;
	}
}

void zuluMount::slotCurrentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectRow( current,previous ) ;
}

void zuluMount::disableAll()
{
	m_ui->pbmenu->setEnabled( false ) ;
	m_ui->pbmount->setEnabled( false ) ;
	m_ui->pbupdate->setEnabled( false ) ;
	m_ui->tableWidget->setEnabled( false ) ;
	m_ui->pbunlockcryptfs->setEnabled( false ) ;
	m_ui->pbfavorites->setEnabled( false ) ;
}

void zuluMount::enableAll()
{
	if( m_removeAllVolumes ){

		return ;
	}
	m_ui->pbmenu->setEnabled( true ) ;
	m_ui->pbupdate->setEnabled( true ) ;
	m_ui->tableWidget->setEnabled( true ) ;
	m_ui->pbmount->setEnabled( true ) ;
	m_ui->tableWidget->setFocus() ;
	m_ui->pbunlockcryptfs->setEnabled( true ) ;
	m_ui->pbfavorites->setEnabled( true ) ;
}

void zuluMount::enableAll_1()
{
	m_removeAllVolumes = false ;
	this->enableAll() ;
}

bool zuluMount::autoMount()
{
	auto& e = utility::settingsObject() ;

	if( !e.contains( "AutoMountVolumes" ) ){

		e.setValue( "AutoMountVolumes",true ) ;
	}

	return e.value( "AutoMountVolumes" ).toBool() ;
}

zuluMount::~zuluMount()
{
	if( !m_ui ){

		return ;
	}

	if( m_autoMountAction ){

		utility::settingsObject().setValue( "AutoMountVolumes",m_autoMountAction->isChecked() ) ;
	}

	auto q = m_ui->tableWidget ;

	const auto& r = this->window()->geometry() ;

	utility::setWindowDimensions( "zuluMount",{ r.x(),
						    r.y(),
						    r.width(),
						    r.height(),
						    q->columnWidth( 0 ),
						    q->columnWidth( 1 ),
						    q->columnWidth( 2 ),
						    q->columnWidth( 4 ),
						    q->columnWidth( 5 ) } ) ;

	delete m_ui ;
}

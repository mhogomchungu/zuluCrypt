/*
 *
 *  Copyright (c) 2012
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
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

#include <utility>

#include <unistd.h>
#include "keydialog.h"
#include "../zuluCrypt-gui/dialogmsg.h"
#include "../zuluCrypt-gui/userfont.h"
#include "../zuluCrypt-gui/tablewidget.h"
#include "mountpartition.h"
#include "oneinstance.h"
#include "events.h"
#include "monitor_mountinfo.h"
#include "../zuluCrypt-gui/utility.h"
#include "zulumounttask.h"
#include "../zuluCrypt-gui/task.h"

template< typename T >
class Object_raii
{
public:
	explicit Object_raii( T t ) : m_Object( t )
	{
	}
	~Object_raii()
	{
		delete m_Object ;
	}
private:
	T m_Object ;
};

#define Object_raii( x ) Object_raii< decltype( x ) > Object_raii_x( x ) ; Q_UNUSED( Object_raii_x )

MainWindow::MainWindow( QWidget * parent ) : QWidget( parent )
{
}

void MainWindow::setUpApp( const QString& volume )
{
	this->setLocalizationLanguage() ;
	m_ui = new Ui::MainWindow ;
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;

	m_ui->tableWidget->setColumnWidth( 0,220 ) ;
	m_ui->tableWidget->setColumnWidth( 1,320 ) ;
	m_ui->tableWidget->setColumnWidth( 2,145 ) ;
	m_ui->tableWidget->setColumnWidth( 4,87 ) ;
	m_ui->tableWidget->setColumnWidth( 5,87 ) ;
	m_ui->tableWidget->hideColumn( 3 ) ;

#if QT_VERSION < QT_VERSION_CHECK( 5,0,0 )
	m_ui->tableWidget->verticalHeader()->setResizeMode( QHeaderView::ResizeToContents ) ;
#else
	m_ui->tableWidget->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;
#endif
	m_ui->tableWidget->verticalHeader()->setMinimumSectionSize( 30 ) ;

	this->setAcceptDrops( true ) ;
	this->setWindowIcon( QIcon( ":/zuluMount.png" ) ) ;

	m_ui->tableWidget->setMouseTracking( true ) ;

	connect( m_ui->tableWidget,SIGNAL( itemEntered( QTableWidgetItem * ) ),this,SLOT( itemEntered( QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->pbmount,SIGNAL( clicked() ),this,SLOT( pbMount() ) ) ;
	connect( m_ui->pbupdate,SIGNAL( clicked()),this,SLOT( pbUpdate() ) ) ;
	connect( m_ui->pbclose,SIGNAL( clicked() ),this,SLOT( pbClose() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;
	connect( m_ui->pbunlockencfs,SIGNAL( clicked() ),this,SLOT( unlockencfs() ) ) ;

	connect( this,SIGNAL( unlistVolume( QString ) ),this,SLOT( removeVolume( QString ) ) ) ;

	this->setUpShortCuts() ;

	this->setUpFont() ;

	m_trayIcon = new QSystemTrayIcon( this ) ;
	m_trayIcon->setIcon( QIcon( ":/zuluMount.png" ) ) ;

	auto trayMenu = new QMenu( this ) ;

	m_autoMountAction = new QAction( this ) ;
	m_autoMount = this->autoMount() ;
	m_autoMountAction->setCheckable( true ) ;
	m_autoMountAction->setChecked( m_autoMount ) ;

	m_autoMountAction->setText( tr( "Automount Volumes" ) ) ;

	connect( m_autoMountAction,SIGNAL( toggled( bool ) ),this,SLOT( autoMountToggled( bool ) ) ) ;

	trayMenu->addAction( m_autoMountAction ) ;

	auto autoOpenFolderOnMount = new QAction( this ) ;
	autoOpenFolderOnMount->setCheckable( true ) ;
	m_autoOpenFolderOnMount = this->autoOpenFolderOnMount() ;
	autoOpenFolderOnMount->setChecked( m_autoOpenFolderOnMount ) ;
	autoOpenFolderOnMount->setText( tr( "Auto Open Mount Point" ) ) ;
	connect( autoOpenFolderOnMount,SIGNAL( toggled( bool ) ),this,SLOT( autoOpenFolderOnMount( bool ) ) ) ;

	trayMenu->addAction( autoOpenFolderOnMount ) ;

	auto ac = new QAction( this ) ;
	ac->setText( tr( "Show The Interface" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( raiseWindow() ) ) ;

	trayMenu->addAction( ac ) ;

	ac = new QAction( this ) ;
	ac->setText( tr( "Unmount All" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( unMountAll() ) ) ;

	trayMenu->addAction( ac ) ;

	m_favorite_menu = trayMenu->addMenu( tr( "Favorites" ) ) ;
	connect( m_favorite_menu,SIGNAL( triggered( QAction * ) ),this,SLOT( favoriteClicked( QAction * ) ) ) ;
	connect( m_favorite_menu,SIGNAL( aboutToShow() ),this,SLOT( showFavorites() ) ) ;

	m_not_hidden_volume_menu = trayMenu->addMenu( tr( "Hide Volume From View" ) ) ;
	connect( m_not_hidden_volume_menu,SIGNAL( triggered( QAction * ) ),this,SLOT( removeVolumeFromVisibleVolumeList( QAction * ) ) ) ;
	connect( m_not_hidden_volume_menu,SIGNAL( aboutToShow() ),this,SLOT( showVisibleVolumeList() ) ) ;

	m_hidden_volume_menu = trayMenu->addMenu( tr( "Unhide Volume From View" ) ) ;
	connect( m_hidden_volume_menu,SIGNAL( triggered( QAction * ) ),this,SLOT( removeVolumeFromHiddenVolumeList( QAction * ) ) ) ;
	connect( m_hidden_volume_menu,SIGNAL( aboutToShow() ),this,SLOT( showHiddenVolumeList() ) ) ;

	ac = new QAction( this ) ;
	ac->setText( tr( "About" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( licenseInfo() ) ) ;
	trayMenu->addAction( ac ) ;

	trayMenu->addAction( tr( "Quit" ),this,SLOT( pbClose() ) ) ;
	m_trayIcon->setContextMenu( trayMenu ) ;

	connect( m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		 this,SLOT( slotTrayClicked( QSystemTrayIcon::ActivationReason ) ) ) ;

	m_trayIcon->show() ;

	QString dirPath = QDir::homePath() + "/.zuluCrypt/" ;
	QDir dir( dirPath ) ;

	if( !dir.exists() ){
		dir.mkdir( dirPath ) ;
	}

	this->disableAll() ;

	this->startAutoMonitor() ;

	this->updateVolumeList( zuluMountTask::updateVolumeList().await() ) ;

	if( volume.isEmpty() ) {
		this->enableAll() ;
	}else{
		this->showMoungDialog( volume ) ;
	}
}

void MainWindow::licenseInfo()
{
	utility::licenseInfo( this ) ;
}

void MainWindow::removeVolumeFromHiddenVolumeList( QAction * ac )
{
	this->disableAll() ;

	auto e = ac->text() ;

	zuluMountTask::removeVolumeFromHiddenVolumeList( e ) ;

	this->updateList( zuluMountTask::getVolumeProperties( e ).await() ) ;

	this->enableAll() ;
}

void MainWindow::showHiddenVolumeList()
{
	m_hidden_volume_menu->clear() ;

	QStringList l = zuluMountTask::hiddenVolumeList() ;

	if( l.isEmpty() ){

		auto ac = new QAction( tr( "List Is Empty" ),m_hidden_volume_menu ) ;
		ac->setEnabled( false ) ;
		m_hidden_volume_menu->addAction( ac ) ;
	}else{
		for( const auto& it : l ){
			auto ac = new QAction( it,m_hidden_volume_menu ) ;
			m_hidden_volume_menu->addAction( ac ) ;
		}
	}
}

void MainWindow::showVisibleVolumeList()
{
	m_not_hidden_volume_menu->clear() ;

	QStringList l = tablewidget::tableColumnEntries( m_ui->tableWidget ) ;

	if( l.isEmpty() ){

		auto ac = new QAction( tr( "List Is Empty" ),m_not_hidden_volume_menu ) ;
		ac->setEnabled( false ) ;
		m_not_hidden_volume_menu->addAction( ac ) ;
	}else{
		for( const auto& it : l ){
			auto ac = new QAction( it,m_not_hidden_volume_menu ) ;
			m_not_hidden_volume_menu->addAction( ac ) ;
		}
	}
}

void MainWindow::removeVolumeFromVisibleVolumeList( QAction * ac )
{
	auto e = ac->text() ;
	auto table = m_ui->tableWidget ;

	zuluMountTask::addVolumeToHiddenVolumeList( e ) ;

	this->disableAll() ;

	tablewidget::selectRow( table,e ) ;
	utility::Task::suspendForOneSecond() ;
	tablewidget::deleteTableRow( table,e ) ;

	tablewidget::selectLastRow( table ) ;

	this->enableAll() ;
}

void MainWindow::favoriteClicked( QAction * ac )
{
	this->showMoungDialog( ac->text() ) ;
}

void MainWindow::showFavorites()
{
	m_favorite_menu->clear() ;
	QStringList l = utility::readFavorites() ;

	if( l.isEmpty() ){
		auto ac = new QAction( tr( "List Is Empty" ),m_favorite_menu ) ;
		ac->setEnabled( false ) ;
		m_favorite_menu->addAction( ac ) ;
	}else{
		l.removeLast() ;
		for( const auto& it : l ){
			auto ac = new QAction( it.split( "\t" ).first(),m_favorite_menu ) ;
			m_favorite_menu->addAction( ac ) ;
		}
	}
}

void MainWindow::setLocalizationLanguage()
{
	auto translator  = new QTranslator( this ) ;

	const char * app = "zuluMount-gui" ;

	QByteArray r = utility::localizationLanguage( app ).toLatin1() ;

	if( r == "en_US" ){
		/*
		 * english_US language,its the default and hence dont load anything
		 */
	}else{
		translator->load( r.constData(),utility::localizationLanguagePath( app ) ) ;
		QCoreApplication::installTranslator( translator ) ;
	}
}

#define zuluMOUNT_AUTO_OPEN_FOLDER "/.zuluCrypt/zuluMount-gui.NoAutoOpenFolder"

void MainWindow::autoOpenFolderOnMount( bool b )
{
	QString x = QDir::homePath() + QString( zuluMOUNT_AUTO_OPEN_FOLDER ) ;

	m_autoOpenFolderOnMount = b ;
	if( b ){
		QFile::remove( x ) ;
	}else{
		QFile f( x ) ;
		f.open( QIODevice::WriteOnly ) ;
		f.close() ;
	}
}

bool MainWindow::autoOpenFolderOnMount( void )
{
	QString x = QDir::homePath() + QString( zuluMOUNT_AUTO_OPEN_FOLDER ) ;
	return QFile::exists( x ) == false ;
}

void MainWindow::startAutoMonitor()
{
	m_mountInfo = new monitor_mountinfo( this ) ;
	m_events = new events( this ) ;

	/*
	 * perform an ordely shut down when the application terminates to prevent an occassional crash with
	 * a warning that says something like "an object was destroyed while a thread is still running"
	 *
	 * On quiting,we first shut down auto_mount object and then monitor_mountinfo object and then we
	 * close the application
	 */
	connect( m_mountInfo,SIGNAL( stopped() ),this,SLOT( quitApplication() ) ) ;
	connect( m_events,SIGNAL( stopped() ),m_mountInfo,SLOT( stop() ) ) ;

	m_mountInfo->start() ;
	m_events->start() ;
}

/*
 * This should be the only function that closes the application
 */
void MainWindow::pbClose()
{
	m_events->stop() ;
}

void MainWindow::quitApplication()
{
	QCoreApplication::quit() ;
}

void MainWindow::autoMountVolume( volumeEntryProperties * entry )
{
	Object_raii( entry ) ;

	if( entry && entry->entryisValid() ){

		QStringList l = entry->entryList() ;

		if( entry->encryptedVolume() ){

			this->addEntryToTable( true,l ) ;
		}else{
			if( m_autoMount ){
				auto mp = new mountPartition( this,m_ui->tableWidget ) ;
				connect( mp,SIGNAL( openMountPoint( QString ) ),
					 this,SLOT( openMountPointPath( QString ) ) ) ;
				mp->AutoMount( l ) ;
			}else{
				this->addEntryToTable( false,l ) ;
			}
		}
	}
}

void MainWindow::volumeRemoved( QString volume )
{
	if( !volume.isEmpty() ){

		auto table = m_ui->tableWidget ;

		int row = tablewidget::columnHasEntry( table,volume ) ;

		if( row != -1 ){
			tablewidget::deleteRowFromTable( table,row ) ;
			/*
			* see if a user just removed the device without properly closing it/unmounting it
			* and try to do so for them
			*/

			Task::exec( [ volume ](){ zuluMountTask::checkUnMount( volume ) ; } ) ;

			this->enableAll() ;
		}
	}
}

void MainWindow::removeVolume( QString volume )
{
	if( volume.isEmpty() ){

		tablewidget::selectLastRow( m_ui->tableWidget ) ;
		this->enableAll() ;
	}else{
		tablewidget::deleteTableRow( m_ui->tableWidget,volume ) ;
	}
}

void MainWindow::itemEntered( QTableWidgetItem * item )
{
	int row = item->row() ;
	auto table = item->tableWidget() ;
	QString m_point = table->item( row,1 )->text() ;

	QString x = table->item( row,3 )->text() ;
	QString z ;
	QString y ;

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

void MainWindow::startGUI()
{
	if( !m_startHidden ){
		this->raiseWindow() ;
	}
}

void MainWindow::raiseWindow()
{
	this->setVisible( true ) ;
	this->raise() ;
	this->show() ;
	this->setWindowState( Qt::WindowActive ) ;
}

void MainWindow::raiseWindow( QString volume )
{
	this->setVisible( true ) ;
	this->raise() ;
	this->show() ;
	this->setWindowState( Qt::WindowActive ) ;
	this->showMoungDialog( volume ) ;
}

void MainWindow::Show()
{
	QStringList l = QCoreApplication::arguments() ;

	m_startHidden  = l.contains( "-e" ) ;
	m_folderOpener = utility::cmdArgumentValue( l,"-m","xdg-open" ) ;

	QString volume = utility::cmdArgumentValue( l,"-d" ) ;

	auto instance = new oneinstance( this,"zuluMount-gui.socket","startGUI",volume ) ;

	if( instance->onlyInstance() ){

		connect( instance,SIGNAL( raise() ),this,SLOT( raiseWindow() ) ) ;
		connect( instance,SIGNAL( raiseWithDevice( QString ) ),this,SLOT( raiseWindow( QString ) ) ) ;
		
		this->setUpApp( volume ) ;
	}
}

void MainWindow::showContextMenu( QTableWidgetItem * item,bool itemClicked )
{
	QMenu m ;

	m.setFont( this->font() ) ;

	int row = item->row() ;

	QString mt = m_ui->tableWidget->item( row,1 )->text() ;

	QString device = m_ui->tableWidget->item( row,0 )->text() ;

	if( mt == "Nil" ){

		connect( m.addAction( tr( "Mount" ) ),SIGNAL( triggered() ),this,SLOT( slotMount() ) ) ;
	}else{
		QString mp = QString( "/run/media/private/%1/" ).arg( utility::userName() ) ;
		QString mp_1 = QString( "/home/%1/" ).arg( utility::userName() ) ;

		if( mt.startsWith( mp ) || mt.startsWith( mp_1 ) ){

			connect( m.addAction( tr( "Unmount" ) ),SIGNAL( triggered() ),this,SLOT( pbUmount() ) ) ;

			m.addSeparator() ;

			QString fs = m_ui->tableWidget->item( row,2 )->text() ;

			if( fs != "encfs" ){

				connect( m.addAction( tr( "Properties" ) ),SIGNAL( triggered() ),this,SLOT( volumeProperties() ) ) ;
				m.addSeparator() ;
			}

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
		}else{
			m_sharedFolderPath = utility::sharedMountPointPath( mt ) ;

			if( m_sharedFolderPath.isEmpty() ){

				if( utility::pathIsReadable( mt ) ){

					connect( m.addAction( tr( "Properties" ) ),SIGNAL( triggered() ),this,SLOT( volumeProperties() ) ) ;
					m.addSeparator() ;
					connect( m.addAction( tr( "Open Folder" ) ),SIGNAL( triggered() ),
						 this,SLOT( slotOpenFolder() ) ) ;
				}else{
					connect( m.addAction( tr( "Properties" ) ),SIGNAL( triggered() ),this,SLOT( volumeProperties() ) ) ;
				}
			}else{
				connect( m.addAction( tr( "Properties" ) ),SIGNAL( triggered() ),this,SLOT( volumeProperties() ) ) ;

				m.addSeparator() ;

				connect( m.addAction( tr( "Open Shared Folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenSharedFolder() ) ) ;
			}
		}
	}

	m.addSeparator() ;
	m.addAction( tr( "Close Menu" ) ) ;

	if( itemClicked ){
		m.exec( QCursor::pos() ) ;
	}else{
		QPoint p = this->pos() ;
		int x = p.x() + 100 + m_ui->tableWidget->columnWidth( 0 ) ;
		int y = p.y() + 50 + m_ui->tableWidget->rowHeight( 0 ) * item->row() ;
		p.setX( x ) ;
		p.setY( y ) ;
		m.exec( p ) ;
	}
}

void MainWindow::itemClicked( QTableWidgetItem * item )
{
	this->showContextMenu( item,true ) ;
}

void MainWindow::defaultButton()
{
	int row = m_ui->tableWidget->currentRow() ;
	QString mt = m_ui->tableWidget->item( row,1 )->text() ;

	if( mt == "Nil" ){
		this->slotMount() ;
	}else{
		this->showContextMenu( m_ui->tableWidget->currentItem(),false ) ;
	}
}

void MainWindow::slotOpenSharedFolder()
{
	this->openMountPoint( m_sharedFolderPath ) ;
}

void MainWindow::slotOpenFolder()
{
	auto item = m_ui->tableWidget->currentItem() ;
	QString path = m_ui->tableWidget->item( item->row(),1 )->text() ;

	this->openMountPoint( path ) ;
}

void MainWindow::openMountPoint( const QString& m_point )
{
	utility::openMountPoint( m_point,m_folderOpener ).then( [ this ]( bool failed ){

		if( failed ){
			QString x = tr( "Could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_folderOpener ) ;
			DialogMsg msg( this ) ;
			msg.ShowUIOK( tr( "Warning" ),x ) ;
		}
	} ) ;
}

void MainWindow::openMountPointPath( QString m )
{
	if( m_autoOpenFolderOnMount ){

		this->openMountPoint( m ) ;
	}
}

void MainWindow::volumeProperties()
{
	this->disableAll() ;

	QString volume     = m_ui->tableWidget->item( m_ui->tableWidget->currentRow(),0 )->text() ;
	QString volumeType = m_ui->tableWidget->item( m_ui->tableWidget->currentRow(),2 )->text() ;

	QString r = zuluMountTask::volumeProperties( volume,volumeType ).await() ;

	DialogMsg msg( this ) ;

	if( r.isEmpty() ){
		msg.ShowUIOK( tr( "ERROR" ),
			      tr( "Could not get volume properties.\nvolume is not open or was opened by a different user" ) ) ;
	}else{
		int i = r.indexOf( "\n" ) ;
		if( i != -1 ){
			msg.ShowUIVolumeProperties( tr( "Volume Properties" ),r.mid( i + 1 ) ) ;
		}else{
			msg.ShowUIOK( tr( "ERROR" ),
				      tr( "Could not get volume properties.\nvolume is not open or was opened by a different user" ) ) ;
		}
	}

	this->enableAll() ;
}

void MainWindow::setUpShortCuts()
{
	auto ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter ) ;
	keys.append( Qt::Key_Return ) ;
	ac->setShortcuts( keys ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( defaultButton() ) ) ;
	this->addAction( ac ) ;

	auto qa = new QAction( this ) ;
	QList<QKeySequence> z ;
	z.append( Qt::Key_M ) ;
	qa->setShortcuts( z ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbMount() ) ) ;
	this->addAction( qa ) ;

	qa = new QAction( this ) ;
	QList<QKeySequence> p ;
	p.append( Qt::Key_U ) ;
	qa->setShortcuts( p ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbUmount() ) ) ;
	this->addAction( qa ) ;

	qa = new QAction( this ) ;
	QList<QKeySequence> q ;
	q.append( Qt::Key_R ) ;
	qa->setShortcuts( q ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbUpdate() ) ) ;
	this->addAction( qa ) ;

	qa = new QAction( this ) ;
	QList<QKeySequence> e ;
	e.append( Qt::Key_C ) ;
	qa->setShortcuts( e ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbClose() ) ) ;
	this->addAction( qa ) ;
}

void MainWindow::setUpFont()
{
	userfont F( this ) ;
	this->setFont( F.getFont() ) ;
}

void MainWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->hide() ;
}

void MainWindow::slotTrayClicked( QSystemTrayIcon::ActivationReason e )
{
	if( e == QSystemTrayIcon::Trigger ){

		if( this->isVisible() ){

			this->hide() ;
		}else{
			this->show() ;
		}
	}
}

void MainWindow::autoMountToggled( bool opt )
{
	m_autoMount = opt ;
}

void MainWindow::dragEnterEvent( QDragEnterEvent * e )
{
	e->accept() ;
}

void MainWindow::dropEvent( QDropEvent * e )
{
	auto l = e->mimeData()->urls() ;

	for( const auto& it : l ){

		this->showMoungDialog( it.path() ) ;
	}
}

void MainWindow::mount( const volumeEntryProperties& entry )
{
	this->disableAll() ;

	if( entry.encryptedVolume() ){

		auto kd = new keyDialog( this,m_ui->tableWidget,entry ) ;

		connect( kd,SIGNAL( cancel() ),this,SLOT( enableAll() ) ) ;
		connect( kd,SIGNAL( openMountPoint( QString ) ),this,SLOT( openMountPointPath( QString ) ) ) ;

		kd->ShowUI() ;
	}else{
		auto mp = new mountPartition( this,m_ui->tableWidget ) ;

		connect( mp,SIGNAL( cancel() ),this,SLOT( enableAll() ) ) ;
		connect( mp,SIGNAL( openMountPoint( QString ) ),this,SLOT( openMountPointPath( QString ) ) ) ;

		mp->ShowUI( entry ) ;
	}
}

void MainWindow::slotMount()
{
	auto table = m_ui->tableWidget ;
	int row = table->currentRow() ;

	volumeEntryProperties entry( tablewidget::tableRowEntries( table,row ) ) ;

	this->mount( entry ) ;
}

void MainWindow::showMoungDialog( const volumeEntryProperties& v )
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

void MainWindow::showMoungDialog( const QString& volume )
{
	if( !volume.isEmpty() ){

		if( utility::pathPointsToAFile( volume ) ){

			this->showMoungDialog( zuluMountTask::getVolumeProperties( volume ).await() ) ;

		}else if( utility::pathPointsToAFolder( volume ) ){

			QStringList l ;

			l << volume << "Nil" << "encfs" << "Nil" << "Nil" << "Nil" ;

			this->mount( volumeEntryProperties( l ) ) ;
		}
	}
}

void MainWindow::pbMount()
{
	this->disableAll() ;

	QString path = QFileDialog::getOpenFileName( this,tr( "Select An Image File To Mount" ),QDir::homePath() ) ;

	if( path.isEmpty() ){

		this->enableAll() ;
	}else{
		this->showMoungDialog( path ) ;
	}
}

void MainWindow::unlockencfs()
{
	this->disableAll() ;

	QString path = QFileDialog::getExistingDirectory( this,tr( "Select An Encfs Volume Directory" ),QDir::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( path.isEmpty() ){

		this->enableAll() ;
	}else{
		this->showMoungDialog( path ) ;
	}
}

QFont MainWindow::getSystemVolumeFont()
{
	QFont f = this->font() ;
	f.setItalic( !f.italic() ) ;
	f.setBold( !f.bold() ) ;
	return f ;
}

void MainWindow::addEntryToTable( bool systemVolume,const QStringList& l )
{
	if( systemVolume ){
		tablewidget::addRowToTable( m_ui->tableWidget,l,this->getSystemVolumeFont() ) ;
	}else{
		tablewidget::addRowToTable( m_ui->tableWidget,l ) ;
	}
}

void MainWindow::removeEntryFromTable( QString volume )
{
	auto table = m_ui->tableWidget ;

	int r = tablewidget::columnHasEntry( table,volume ) ;

	if( r != -1 ){
		tablewidget::deleteRowFromTable( table,r ) ;
		this->enableAll() ;
	}else{
		this->pbUpdate() ;
	}
}

void MainWindow::volumeMiniProperties( volumeEntryProperties * volumeInfo )
{
	Object_raii( volumeInfo ) ;

	this->disableAll() ;

	if( volumeInfo ){

		this->updateList( *volumeInfo ) ;
		this->enableAll() ;
	}else{
		this->pbUpdate() ;
	}
}

void MainWindow::volumeMiniProperties_0( volumeEntryProperties * volumeInfo )
{
	Object_raii( volumeInfo ) ;

	this->disableAll() ;

	if( volumeInfo && volumeInfo->mounted() ){

		this->updateList( *volumeInfo ) ;
		this->enableAll() ;
	}else{
		this->pbUpdate() ;
	}
}

void MainWindow::updateList( const volumeEntryProperties& entry )
{
	if( entry.isNotEmpty() ){

		auto table = m_ui->tableWidget ;

		int row = tablewidget::columnHasEntry( table,entry.volumeName() ) ;
		if( row == -1 ){
			row = tablewidget::addEmptyRow( table ) ;
		}
		if( entry.isSystem() ){
			tablewidget::updateRowInTable( table,entry.entryList(),row,this->getSystemVolumeFont() ) ;
		}else{
			tablewidget::updateRowInTable( table,entry.entryList(),row,this->font() ) ;
		}
		tablewidget::selectRow( table,row ) ;
	}
}

void MainWindow::pbUmount()
{
	this->disableAll() ;

	int row = m_ui->tableWidget->currentRow() ;

	QString path = m_ui->tableWidget->item( row,0 )->text() ;
	QString type = m_ui->tableWidget->item( row,2 )->text() ;

	if( type == "encfs" ){

		QString m = m_ui->tableWidget->item( row,1 )->text() ;

		if( !zuluMountTask::encfsUnmount( m ).await() ){

			DialogMsg m( this ) ;
			m.ShowUIOK( tr( "ERROR" ),tr( "Failed to unmount encfs volume" ) ) ;
			this->enableAll() ;
		}
	}else{
		auto r = zuluMountTask::unmountVolume( path,type ).await() ;

		if( r.failed() ){

			DialogMsg m( this ) ;
			m.ShowUIOK( tr( "ERROR" ),r.output() ) ;
			this->enableAll() ;
		}
	}
}

void MainWindow::unMountAll()
{
	this->disableAll() ;

	auto table = m_ui->tableWidget ;

	QStringList x = tablewidget::tableColumnEntries( table,1 ) ;
	QStringList y = tablewidget::tableColumnEntries( table,0 ) ;
	QStringList z = tablewidget::tableColumnEntries( table,2 ) ;

	QStringList p ;
	QStringList q ;
	QStringList n ;

	QString a = utility::userName() ;
	QString b = utility::mountPath( QString() ) ;
	QString c = utility::homeMountPath( QString() ) ;

	int k = x.size() ;

	for( int i = 0 ; i < k ; i++ ){

		const QString& e = x.at( i ) ;

		if( e.startsWith( b ) || e.startsWith( c ) ){

			p.append( y.at( i ) ) ;
			q.append( z.at( i ) ) ;
			n.append( e ) ;
		}
	}

	m_removeAllVolumes = true ;

	Task::await( [ & ](){

		if( p.isEmpty() ){

			utility::Task::waitForOneSecond() ;
		}else{
			int r = p.size() ;

			for( int i = 0 ; i < r ; i++ ){

				const QString& e = q.at( i ) ;

				if( e == "encfs" ){

					zuluMountTask::encfsUnmount( n.at( i ) ).get() ;
				}else{
					zuluMountTask::volumeUnmount( p.at( i ),e ) ;
				}

				utility::Task::waitForOneSecond() ;
			}

			utility::Task::waitForTwoSeconds() ;
		}
	} ) ;

	this->enableAll_1() ;
}

void MainWindow::pbUpdate()
{
	this->disableAll() ;

	auto r = zuluMountTask::updateVolumeList().await() ;

	this->updateVolumeList( r ) ;

	this->removeDisappearedEntries( r ) ;
}

void MainWindow::updateVolumeList( const QVector< volumeEntryProperties >& r )
{
	if( r.isEmpty() ){

		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),
			      tr( "Reading partition properties took longer than expected and operation was terminated,click refresh to try again" ) ) ;

		this->enableAll() ;
	}else{
		for( const auto& it : r ){

			if( it.entryisValid() ){
				this->updateList( it ) ;
			}
		}
	}
}

void MainWindow::removeDisappearedEntries( const QVector< volumeEntryProperties >& entries )
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

	QStringList l = tablewidget::tableColumnEntries( table,0 ) ;

	auto _hasNoEntry = [&]( const QString& volume ){

		for( const auto& it : entries ){

			if( it.volumeName() == volume ){

				if( it.fileSystem() == "encfs" ){

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

	QStringList z ;

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
			tablewidget::deleteTableRow( table,e ) ;
		}

		utility::Task::suspendForOneSecond() ;
		_done() ;
	}
}

void MainWindow::slotCurrentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;
}

void MainWindow::disableAll()
{
	m_ui->pbclose->setEnabled( false ) ;
	m_ui->pbmount->setEnabled( false ) ;
	m_ui->pbupdate->setEnabled( false ) ;
	m_ui->tableWidget->setEnabled( false ) ;
	m_ui->pbunlockencfs->setEnabled( false ) ;
}

void MainWindow::enableAll()
{
	if( m_removeAllVolumes ){
		return ;
	}
	m_ui->pbclose->setEnabled( true ) ;
	m_ui->pbupdate->setEnabled( true ) ;
	m_ui->tableWidget->setEnabled( true ) ;
	m_ui->pbmount->setEnabled( true ) ;
	m_ui->tableWidget->setFocus() ;
	m_ui->pbunlockencfs->setEnabled( true ) ;
}

void MainWindow::enableAll_1()
{
	m_removeAllVolumes = false ;
	this->enableAll() ;
}

#define zuluMOUNT_AUTOPATH "/.zuluCrypt/zuluMount-gui.autoMountPartitions"

bool MainWindow::autoMount()
{
	QFile f( QDir::homePath() + QString( zuluMOUNT_AUTOPATH ) ) ;
	return f.exists() ;
}

MainWindow::~MainWindow()
{
	QFile f( QDir::homePath() + QString( zuluMOUNT_AUTOPATH ) ) ;

	if( m_autoMountAction ){
		if( m_autoMountAction->isChecked() ){
			if( !f.exists() ){
				f.open( QIODevice::WriteOnly ) ;
				f.close() ;
			}
		}else{
			f.remove() ;
		}
	}
}

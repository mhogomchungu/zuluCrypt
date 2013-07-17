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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow( int argc,char * argv[],QWidget * parent ) :QWidget( parent ),
	m_autoMountThread( 0 ),m_autoMountAction( 0 ),m_started( false )
{
	m_argc = argc ;
	m_argv = argv ;
	this->processArgumentList() ;
}

void MainWindow::setUpApp()
{
	this->setLocalizationLanguage();
	m_ui = new Ui::MainWindow ;
	m_ui->setupUi(this);

	this->setFixedSize( this->size() ) ;

	m_ui->tableWidget->setColumnWidth( 0,228 );
	//m_ui->tableWidget->setColumnWidth( 1,226 ); //original width with label column visible
	m_ui->tableWidget->setColumnWidth( 1,328 );   // new column with label column hidden
	m_ui->tableWidget->setColumnWidth( 2,98 );
	//m_ui->tableWidget->hideColumn( 2 ) ;
	//m_ui->tableWidget->setColumnWidth( 3,98 ); // hide the label column
	//m_ui->tableWidget->setColumnWidth( 3,0 );    // hide the label column
	m_ui->tableWidget->hideColumn( 3 ) ;
	m_ui->tableWidget->setColumnWidth( 4,87 );
	m_ui->tableWidget->setColumnWidth( 5,87 );

	this->setAcceptDrops( true );
	this->setWindowIcon( QIcon( QString( ":/zuluMount.png" ) ) );

	m_ui->tableWidget->setMouseTracking( true );

	connect( m_ui->tableWidget,SIGNAL( itemEntered( QTableWidgetItem * ) ),this,SLOT( itemEntered( QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->pbmount,SIGNAL( clicked() ),this,SLOT( pbMount() ) ) ;
	connect( m_ui->pbunmount,SIGNAL( clicked() ),this,SLOT( pbUmount() ) ) ;
	connect( m_ui->pbupdate,SIGNAL( clicked()),this,SLOT(pbUpdate() ) ) ;
	connect( m_ui->pbclose,SIGNAL( clicked() ),this,SLOT( pbClose() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;
	connect( m_ui->cbReadOnly,SIGNAL( stateChanged( int ) ),this,SLOT( stateChanged( int ) ) ) ;

	m_ui->cbReadOnly->setVisible( false );

	this->setUpShortCuts();

	this->setUpFont();

	m_trayIcon = new QSystemTrayIcon( this ) ;
	m_trayIcon->setIcon( QIcon( QString( ":/zuluMount.png" ) ) );

	QMenu * trayMenu = new QMenu( this ) ;

	m_autoMountAction = new QAction( this ) ;
	m_autoMount = this->autoMount() ;
	m_autoMountAction->setCheckable( true );
	m_autoMountAction->setChecked( m_autoMount );

	m_autoMountAction->setText( tr( "automount volumes" ) ) ;

	connect( m_autoMountAction,SIGNAL( toggled( bool ) ),this,SLOT( autoMountToggled( bool ) ) ) ;

	trayMenu->addAction( m_autoMountAction ) ;

	QAction * autoOpenFolderOnMount = new QAction( this ) ;
	autoOpenFolderOnMount->setCheckable( true ) ;
	m_autoOpenFolderOnMount = this->autoOpenFolderOnMount() ;
	autoOpenFolderOnMount->setChecked( m_autoOpenFolderOnMount ) ;
	autoOpenFolderOnMount->setText( tr( "auto open mount point" ) ) ;
	connect( autoOpenFolderOnMount,SIGNAL( toggled( bool ) ),this,SLOT( autoOpenFolderOnMount( bool ) ) ) ;

	trayMenu->addAction( autoOpenFolderOnMount ) ;

	trayMenu->addAction( tr( "quit" ),this,SLOT( slotCloseApplication() ) );
	m_trayIcon->setContextMenu( trayMenu );

	connect( m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),this,SLOT( slotTrayClicked( QSystemTrayIcon::ActivationReason ) ) );

	m_trayIcon->show();

	QString dirPath = QDir::homePath() + QString( "/.zuluCrypt/" ) ;
	QDir dir( dirPath ) ;

	if( !dir.exists() ){
		dir.mkdir( dirPath ) ;
	}

	m_ui->pbunmount->setVisible( false );

	managepartitionthread * part = new managepartitionthread() ;

	this->disableAll();

	connect( part,SIGNAL( signalMountedList( QStringList,QStringList ) ),this,SLOT( slotMountedList( QStringList,QStringList ) ) ) ;
	connect( part,SIGNAL( done() ),this,SLOT( openVolumeFromArgumentList() ) ) ;
	connect( part,SIGNAL( done() ),this,SLOT( started() ) ) ;

	part->startAction( managepartitionthread::Update ) ;

	this->startAutoMonitor();
}

void MainWindow::setLocalizationLanguage()
{
	QTranslator * translator = new QTranslator( this ) ;
	QString pgr      = QString( "zuluMount-gui" ) ;
	QString lang     = utility::localizationLanguage( pgr ) ;
	QString langPath = utility::localizationLanguagePath( pgr ) ;

	QByteArray r = lang.toAscii() ;

	QByteArray e( "en_US" ) ;
	if( e == r ){
		/*
		 * english_US language,its the default and hence dont load anything
		 */
	}else{
		translator->load( r.constData(),langPath ) ;
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
		f.close();
	}
}

bool MainWindow::autoOpenFolderOnMount( void )
{
	QString x = QDir::homePath() + QString( zuluMOUNT_AUTO_OPEN_FOLDER ) ;
	return QFile::exists( x ) == false ;
}

void MainWindow::startAutoMonitor()
{
	m_autoMountThread = new auto_mount( this ) ;
	connect( m_autoMountThread,SIGNAL( stopped() ),this,SLOT( close() ) ) ;
	m_autoMountThread->start();
}

void MainWindow::started( void )
{
	m_started = true ;
}

void MainWindow::showEvent( QShowEvent * e )
{
	if( m_startHidden && !m_started ){
		e->ignore();
		this->hide();
	}
}

void MainWindow::autoMountVolumeSystemInfo( QStringList l )
{
	//QString s = QString( "system device added: device=%1" ).arg( l.at( 0 ) ) ;
	//qDebug() << s ;
	if( l.size() == 0 ){
		return ;
	}
	if( l.at( 0 ).size() == strlen( "/dev/sdX" ) && l.at( 2 ) == QString( "Nil" ) ){
		/*
		 * root device with no file system,dont show them.This will be a bug if a user just put a plain volume
		 * or a truecrypt volume without first partitio the drive.
		 */
		return ;
	}

	this->addEntryToTable( true,l ) ;
}

void MainWindow::autoMountAddToTable( QString entry )
{
	this->volumeMiniProperties( m_ui->tableWidget,entry,QString( "Nil" ) ) ;
}

void MainWindow::autoMountVolumeInfo( QStringList l )
{
	if( l.size() == 0 ){
		return ;
	}
	if( l.at( 4 ) == QString( "1.0 KB" ) ){
		return ;
	}

	QString dev = l.at( 0 ) ;
	QString type = l.at( 2 ) ;

	if( dev.size() == strlen( "/dev/sdX" ) && type == QString( "Nil" ) ){
		/*
		 * root device with no file system,dont show them.This will be a bug if a user just put a plain volume
		 * or a truecrypt volume without first partitio the drive.
		 */
		return ;
	}
	if( type.startsWith( QString( "crypto" ) ) || type == QString( "Nil" ) ){
		this->addEntryToTable( false,l );
	}else{
		if( m_autoMount ){
			mountPartition * mp = new mountPartition( this,m_ui->tableWidget,m_folderOpener,m_autoOpenFolderOnMount ) ;
			connect( mp,SIGNAL( autoMountComplete() ),mp,SLOT( deleteLater() ) ) ;
			connect( mp,SIGNAL( autoMountComplete() ),this,SLOT( enableAll() ) ) ;
			mp->AutoMount( l );
		}else{
			this->addEntryToTable( false,l );
		}
	}
}

void MainWindow::deviceRemoved( QString dev )
{
	int row = tablewidget::columnHasEntry( m_ui->tableWidget,0,dev ) ;
	if( row != -1 ){
		tablewidget::deleteRowFromTable( m_ui->tableWidget,row ) ;
		/*
		* see if a user just removed the device without properly closing it/unmounting it
		* and try to do so for them
		*/
		managepartitionthread * part = new managepartitionthread() ;
		part->setDevice( dev );
		part->startAction( managepartitionthread::checkUnMount ) ;
	}
}

void MainWindow::itemEntered( QTableWidgetItem * item )
{
	int row = item->row() ;
	QTableWidget * table = item->tableWidget() ;
	QString m_point = table->item( row,1 )->text() ;

	QString x = table->item( row,3 )->text() ;
	QString z ;
	QString y ;

	if( m_point == QString( "/" ) ){
		/*
		 * we dont check if root path is publicly shared because the path it will produce (/run/media/shared/)
		 * will always return true,a solution is to examine /proc/self/mountinfo and thats work for another day
		 */
		if( x == QString( "Nil" ) ){
			x.clear();
		}
		z += tr( "LABEL=\"%1\"" ).arg( x ) ;

	}else if( m_point == QString( "Nil" ) ){
		/*
		 * volume is not mounted,cant know its LABEL value
		 */
		x.clear();
		z += tr( "LABEL=\"%1\"" ).arg( x ) ;
	}else{
		if( x == QString( "Nil" ) ){
			x.clear();
		}
		y = utility::shareMountPointToolTip( m_point ) ;
		if( y.isEmpty() ){
			z += tr( "LABEL=\"%1\"" ).arg( x ) ;
		}else{
			z += tr( "LABEL=\"%1\"\n%2" ).arg( x ).arg( y ) ;
		}
	}
	item->setToolTip( z );
}

void MainWindow::processArgumentList()
{
	int c ;
	m_startHidden = false ;
	m_device = QString( "" ) ;
	m_folderOpener = QString( "xdg-open" ) ;

	while( ( c = getopt( m_argc,m_argv,"ed:m:" ) ) != -1 ) {
		switch( c ){
			case 'e' : m_startHidden  = true               ; break ;
			case 'd' : m_device       = QString( optarg )  ; break ;
			case 'm' : m_folderOpener = QString( optarg )  ; break ;
		}
	}
}

void MainWindow::raiseWindow()
{
	this->setVisible( true );
	this->raise();
	this->show();
	this->setWindowState( Qt::WindowActive ) ;
}

void MainWindow::raiseWindow( QString device )
{
	this->setVisible( true );
	this->raise();
	this->show();
	this->setWindowState( Qt::WindowActive ) ;
	m_device = device ;
	this->openVolumeFromArgumentList();
}

void MainWindow::start()
{
	QString sockpath = QString( "zuluMount-gui.socket" ) ;
	oneinstance * instance = new oneinstance( this,sockpath,"raiseWindow",m_device ) ;
	if( !instance->instanceExist() ){
		connect( instance,SIGNAL( raise() ),this,SLOT( raiseWindow() ) ) ;
		connect( instance,SIGNAL( raiseWithDevice( QString ) ),this,SLOT( raiseWindow( QString ) ) ) ;
		this->setUpApp();
	}
}

void MainWindow::pbClose()
{
	this->slotCloseApplication();
}

void MainWindow::close()
{
	QCoreApplication::quit();
}

void MainWindow::slotCloseApplication()
{
	m_autoMountThread->stop();
}

void MainWindow::showContextMenu( QTableWidgetItem * item,bool itemClicked )
{
	QMenu m ;

	m.setFont( this->font() );

	QString mt = m_ui->tableWidget->item( item->row(),1 )->text() ;
	QString type = m_ui->tableWidget->item( item->row(),2 )->text() ;

	if( mt == QString( "Nil" ) ){
		connect( m.addAction( tr( "mount" ) ),SIGNAL( triggered() ),this,SLOT( slotMount() ) ) ;
	}else{
		QString mp = QString( "/run/media/private/%1/" ).arg( utility::userName() ) ;
		if( mt.startsWith( mp ) ){
			connect( m.addAction( tr( "unmount" ) ),SIGNAL( triggered() ),this,SLOT( pbUmount() ) ) ;
			if( type.startsWith( QString( "crypto" ) ) ) {
				m.addSeparator() ;
				connect( m.addAction( tr( "properties" ) ),SIGNAL( triggered() ),this,SLOT( volumeProperties() ) ) ;
			}
			m.addSeparator() ;
			m_sharedFolderPath = utility::sharedMountPointPath( mt ) ;
			if( m_sharedFolderPath.isEmpty() ){
				connect( m.addAction( tr( "open folder" ) ),SIGNAL( triggered() ),this,SLOT( slotOpenFolder() ) ) ;
			}else{
				connect( m.addAction( tr( "open private folder" ) ),SIGNAL( triggered() ),this,SLOT( slotOpenFolder() ) ) ;
				connect( m.addAction( tr( "open shared folder" ) ),SIGNAL( triggered() ),this,SLOT( slotOpenSharedFolder() ) ) ;
			}
		}else{
			m_sharedFolderPath = utility::sharedMountPointPath( mt ) ;
			if( m_sharedFolderPath.isEmpty() ){
				if( mt.startsWith( QString( "/run/media/private/" ) ) ){
					m.addAction( tr( "no available options for this volume" ) ) ;
				}else{
					connect( m.addAction( tr( "open folder" ) ),SIGNAL( triggered() ),this,SLOT( slotOpenFolder() ) ) ;
				}
			}else{
				connect( m.addAction( tr( "open shared folder" ) ),SIGNAL( triggered() ),this,SLOT( slotOpenSharedFolder() ) ) ;
			}
		}
	}

	m.addSeparator() ;
	m.addAction( tr( "close menu" ) ) ;

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

	if( mt == QString( "Nil" ) ){
		this->slotMount();
	}else{
		this->showContextMenu( m_ui->tableWidget->currentItem(),false );
	}
}

void MainWindow::fileManagerOpenStatus( int exitCode, int exitStatus,int startError )
{
	Q_UNUSED( startError ) ;
	if( exitCode != 0 || exitStatus != 0 ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "warning" ),tr( "could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_folderOpener ) );
	}
}

void MainWindow::slotOpenSharedFolder()
{
	openmountpointinfilemanager * ofm = new openmountpointinfilemanager( m_folderOpener,m_sharedFolderPath ) ;
	connect( ofm,SIGNAL( errorStatus( int,int,int ) ),this,SLOT( fileManagerOpenStatus( int,int,int ) ) ) ;
	ofm->start();
}

void MainWindow::slotOpenFolder()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	QString path = m_ui->tableWidget->item( item->row(),1 )->text() ;
	openmountpointinfilemanager * ofm = new openmountpointinfilemanager( m_folderOpener,path ) ;
	connect( ofm,SIGNAL( errorStatus( int,int,int ) ),this,SLOT( fileManagerOpenStatus( int,int,int ) ) ) ;
	ofm->start();
}

void MainWindow::volumeProperties()
{
	this->disableAll();
	managepartitionthread * part = new managepartitionthread() ;
	part->setDevice( m_ui->tableWidget->item( m_ui->tableWidget->currentRow(),0 )->text() );
	connect( part,SIGNAL( signalProperties( QString ) ),this,SLOT( volumeProperties( QString ) ) ) ;

	part->startAction( managepartitionthread::VolumeProperties ) ;
}

void MainWindow::volumeProperties( QString properties )
{
	DialogMsg msg( this ) ;

	if( properties.isEmpty() ){
		msg.ShowUIOK( tr( "ERROR" ),tr( "could not get volume properties.\nvolume is not open or was opened by a different user" ) ) ;
	}else{
		int i = properties.indexOf( "\n" ) ;
		if( i != -1 ){
			msg.ShowUIVolumeProperties( tr( "volume properties" ),properties.mid( i + 1 ) ) ;
		}else{
			msg.ShowUIOK( tr( "ERROR" ),tr( "could not get volume properties.\nvolume is not open or was opened by a different user" ) ) ;
		}
	}
	this->enableAll();
}

void MainWindow::setUpShortCuts()
{
	QAction * ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter );
	keys.append( Qt::Key_Return );
	ac->setShortcuts( keys ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( defaultButton() ) ) ;
	this->addAction( ac );

	QAction * qa = new QAction( this ) ;
	QList<QKeySequence> z ;
	z.append( Qt::Key_M );
	qa->setShortcuts( z ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbMount() ) ) ;
	this->addAction( qa );

	qa = new QAction( this ) ;
	QList<QKeySequence> p ;
	p.append( Qt::Key_U );
	qa->setShortcuts( p ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbUmount() ) ) ;
	this->addAction( qa ) ;

	qa = new QAction( this ) ;
	QList<QKeySequence> q ;
	q.append( Qt::Key_R );
	qa->setShortcuts( q ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbUpdate() ) );
	this->addAction( qa ) ;

	qa = new QAction( this ) ;
	QList<QKeySequence> d ;
	d.append( Qt::Key_O );
	qa->setShortcuts( d ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( slotcbReadOnly() ) );
	this->addAction( qa ) ;

	qa = new QAction( this ) ;
	QList<QKeySequence> e ;
	e.append( Qt::Key_C );
	qa->setShortcuts( e ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( slotCloseApplication() ) );
	this->addAction( qa ) ;
}

void MainWindow::setUpFont()
{
	userfont F( this ) ;
	this->setFont( F.getFont() ) ;
}

void MainWindow::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->hide();
}

void MainWindow::slotTrayClicked( QSystemTrayIcon::ActivationReason e )
{
	if( e == QSystemTrayIcon::Trigger ){
		if( this->isVisible() ){
			this->hide();
		}else{
			this->show();
		}
	}
}

void MainWindow::autoMountToggled( bool opt )
{
	m_autoMount = opt ;
}

void MainWindow::dragEnterEvent( QDragEnterEvent * e )
{
	e->accept();
}

void MainWindow::dropEvent( QDropEvent * e )
{
	const QMimeData * m = e->mimeData() ;
	QList<QUrl> l = m->urls() ;
	int j = l.size() ;

	for( int i = 0 ; i < j ; i++ ){
		m_device = l.at( i ).path() ;
		if( utility::pathPointsToAFile( m_device ) ){
			managepartitionthread * m = new managepartitionthread() ;
			connect( m,SIGNAL( getVolumeInfo( QStringList ) ),this,SLOT( showMoungDialog( QStringList ) ) ) ;
			m->setDevice( m_device );
			m->startAction( managepartitionthread::VolumeType ) ;
		}
	}
}

void MainWindow::slotcbReadOnly()
{
	if( m_ui->cbReadOnly->isChecked() ){
		m_ui->cbReadOnly->setChecked( false );
	}else{
		m_ui->cbReadOnly->setChecked( true );
	}
}

void MainWindow::stateChanged( int state )
{
	m_ui->cbReadOnly->setEnabled( false );
	m_ui->cbReadOnly->setChecked( openvolumereadonly::setOption( this,state,QString( "zuluMount-gui" ) ) );
	m_ui->cbReadOnly->setEnabled( true );
}

void MainWindow::mount( QString type,QString device,QString label )
{
	this->disableAll();
	if( type.startsWith( QString( "crypto" ) ) || type == QString( "Nil" ) ){
		keyDialog * kd = new keyDialog( this,m_ui->tableWidget,device,type,m_folderOpener,m_autoOpenFolderOnMount ) ;
		connect( kd,SIGNAL( hideUISignal() ),kd,SLOT( deleteLater() ) ) ;
		connect( kd,SIGNAL( hideUISignal() ),this,SLOT( enableAll() ) ) ;
		kd->ShowUI();
	}else{
		mountPartition * mp = new mountPartition( this,m_ui->tableWidget,m_folderOpener,m_autoOpenFolderOnMount ) ;
		connect( mp,SIGNAL( hideUISignal() ),mp,SLOT( deleteLater() ) ) ;
		connect( mp,SIGNAL( hideUISignal() ),this,SLOT( enableAll() ) ) ;
		mp->ShowUI( device,label );
	}
}

void MainWindow::openVolumeFromArgumentList()
{
	if( !m_device.isEmpty() ){
		managepartitionthread * m = new managepartitionthread() ;
		connect( m,SIGNAL( getVolumeInfo( QStringList ) ),this,SLOT( showMoungDialog( QStringList ) ) ) ;
		m->setDevice( m_device );
		m->startAction( managepartitionthread::VolumeType ) ;
	}
}

void MainWindow::slotMount()
{
	QTableWidget * table = m_ui->tableWidget ;
	int row = table->currentRow() ;
	QString device = table->item( row,0 )->text() ;
	QString type   = table->item( row,2 )->text() ;
	QString label  = table->item( row,3 )->text() ;
	this->mount( type,device,label );
}

void MainWindow::showMoungDialog( QStringList l )
{
	if( l.size() >= 4  ){
		this->mount( l.at( 2 ),l.at( 0 ),l.at( 3 ) );
	}else{
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "permission to access the volume was denied\nor\nthe volume is not supported\n(LVM/MDRAID signatures found)" ) ) ;
		this->enableAll();
	}
}

void MainWindow::pbMount()
{
	this->disableAll();

	QString path = QFileDialog::getOpenFileName( this,tr( "select an image file to mount" ),QDir::homePath() ) ;
	if( path.isEmpty() ){
		this->enableAll();
	}else{
		m_device = path ;
		managepartitionthread * m = new managepartitionthread() ;
		connect( m,SIGNAL( getVolumeInfo( QStringList ) ),this,SLOT( showMoungDialog( QStringList ) ) ) ;
		m->setDevice( m_device );
		m->startAction( managepartitionthread::VolumeType ) ;
	}
}

void MainWindow::addEntryToTable( bool b,QStringList l )
{
	QFont f = this->font() ;

	f.setItalic( !f.italic() );
	f.setBold( !f.bold() );

	QString x = l.at( 5 ) ;
	x = x.remove( QChar( '\n' ) ) ;
	l.replace( 5,x );

	if( b ){
		tablewidget::addRowToTable( m_ui->tableWidget,l,f ) ;
	}else{
		tablewidget::addRowToTable( m_ui->tableWidget,l ) ;
	}
}

void MainWindow::volumeMiniProperties( QTableWidget * table,QString p,QString mountPointPath )
{
	QStringList l ;
	QString device ;
	QString fileSystem ;
	QString total ;
	QString perc ;
	QString label ;

	if( p.isEmpty() ){
		device = QString( "Nil" ) ;
		fileSystem = QString( "Nil" ) ;
		total = QString( "0" ) ;
		perc  = QString( "0%" );
		label = QString( "Nil" ) ;
	}else{
		l = p.split( "\t" ) ;
		device = l.at( 0 ) ;
		fileSystem = l.at( 2 ) ;
		int index = fileSystem.indexOf( QString( "/" ) ) ;
		if( index != -1 ){
			fileSystem = fileSystem.replace( QString( "/" ),QString( "\n(" ) ) + QString( ")" ) ;
		}
		label = l.at( 3 ) ;
		total = l.at( 4 ) ;
		perc = l.at( 5 ) ;
		perc.remove( QChar( '\n' ) ) ;
	}

	if( tablewidget::columnHasEntry( table,0,device ) == -1 ){
		tablewidget::addEmptyRow( table ) ;
	}

	int row = table->currentRow() ;
	tablewidget::setText( table,row,0,device ) ;
	tablewidget::setText( table,row,1,mountPointPath ) ;
	tablewidget::setText( table,row,2,fileSystem ) ;
	tablewidget::setText( table,row,3,label ) ;
	tablewidget::setText( table,row,4,total ) ;
	tablewidget::setText( table,row,5,perc ) ;
}

void MainWindow::pbUmount()
{
	this->disableAll();

	int row = m_ui->tableWidget->currentRow() ;

	QString path = m_ui->tableWidget->item( row,0 )->text() ;
	QString type = m_ui->tableWidget->item( row,2 )->text() ;

	managepartitionthread * part = new managepartitionthread() ;

	part->setDevice( path );
	part->setType( type );

	connect( part,SIGNAL( signalUnmountComplete( int,QString,QString ) ),this,SLOT( slotUnmountComplete( int,QString,QString ) ) ) ;

	part->startAction( managepartitionthread::Unmount ) ;
}

void MainWindow::pbUpdate()
{
	this->disableAll();

	while( m_ui->tableWidget->rowCount() ){
		m_ui->tableWidget->removeRow( 0 ) ;
	}
	managepartitionthread * part = new managepartitionthread() ;

	m_ui->tableWidget->setEnabled( false );
	connect( part,SIGNAL( signalMountedList( QStringList,QStringList ) ),this,SLOT( slotMountedList( QStringList,QStringList ) ) ) ;

	part->startAction( managepartitionthread::Update ) ;
}

void MainWindow::slotMountedList( QStringList list,QStringList sys )
{
	if( list.isEmpty() || sys.isEmpty() ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "reading partition properties took longer than expected and operation was terminated,click refresh to try again" ) );
		this->enableAll();
		return ;
	}

	QTableWidget * table = m_ui->tableWidget ;

	QStringList entries ;

	int j = list.size() - 1 ;

	QFont f = this->font() ;

	f.setItalic( !f.italic() );
	f.setBold( !f.bold() );

	QString opt ;
	QString fs ;
	QString x = QString( "/run/media/private/" ) + utility::userName() ;
	QString y ;

	int index ;
	for( int i = 0 ; i < j ; i++ ){
		entries = list.at( i ).split( '\t' ) ;
		if( entries.size() < 6 ){
			continue ;
		}
		if( entries.at( 0 ) == QString( "/dev/md/md-device-map" ) ){
			/*
			 * dont display this path as it is some sort of an accounting text file
			 * in some distributions
			 */
			continue ;
		}

		fs =  entries.at( 2 ) ;

		/*
		 * MDRAID partitions have "linux_raid_member" as their file system
		 * LVM partitions have "LVM2_member" as their file system
		 *
		 * we are not showing these partitions since we dont support them
		 */
		if( fs == QString( "swap" ) || fs.contains( QString( "member" ) ) ){
			continue ;
		}

		index = fs.indexOf( QString( "/" ) ) ;
		if( index != -1 ){
			fs = fs.replace( QString( "/" ),QString( "\n(" ) ) + QString( ")" ) ;
			entries.replace( 2,fs ) ;
		}

		y = entries.at( 1 ) ;

		if( y.startsWith( QString( "/run/media/private/" ) ) ){
			if( !y.startsWith( x ) ){
				/*
				 * dont show volumes mounted by other users
				 */
				//continue ;
			}
		}

		if( y.startsWith( QString( "/run/media/public/" ) ) ){
			/*
			 * dont show mirror mounts
			 */
			continue ;
		}
		opt = entries.at( 4 ) ;
		if( opt == QString( "Nil" ) || opt == QString( "1.0 KB" ) ){
			continue ;
		}
		if( sys.contains( entries.at( 0 ) ) ){
			tablewidget::addRowToTable( table,entries,f ) ;
		}else{
			tablewidget::addRowToTable( table,entries ) ;
		}
	}

	this->enableAll();
}

void MainWindow::slotUnmountComplete( int status,QString msg,QString deviceSize )
{
	if( status ){
		DialogMsg m( this ) ;
		m.ShowUIOK( tr( "ERROR" ),msg );
	}else{
		QTableWidget * table = m_ui->tableWidget ;

		int row = table->currentRow() ;

		QString type = table->item( row,2 )->text() ;
		QString device = table->item( row,0 )->text() ;

		if( device.startsWith( QString( "/dev/" ) ) || device.startsWith( "UUID=" ) ){
			QString nil = QString( "Nil" ) ;
			table->item( row,1 )->setText( QString( "Nil" ) );

			if( type.startsWith( QString( "crypto_LUKS" ) ) ){
				table->item( row,3 )->setText( nil );
				table->item( row,2 )->setText( QString( "crypto_LUKS" ) );
			}else if( type.startsWith( QString( "crypto_PLAIN" ) ) ){
				table->item( row,3 )->setText( nil );
				table->item( row,2 )->setText( nil );
			}else if( type.startsWith( QString( "crypto_TCRYPT" ) ) ){
				table->item( row,3 )->setText( nil );
				table->item( row,2 )->setText( nil );
			}

			table->item( row,4 )->setText( deviceSize );
			table->item( row,5 )->setText( nil );
		}else{
			tablewidget::deleteRowFromTable( m_ui->tableWidget,row ) ;
		}
	}

	this->enableAll();
}

void MainWindow::slotCurrentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;
}

void MainWindow::disableAll()
{
	m_ui->cbReadOnly->setEnabled( false );
	m_ui->pbclose->setEnabled( false );
	m_ui->pbmount->setEnabled( false );
	m_ui->pbupdate->setEnabled( false );
	m_ui->tableWidget->setEnabled( false );
	m_ui->pbunmount->setEnabled( false );
}

void MainWindow::enableAll()
{
	m_ui->cbReadOnly->setEnabled( true );
	m_ui->pbclose->setEnabled( true );
	m_ui->pbupdate->setEnabled( true );
	m_ui->tableWidget->setEnabled( true );
	m_ui->pbmount->setEnabled( true );
	m_ui->tableWidget->setFocus();
}

#define zuluMOUNT_AUTOPATH "/.zuluCrypt/zuluMount-gui.autoMountPartitions"

bool MainWindow::autoMount()
{
	QFile f( QDir::homePath() + QString( zuluMOUNT_AUTOPATH ) );
	return f.exists() ;
}

MainWindow::~MainWindow()
{
	QFile f( QDir::homePath() + QString( zuluMOUNT_AUTOPATH ) );

	if( m_autoMountAction ){
		if( m_autoMountAction->isChecked() ){
			if( !f.exists() ){
				f.open( QIODevice::WriteOnly ) ;
				f.close();
			}
		}else{
			f.remove() ;
		}
	}
}

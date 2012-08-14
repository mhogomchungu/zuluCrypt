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
MainWindow::MainWindow( QWidget * parent ) :
	QMainWindow( parent ),
	m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);

	this->setFixedSize( this->size() ) ;

	m_ui->tableWidget->setColumnWidth( 0,140 );
	m_ui->tableWidget->setColumnWidth( 1,140 );
	m_ui->tableWidget->setColumnWidth( 2,100 );
	m_ui->tableWidget->setColumnWidth( 3,100 );
	m_ui->tableWidget->setColumnWidth( 4,90 );
	m_ui->tableWidget->setColumnWidth( 5,90 );
	m_ui->tableWidget->setColumnWidth( 6,90 );

	this->setWindowIcon( QIcon( QString( ":/zuluMount.png" ) ) );

	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->pbmount,SIGNAL( clicked() ),this,SLOT( pbMount() ) ) ;
	connect( m_ui->pbunmount,SIGNAL( clicked() ),this,SLOT( pbUmount() ) ) ;
	connect( m_ui->pbupdate,SIGNAL( clicked()),this,SLOT(pbUpdate() ) ) ;
	connect( m_ui->pbclose,SIGNAL( clicked() ),this,SLOT( pbClose() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;

	this->setUpShortCuts();

	this->setUpFont();

	QSystemTrayIcon * tray_icon = new QSystemTrayIcon( this ) ;
	tray_icon->setIcon( QIcon( QString( ":/zuluMount.png" ) ) );

	QMenu * trayMenu = new QMenu( this ) ;
	trayMenu->addAction( tr( "quit" ),this,SLOT( slotCloseApplication() ) );
	tray_icon->setContextMenu( trayMenu );

	connect( tray_icon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),this,SLOT( slotTrayClicked( QSystemTrayIcon::ActivationReason ) ) );

	tray_icon->show();

	managepartitionthread * part = new managepartitionthread() ;

	this->disableAll();

	connect( part,SIGNAL( signalMountedList( QStringList,QStringList ) ),this,SLOT( slotMountedList( QStringList,QStringList ) ) ) ;

	part->startAction( QString( "update" ) ) ;

	m_working = false ;
}

void MainWindow::itemClicked( QTableWidgetItem * item )
{
	QMenu m ;
	m.setFont( this->font() );
	QString mt = m_ui->tableWidget->item( item->row(),1 )->text() ;

	if( mt == QString( "Nil" ) )
		connect( m.addAction( tr( "mount" ) ),SIGNAL( triggered() ),this,SLOT( pbMount() ) ) ;
	else
		connect( m.addAction( tr( "unmount" ) ),SIGNAL( triggered() ),this,SLOT( pbUmount() ) ) ;
	m.exec( QCursor::pos() ) ;
}

void MainWindow::setUpShortCuts()
{
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

	qa = new QAction( this ) ;
	QList<QKeySequence> n ;
	n.append( Qt::Key_Space );
	//n.append( Qt::Key_Enter );
	qa->setShortcuts( n ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( enterKeyPressed() ) );
	this->addAction( qa ) ;
}

void MainWindow::enterKeyPressed()
{
	int row = m_ui->tableWidget->currentRow() ;
	QString mt = m_ui->tableWidget->item( row,1 )->text() ;

	if( mt == QString( "Nil" ) )
		this->pbMount();
	else
		this->pbUmount();
}

void MainWindow::setUpFont()
{
	userfont F( this ) ;
	QFont f( F.getFont() ) ;

	m_ui->cbReadOnly->setFont( f );
	m_ui->centralWidget->setFont( f );
	m_ui->pbclose->setFont( f );
	m_ui->pbmount->setFont( f );
	m_ui->pbunmount->setFont( f );
	m_ui->pbupdate->setFont( f );
	m_ui->tableWidget->setFont( f );
}

void MainWindow::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->hide();
}

void MainWindow::pbClose()
{
	this->slotCloseApplication();
}

void MainWindow::slotCloseApplication()
{
	if( m_working == false )
		QCoreApplication::quit();
}

void MainWindow::slotTrayClicked( QSystemTrayIcon::ActivationReason e )
{
	if( e == QSystemTrayIcon::Trigger ){
		if( this->isVisible() == true )
			this->hide();
		else
			this->show();
	}
}

void MainWindow::slotcbReadOnly()
{
	if( m_ui->cbReadOnly->isChecked() )
		m_ui->cbReadOnly->setChecked( false );
	else
		m_ui->cbReadOnly->setChecked( true );
}

void MainWindow::pbMount()
{
	this->disableAll();

	int row = m_ui->tableWidget->currentRow() ;
	QString type = m_ui->tableWidget->item( row,2 )->text()  ;
	QString path = m_ui->tableWidget->item( row,0 )->text() ;

	QString mode ;
	if( m_ui->cbReadOnly->isChecked() )
		mode = QString( "ro ") ;
	else
		mode = QString( "rw" ) ;

	if( type == QString( "crypto_LUKS" ) ){

		keyDialog * kd = new keyDialog( this,path,mode ) ;
		connect( kd,SIGNAL( hideUISignal() ),kd,SLOT( deleteLater() ) ) ;
		connect( kd,SIGNAL( hideUISignal() ),this,SLOT( enableAll() ) ) ;
		connect( kd,SIGNAL( updatePartitions() ),this,SLOT( pbUpdate() ) ) ;
		kd->ShowUI();
	}else{
		managepartitionthread * part = new managepartitionthread() ;
		part->setDevice( path );
		part->setMode( mode );
		m_ui->tableWidget->setEnabled( false );
		connect( part,SIGNAL( signalMountComplete( int,QString ) ),this,SLOT( slotMountComplete(int,QString ) ) ) ;

		part->startAction( QString( "mount" ) ) ;
	}
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

	connect( part,SIGNAL( signalUnmountComplete( int,QString ) ),this,SLOT( slotUnmountComplete(int,QString ) ) ) ;

	part->startAction( QString( "umount" ) ) ;
}

void MainWindow::pbUpdate()
{
	this->disableAll();

	while( m_ui->tableWidget->rowCount() )
		m_ui->tableWidget->removeRow( 0 ) ;

	managepartitionthread * part = new managepartitionthread() ;

	m_ui->tableWidget->setEnabled( false );
	connect( part,SIGNAL( signalMountedList( QStringList,QStringList ) ),this,SLOT( slotMountedList( QStringList,QStringList ) ) ) ;

	part->startAction( QString( "update" ) ) ;
}

void MainWindow::slotMountedList( QStringList list,QStringList sys )
{
	QTableWidgetItem * item ;
	QTableWidget * table = m_ui->tableWidget ;

	QStringList entries ;

	int j = list.size() - 1 ;
	int row ;
	int col = table->columnCount() ;

	QFont f = this->font() ;

	if( f.italic() )
		f.setItalic( false );
	else
		f.setItalic( true );

	int p ;
	
	for( int i = 0 ; i < j ; i++ ){

		row = table->rowCount() ;
		table->insertRow( row ) ;
		entries = list.at( i ).split( '\t' ) ;

		if( sys.contains( entries.at( 0 ) ) ){
			for( p = 0 ; p < col ; p++ ){
				item = new QTableWidgetItem( entries.at( p ) ) ;
				item->setTextAlignment( Qt::AlignCenter ) ;
				item->setFont( f );
				table->setItem( row,p,item );
			}
		}else{
			for( p = 0 ; p < col ; p++ ){
				item = new QTableWidgetItem( entries.at( p ) ) ;
				item->setTextAlignment( Qt::AlignCenter ) ;
				table->setItem( row,p,item );
			}
		}

		table->setCurrentCell( row,4 ) ;
	}

	this->enableAll();
}

void MainWindow::slotMountComplete( int status,QString msg )
{
	emit result( status,msg );

	if( status == 0 ){
		this->pbUpdate();
	}else{
		DialogMsg m( this ) ;
		m.ShowUIOK( QString( "ERROR" ),msg );
		this->enableAll();
	}
}

void MainWindow::slotUnmountComplete( int status,QString msg )
{
	if( status == 0 ){
		this->pbUpdate();
	}else{
		DialogMsg m( this ) ;
		m.ShowUIOK( QString( "ERROR" ),msg );
		this->enableAll();
	}
}

void MainWindow::slotCurrentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	if( current == 0 )
		return ;

	if( previous == 0 )
		return ;

	QTableWidget * table = m_ui->tableWidget ;

	if( current->row() == previous->row() ){
		table->setCurrentCell( current->row(),4 ) ;
		return ;
	}

	int prow = previous->row() ;
	int crow = current->row() ;
	int col = table->columnCount() ;

	for( int i = 0 ; i < col ; i++ ){
		table->item( prow,i )->setSelected( false );
		table->item( crow,i )->setSelected( true );
	}
}

void MainWindow::disableAll()
{
	m_ui->cbReadOnly->setEnabled( false );
	m_ui->pbclose->setEnabled( false );
	m_ui->pbmount->setEnabled( false );
	m_ui->pbupdate->setEnabled( false );
	m_ui->tableWidget->setEnabled( false );
	m_ui->pbunmount->setEnabled( false );
	m_working = true ;
}

void MainWindow::enableAll()
{
	m_ui->cbReadOnly->setEnabled( true );
	m_ui->pbclose->setEnabled( true );
	m_ui->pbmount->setEnabled( true );
	m_ui->pbupdate->setEnabled( true );
	m_ui->tableWidget->setEnabled( true );
	m_ui->tableWidget->setFocus();
	m_ui->pbunmount->setEnabled( true );
	m_working = false ;
}

MainWindow::~MainWindow()
{
	delete m_ui;
}

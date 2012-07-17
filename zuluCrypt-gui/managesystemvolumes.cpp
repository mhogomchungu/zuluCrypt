#include "managesystemvolumes.h"
#include "ui_managesystemvolumes.h"

#include <QDebug>

manageSystemVolumes::manageSystemVolumes( QWidget * parent ) :
        QDialog(parent),
	m_ui(new Ui::manageSystemVolumes)
{
	m_ui->setupUi(this);

	this->setFont( parent->font() );
	this->setFixedSize( this->size() );
	this->setWindowFlags( Qt::Window | Qt::Dialog );

	m_ui->tableWidget->setColumnWidth( 0,505 );

	m_ui->pbPartition->setIcon( QIcon( QString( ":/partition.png" ) ) );
	m_ui->pbFile->setIcon( QIcon( QString( ":/file.png" ) ) );

	m_ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter );
	keys.append( Qt::Key_Return );
	keys.append( Qt::Key_Menu );
	m_ac->setShortcuts( keys ) ;
	this->addAction( m_ac );

	connect( m_ac,SIGNAL( triggered() ),this,SLOT( contextMenu() ) ) ;
	connect( m_ui->pbAdd,SIGNAL( clicked() ),this,SLOT( pbAdd() ) ) ;
	connect( m_ui->pbDone,SIGNAL( clicked() ),this,SLOT( pbDone() ) ) ;
	connect( m_ui->pbFile,SIGNAL( clicked() ),this,SLOT( pbFile() ) ) ;
	connect( m_ui->pbPartition,SIGNAL( clicked() ),this,SLOT(pbPartition() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),this,
		SLOT( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) );
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,
		SLOT( itemClicked( QTableWidgetItem * ) ) );
}

void manageSystemVolumes::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	if( current != NULL )
		this->highlightRow( current->row(),true ) ;
	if( previous != NULL )
		if( current != NULL )
			if( previous->row() != current->row() )
				this->highlightRow( previous->row(),false ) ;
}

void manageSystemVolumes::highlightRow( int row,bool b )
{
	m_ui->tableWidget->item( row,0 )->setSelected( b );
	if( b == true )
		m_ui->tableWidget->setCurrentCell( row,0 );
}

void manageSystemVolumes::readSystemPartitions()
{
	QFile file( QString( "/etc/zuluCrypttab" ) ) ;
	if( file.open( QIODevice::ReadOnly ) == false )
		return ;

	QStringList entries = QString( file.readAll() ).split( '\n' ) ;

	int p ;
	while( true ){
		p = entries.indexOf( QString( "" ) ) ;
		if( p == -1 )
			break ;
		entries.removeAt( p );
	}

	entries.removeDuplicates() ;

	if ( entries.size() > 0 )
		this->addItemsToTable( entries );
}

void manageSystemVolumes::writeSystemPartitions()
{
	QFile file( QString( "/etc/zuluCrypttab") ) ;
	if( file.open( QIODevice::WriteOnly | QIODevice::Truncate ) == false ){

		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "could not open \"/etc/zuluCrypttab\" for writing" ) );
	}else{
		int j = m_ui->tableWidget->rowCount() ;
		if( j > 0 ){
			QTableWidgetItem * it ;
			for( int i = 0 ; i < j ; i++ ){
				it = m_ui->tableWidget->item( i,0 ) ;
				file.write( it->text().toAscii() ) ;
				file.putChar( '\n' ) ;
			}
		}

		file.setPermissions( QFile::ReadOwner|QFile::WriteOwner ) ;
		file.close();
	}
}

void manageSystemVolumes::pbAdd()
{
	QString path = m_ui->volumePath->text() ;

	if( path.isEmpty() )
		return ;

	QStringList l ;
	l.append( path );
	this->addItemsToTable( l );
}

void manageSystemVolumes::itemClicked( QTableWidgetItem * current )
{
	this->itemClicked( current,true );
}

void manageSystemVolumes::contextMenu()
{
	this->itemClicked( m_ui->tableWidget->currentItem(),false );
}

void manageSystemVolumes::itemClicked( QTableWidgetItem * current,bool clicked )
{
	QMenu m ;
	m.setFont( this->font() );
	connect( m.addAction( tr( "remove selected entry" ) ),SIGNAL( triggered() ),this,SLOT( removeCurrentRow() ) ) ;

	m.addSeparator() ;
	m.addAction( tr( "cancel" ) );

	if( clicked == true )
		m.exec( QCursor::pos() ) ;
	else{
		int x = m_ui->tableWidget->columnWidth( 0 ) / 2 ;
		int y = m_ui->tableWidget->rowHeight( current->row() ) * current->row() + 20 ;
		m.exec( m_ui->tableWidget->mapToGlobal( QPoint( x,y ) ) ) ;
	}
}

void manageSystemVolumes::removeCurrentRow()
{
	QTableWidgetItem * it = m_ui->tableWidget->currentItem() ;
	QString m = tr( "are you sure you want to remove \"%1\" from the list?" ).arg( it->text() ) ;

	DialogMsg msg( this ) ;

	if( msg.ShowUIYesNoDefaultNo( tr( "WARNING"),m ) == QMessageBox::Yes )
		m_ui->tableWidget->removeRow( it->row() ) ;

	m_ui->tableWidget->setFocus();
}

void manageSystemVolumes::addItemsToTable( QString path )
{
	if( path.isEmpty() )
		m_ui->tableWidget->setFocus();
	else{
		QStringList l ;
		l.append( path );
		this->addItemsToTable( l );
	}
}

void manageSystemVolumes::addItemsToTable( QStringList paths )
{
	int row = m_ui->tableWidget->rowCount() ;
	int i ;
	int j = paths.size() ;
	
	QTableWidgetItem * it ;

	for( i = 0 ; i < j ; i++ ){

		it = new QTableWidgetItem( paths.at( i ) ) ;

		it->setTextAlignment( Qt::AlignCenter );

		m_ui->tableWidget->insertRow( row );
		m_ui->tableWidget->setItem( row,0,it );
		m_ui->tableWidget->setCurrentCell( row,0 );
		row++ ;
	}

	m_ui->tableWidget->setFocus();
}

void manageSystemVolumes::pbDone()
{
	this->writeSystemPartitions();
	this->HideUI();
}

void manageSystemVolumes::pbFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "select path to system volume" ),QDir::homePath(),0 );

	this->addItemsToTable( Z ) ;
}

void manageSystemVolumes::pbPartition()
{
	openpartition * op = new openpartition( this ) ;
	connect( op,SIGNAL( HideUISignal() ),op,SLOT(deleteLater() ) ) ;
	connect( op,SIGNAL( clickedPartition( QString ) ),this,SLOT( clickedPartition( QString ) ) ) ;
	op->ShowAllPartitions();
}

void manageSystemVolumes::clickedPartition( QString path )
{
	this->addItemsToTable( path ) ;
}

void manageSystemVolumes::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->pbDone();
}

void manageSystemVolumes::HideUI()
{
	this->hide();
	emit HideUISignal();
}

void manageSystemVolumes::ShowUI()
{
	this->readSystemPartitions() ;
	this->show();
}

manageSystemVolumes::~manageSystemVolumes()
{
	delete m_ac ;
	delete m_ui;
}

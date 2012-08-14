#include "kwalletconfig.h"
#include "ui_kwalletconfig.h"

#include <QDebug>
kwalletconfig::kwalletconfig( QWidget * parent ) : QWidget( parent ),m_ui( new Ui::kwalletconfig )
{
	m_ui->setupUi( this );

	m_wallet = 0 ;

	this->setFixedSize( this->size() );
	this->setWindowFlags( Qt::Window | Qt::Dialog );
	this->setFont( parent->font() );

	m_ui->tableWidget->setColumnWidth( 0,415 );
	m_ui->tableWidget->setColumnWidth( 1,246 );

	m_ui->pbShowUUID->setIcon( QIcon( QString( ":/partition.png" ) ) );
	m_ui->pbGetUUIDFromFile->setIcon( QIcon( QString( ":/file.png" ) ) );

	m_ui->lineEditPassphrase->setEchoMode( QLineEdit::Password );
	m_ui->lineEditRepeatPassphrase->setEchoMode( QLineEdit::Password );

	connect( m_ui->pbGetUUIDFromFile,SIGNAL( clicked() ),this,SLOT( pbGetUUIDFromFile() ) ) ;
	connect( m_ui->pbAdd,SIGNAL( clicked() ),this,SLOT(pbAdd() ) );
	connect( m_ui->pbClose,SIGNAL( clicked() ),this,SLOT(pbClose() ) );
	connect( m_ui->pbDelete,SIGNAL( clicked() ),this,SLOT(pbDelete() ) );
	connect( m_ui->pbShowUUID,SIGNAL( clicked() ),this,SLOT( pbGetUUIDFromDevices() ) );
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT(currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
}

void kwalletconfig::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	if( current != NULL )
		this->HighlightRow( current->row(),true ) ;
	if( previous != NULL )
		if( current != NULL )
			if( previous->row() != current->row() )
				this->HighlightRow( previous->row(),false ) ;
}


void kwalletconfig::HighlightRow( int r,bool b )
{
	for( int i = 0 ; i < 2 ; i++ )
		m_ui->tableWidget->item( r,i )->setSelected( b );

	if( b )
		m_ui->tableWidget->setCurrentCell( r,1 );
}

void kwalletconfig::pbAdd()
{
	QString uuid  = m_ui->lineEditUUID->text() ;
	QString pass  = m_ui->lineEditPassphrase->text() ;
	QString rpass = m_ui->lineEditRepeatPassphrase->text() ;

	DialogMsg msg( this ) ;

	if( rpass != pass )
		return msg.ShowUIOK( tr( "ERROR"),tr( "passphrases do not match" ) ) ;

	if( pass.isEmpty() )
		return msg.ShowUIOK( tr( "ERROR"),tr( "passphrase field is empty" ) ) ;

	if( uuid.isEmpty() )
		return msg.ShowUIOK( tr( "ERROR"),tr( "volume UUID field is empty" ) ) ;

	m_map.insert( uuid,pass ) ;

	QTableWidgetItem * item ;
	QTableWidget * table = m_ui->tableWidget ;

	int row = table->rowCount() ;
	table->insertRow( row );

	item = new QTableWidgetItem() ;
	item->setTextAlignment( Qt::AlignCenter );
	item->setText( uuid );
	table->setItem( row,0,item ) ;

	item = new QTableWidgetItem() ;
	item->setTextAlignment( Qt::AlignCenter );
	//item->setText( pass );
	item->setText( tr( "< reducted >" ) ) ;
	table->setItem( row,1,item ) ;

	m_ui->lineEditUUID->clear();
	m_ui->lineEditPassphrase->clear();
	m_ui->lineEditRepeatPassphrase->clear();
}

void kwalletconfig::pbDelete()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;

	QString key = m_ui->tableWidget->item( item->row(),0 )->text() ;

	m_map.remove( key ) ;

	m_ui->tableWidget->removeRow( item->row() );
}

void kwalletconfig::pbClose()
{
	this->HideUI();
}

void kwalletconfig::pbGetUUIDFromDevices()
{
	openpartition * op = new openpartition( this ) ;
	connect( op,SIGNAL( HideUISignal() ),op,SLOT( deleteLater() ) ) ;
	connect( op,SIGNAL( clickedPartition( QString ) ),this,SLOT( selectedVolume( QString ) ) ) ;
	op->allowLUKSOnly();
}

void kwalletconfig::pbGetUUIDFromFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "select a luks volume" ),QDir::homePath(),0 );

	if( Z.isEmpty() )
		return ;

	Z = miscfunctions::getUUIDFromPath( Z ) ;

	if( Z.isEmpty() ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "File does not appear to be a luks volume") ) ;
		m_ui->lineEditUUID->clear();
	}else{
		m_ui->lineEditUUID->setText( Z ) ;
	}
}

void kwalletconfig::selectedVolume( QString volume )
{
	m_ui->lineEditUUID->setText( volume );
}

void kwalletconfig::ShowUI()
{
	m_wallet = new kwalletplugin( this ) ;

	if( m_wallet->open() ){
		this->ShowWalletEntries();
	}else{
		qDebug() << "failed to open";
		this->HideUI();
	}
}

void kwalletconfig::ShowWalletEntries()
{
	if( !m_wallet->setFolder( "Form Data" ) ){
		qDebug() << "setFolder() failed ";
		return ;
	}

	if( m_wallet->readMap( m_map ) ){
		qDebug() << "readMap() failed";
		return ;
	}

	if( m_map.empty() ){
		qDebug()<< "map is empty";
		return ;
	}

	QTableWidget * table = m_ui->tableWidget ;
	QTableWidgetItem * item ;

	int row = table->rowCount() ;

	QMap< QString,QString >::const_iterator i = m_map.constBegin() ;
	QMap< QString,QString >::const_iterator j = m_map.constEnd() ;

	for( ; i != j; i++ ){
		table->insertRow( row );
		item = new QTableWidgetItem() ;
		item->setText( i.key() );
		item->setTextAlignment( Qt::AlignCenter );
		table->setItem( row,0,item );

		item = new QTableWidgetItem() ;
		//item->setText( i.value() );
		item->setText( tr( "< reducted >" ) ) ;
		item->setTextAlignment( Qt::AlignCenter );
		table->setItem( row,1,item );

		table->setCurrentCell( row,1 );
		row++ ;
	}

	table->setFocus();
	this->show();
}

void kwalletconfig::HideUI()
{
	m_wallet->writeMap( m_map ) ;
	emit HideUISignal() ;
	this->hide();
}

void kwalletconfig::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->HideUI();
}

kwalletconfig::~kwalletconfig()
{
	if( m_wallet != 0 )
		m_wallet->close();
	delete m_ui;
}

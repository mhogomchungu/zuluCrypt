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

	m_ui->tableWidget->setColumnWidth( 0,386 );
	m_ui->tableWidget->setColumnWidth( 1,173 );
	m_ui->tableWidget->setColumnWidth( 2,109 );

	m_ui->pbShowUUID->setIcon( QIcon( QString( ":/partition.png" ) ) );
	m_ui->pbGetUUIDFromFile->setIcon( QIcon( QString( ":/file.png" ) ) );

	m_ui->lineEditPassphrase->setEchoMode( QLineEdit::Password );
	m_ui->lineEditRepeatPassphrase->setEchoMode( QLineEdit::Password );

	connect( m_ui->pbGetUUIDFromFile,SIGNAL( clicked() ),this,SLOT( pbGetUUIDFromFile() ) ) ;
	connect( m_ui->pbAdd,SIGNAL( clicked() ),this,SLOT( pbAdd() ) );
	connect( m_ui->pbClose,SIGNAL( clicked() ),this,SLOT( pbClose() ) );
	connect( m_ui->pbDelete,SIGNAL( clicked() ),this,SLOT( pbDelete() ) );
	connect( m_ui->pbShowUUID,SIGNAL( clicked() ),this,SLOT( pbGetUUIDFromDevices() ) );
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT(currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
}

void kwalletconfig::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	miscfunctions::selectTableRow( current,previous ) ;
}

void kwalletconfig::pbAdd()
{
	QString uuid  = m_ui->lineEditUUID->text() ;
	QString pass  = m_ui->lineEditPassphrase->text() ;
	QString rpass = m_ui->lineEditRepeatPassphrase->text() ;
	QString comm  = m_ui->lineEditComment->text() ;

	if( comm.isEmpty() )
		comm = QString( "Nil" ) ;

	DialogMsg msg( this ) ;

	if( rpass != pass )
		return msg.ShowUIOK( tr( "ERROR"),tr( "passphrases do not match" ) ) ;

	if( pass.isEmpty() )
		return msg.ShowUIOK( tr( "ERROR"),tr( "passphrase field is empty" ) ) ;

	if( uuid.isEmpty() )
		return msg.ShowUIOK( tr( "ERROR"),tr( "volume UUID field is empty" ) ) ;

	//read comment on ShowWalletEntries() function below to understand why there are two inserts.
	QString m = uuid + QString( "-comment") ;

	m_map.insert( m,comm ) ;
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
	item->setText( comm );
	table->setItem( row,1,item ) ;

	item = new QTableWidgetItem() ;
	item->setTextAlignment( Qt::AlignCenter );
	//item->setText( pass );
	item->setText( tr( "<reducted>" ) ) ;
	table->setItem( row,2,item ) ;

	table->setCurrentCell( row,table->columnCount() -1 );

	m_ui->lineEditUUID->clear();
	m_ui->lineEditPassphrase->clear();
	m_ui->lineEditRepeatPassphrase->clear();
	m_ui->lineEditComment->clear();
}

void kwalletconfig::pbDelete()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;

	QString key = m_ui->tableWidget->item( item->row(),0 )->text() ;

	//read comment on ShowWalletEntries() function below to understand why there are two remove.

	m_map.remove( key ) ;

	key = key + QString( "-comment" ) ;

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
		return 	this->show();
	}

	QTableWidget * table = m_ui->tableWidget ;
	QTableWidgetItem * item ;

	int row = table->rowCount() ;

	QMap< QString,QString >::const_iterator i = m_map.constBegin() ;
	QMap< QString,QString >::const_iterator j = m_map.constEnd() ;

	QString uuid ;

	/*
	 * each volume gets two entries in kwallet:
	 * First one in the form of  : key<UUID="blablabla">value<uuid passphrase>
	 * Second one in the form of : key<UUID="blablabla"-comment">value<comment>
	 *
	 * This allows to store a a volume UUID, a comment about it and its passphrase.
	 *
	 * To populate the table with the above 3 information,the first loop below sets UUID and passphrase
	 * and the second loop match each comment to its corresponding volume line based on UUID
	 */
	int column = table->columnCount() - 1 ;

	for( ; i != j ; i++ ){

		uuid = i.key() ;
		if( uuid.contains( QString( "-comment" ) ) )
			continue ;

		table->insertRow( row );

		item = new QTableWidgetItem() ;
		item->setText( uuid );
		item->setTextAlignment( Qt::AlignCenter );
		table->setItem( row,0,item );

		item = new QTableWidgetItem() ;
		item->setText( QString("") );
		item->setTextAlignment( Qt::AlignCenter );
		table->setItem( row,1,item );

		item = new QTableWidgetItem() ;
		//item->setText( i.value() );
		item->setText( tr( "<reducted>" ) ) ;
		item->setTextAlignment( Qt::AlignCenter );
		table->setItem( row,2,item );

		table->setCurrentCell( row,column );
		row++ ;
	}

	int p ;
	int q = m_ui->tableWidget->rowCount() ;

	if( q > 0 ){
		for( i = m_map.constBegin() ; i != j ; i++ ){

			uuid = i.key() ;

			if( !uuid.contains( QString( "-comment" ) ) )
				continue ;

			uuid.remove( QString( "-comment" ) ) ;

			for( p = 0 ; p < q ; p++ ){
				if( m_ui->tableWidget->item( p,0 )->text() == uuid ){
					m_ui->tableWidget->item( p,1 )->setText( i.value() );
					break ;
				}
			}
		}
	}

	table->setFocus();

	this->show();
}

void kwalletconfig::HideUI()
{
	if( m_wallet->isOpen() )
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
	//qDebug() << m_ui->tableWidget->columnWidth(0);
	//qDebug() << m_ui->tableWidget->columnWidth(1);
	//qDebug() << m_ui->tableWidget->columnWidth(2);

	if( m_wallet )
		m_wallet->close();
	delete m_ui;
}

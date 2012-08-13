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

	m_ui->tableWidget->setColumnWidth( 0,343 );
	m_ui->tableWidget->setColumnWidth( 1,324 );

	connect( m_ui->pbAdd,SIGNAL( clicked() ),this,SLOT(pbAdd() ) );
	connect( m_ui->pbClose,SIGNAL( clicked() ),this,SLOT(pbClose() ) );
	connect( m_ui->pbDelete,SIGNAL( clicked() ),this,SLOT(pbDelete() ) );
	connect( m_ui->pbShowUUID,SIGNAL( clicked() ),this,SLOT( pbShowDevices() ) );
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

}

void kwalletconfig::pbDelete()
{

}

void kwalletconfig::pbClose()
{
	this->HideUI();
}

void kwalletconfig::pbShowDevices()
{

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

	QMap < QString,QString > map ;

	if( m_wallet->readMap( "LUKS",map ) ){
		qDebug() << "readMap() failed";
		return ;
	}

	if( map.empty() ){
		qDebug()<< "map is empty";
		return ;
	}

	QTableWidget * table = m_ui->tableWidget ;
	QTableWidgetItem * item ;

	int row = table->rowCount() ;

	QMap< QString,QString >::const_iterator i = map.constBegin() ;
	QMap< QString,QString >::const_iterator j = map.constEnd() ;

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

	if( m_wallet != 0 )
		m_wallet->close();
	delete m_ui;
}

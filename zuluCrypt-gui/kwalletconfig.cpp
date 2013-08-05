#include "kwalletconfig.h"
#include "ui_kwalletconfig.h"

kwalletconfig::kwalletconfig( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::kwalletconfig )
{
	m_ui->setupUi( this );

	this->setFixedSize( this->size() );
	this->setFont( parent->font() );

	m_ui->tableWidget->setColumnWidth( 0,386 );
	m_ui->tableWidget->setColumnWidth( 1,237 );
	m_ui->tableWidget->hideColumn( 2 ) ;

	connect( m_ui->pbAdd,SIGNAL( clicked() ),this,SLOT( pbAdd() ) );
	connect( m_ui->pbClose,SIGNAL( clicked() ),this,SLOT( pbClose() ) );
	connect( m_ui->pbDelete,SIGNAL( clicked() ),this,SLOT( pbDelete() ) );
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT(currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),
		 this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;
}

void kwalletconfig::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;
}

void kwalletconfig::itemClicked( QTableWidgetItem * item )
{
	DialogMsg msg( this ) ;

	QTableWidget * table = m_ui->tableWidget ;
	QString volumeID = table->item( item->row(),0 )->text() ;

	int r = msg.ShowUIYesNo( tr( "warning" ),tr( "are you sure you want to delete a volume with an id of \"%1\"?" ).arg( volumeID ) ) ;

	if( r == QMessageBox::Yes ){
		//read comment on ShowWalletEntries() function below to understand why there are two remove.

		m_map.remove( volumeID ) ;

		volumeID = volumeID + QString( "-comment" ) ;

		m_map.remove( volumeID ) ;

		tablewidget::deleteRowFromTable( table,item->row() ) ;
	}
}

void kwalletconfig::pbDelete()
{
	this->itemClicked( m_ui->tableWidget->currentItem() ) ;
}

void kwalletconfig::pbClose()
{
	this->HideUI();
}

void kwalletconfig::add(QString volumeID,QString comment,QString key)
{
	//read comment on ShowWalletEntries() function below to understand why there are two inserts.
	QString m = volumeID + QString( "-comment") ;

	m_map.insert( m,comment ) ;
	m_map.insert( volumeID,key ) ;

	QStringList entry ;

	entry.append( volumeID );
	entry.append( comment );
	//entry.append( pass );
	entry.append( tr( "<redacted>" ) );

	tablewidget::addRowToTable( m_ui->tableWidget,entry ) ;
}

void kwalletconfig::pbAdd()
{
	kwalletconfiginput * k = new kwalletconfiginput( this ) ;
	connect( k,SIGNAL( add( QString,QString,QString ) ),this,SLOT( add( QString,QString,QString ) ) ) ;
	k->ShowUI() ;
}

void kwalletconfig::ShowUI()
{
	m_wallet = new kwalletplugin( this ) ;
	if( m_wallet->open() ){
		this->ShowWalletEntries() ;
	}else{
		this->failedToOpenWallet() ;
	}
}

void kwalletconfig::failedToOpenWallet()
{
	emit couldNotOpenWallet();
	this->HideUI();
}

void kwalletconfig::ReShowUI()
{
	this->show();
}

void kwalletconfig::ShowWalletEntries()
{
	m_wallet->setFolder( kwalletplugin::formData() ) ;

	m_wallet->readMap( m_map ) ;

	if( m_map.empty() ){
		return this->show();
	}
	QTableWidget * table = m_ui->tableWidget ;

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

	QStringList s ;

	for( ; i != j ; i++ ){

		uuid = i.key() ;
		if( uuid.contains( QString( "-comment" ) ) ){
			continue ;
		}
		s.clear();
		s.append( uuid );
		s.append( QString( "" ) );
		//item->setText( i.value() );
		s.append( tr( "<redacted>" ) ) ;

		tablewidget::addRowToTable( table,s ) ;
	}

	int p ;
	int q = m_ui->tableWidget->rowCount() ;

	if( q > 0 ){
		for( i = m_map.constBegin() ; i != j ; i++ ){

			uuid = i.key() ;

			if( !uuid.contains( QString( "-comment" ) ) ){
				continue ;
			}
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
	if( m_wallet->isOpen() ){
		m_wallet->writeMap( m_map ) ;
		//m_wallet->close();
	}

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
	m_wallet->deleteLater() ;
	delete m_ui;
}

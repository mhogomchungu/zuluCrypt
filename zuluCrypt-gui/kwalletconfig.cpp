/*
 *  Copyright ( c ) 2012
 *  name : mhogo mchungu
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

#include "kwalletconfig.h"
#include "ui_kwalletconfig.h"

#include <QCloseEvent>

#include "dialogmsg.h"
#include "openvolume.h"
#include "tablewidget.h"
#include "kwalletconfiginput.h"
#include "utility.h"
#include "lxqt_wallet/frontend/lxqt_wallet.h"

#include <Qt>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

#define COMMENT "-zuluCrypt_Comment_ID"

kwalletconfig::kwalletconfig( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::kwalletconfig )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_ui->tableWidget->setColumnWidth( 0,386 ) ;
	m_ui->tableWidget->setColumnWidth( 1,237 ) ;
	m_ui->tableWidget->hideColumn( 2 ) ;

	connect( m_ui->pbAdd,SIGNAL( clicked() ),this,SLOT( pbAdd() ) ) ;
	connect( m_ui->pbClose,SIGNAL( clicked() ),this,SLOT( pbClose() ) ) ;
	connect( m_ui->pbDelete,SIGNAL( clicked() ),this,SLOT( pbDelete() ) ) ;
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
	this->disableAll() ;

	DialogMsg msg( this ) ;

	QTableWidget * table = m_ui->tableWidget ;
	QString volumeID = table->item( item->row(),0 )->text() ;

	int r = msg.ShowUIYesNo( tr( "warning" ),tr( "are you sure you want to delete a volume with an id of \"%1\"?" ).arg( volumeID ) ) ;

	if( r == QMessageBox::Yes ){
		m_wallet->deleteKey( volumeID ) ;
		m_wallet->deleteKey( volumeID + COMMENT ) ;
		tablewidget::deleteRowFromTable( table,item->row() ) ;
	}

	this->enableAll() ;
}

void kwalletconfig::pbDelete()
{
	this->itemClicked( m_ui->tableWidget->currentItem() ) ;
}

void kwalletconfig::pbClose()
{
	this->HideUI() ;
}

void kwalletconfig::add( QString volumeID,QString comment,QString key )
{
	if( comment.isEmpty() ){
		comment = QString( "Nil" ) ;
	}

	m_wallet->addKey( volumeID,key.toAscii() ) ;
	m_wallet->addKey( volumeID + COMMENT,comment.toAscii() ) ;

	QStringList entry ;

	entry.append( volumeID ) ;
	entry.append( comment ) ;
	entry.append( tr( "<redacted>" ) ) ;

	tablewidget::addRowToTable( m_ui->tableWidget,entry ) ;
	this->enableAll() ;
	/*
	 * we and hide and delete this object here and not in the object itself because some backends(libsecret) takes too long
	 * to complete and UI freeze maybe noticiable
	 */
	m_kwalletConfig->hide() ;
	m_kwalletConfig->deleteLater() ;
}

void kwalletconfig::cancel()
{
	this->enableAll() ;
}

void kwalletconfig::pbAdd()
{
	this->disableAll() ;
	m_kwalletConfig = new kwalletconfiginput( this ) ;
	connect( m_kwalletConfig,SIGNAL( add( QString,QString,QString ) ),this,SLOT( add( QString,QString,QString ) ) ) ;
	connect( m_kwalletConfig,SIGNAL( cancel() ),this,SLOT( cancel() ) ) ;
	m_kwalletConfig->ShowUI() ;
}

void kwalletconfig::ShowUI( lxqt::Wallet::walletBackEnd backEnd )
{
	this->disableAll() ;
	this->show() ;
	m_wallet = lxqt::Wallet::getWalletBackend( backEnd ) ;
	m_wallet->setInterfaceObject( this ) ;
	if( backEnd == lxqt::Wallet::kwalletBackEnd ){
		m_wallet->open( utility::defaultKDEWalletName(),utility::applicationName() ) ;
	}else{
		m_wallet->open( utility::walletName(),utility::applicationName() ) ;
	}
}

void kwalletconfig::walletIsOpen( bool opened )
{
	if( opened ){
		this->ShowWalletEntries() ;
	}else{
		this->failedToOpenWallet() ;
	}
}

void kwalletconfig::enableAll()
{
	m_ui->groupBox->setEnabled( true ) ;
	m_ui->pbAdd->setEnabled( true ) ;
	m_ui->pbClose->setEnabled( true ) ;
	m_ui->pbDelete->setEnabled( true ) ;
	m_ui->tableWidget->setEnabled( true ) ;
}

void kwalletconfig::disableAll()
{
	m_ui->groupBox->setEnabled( false ) ;
	m_ui->pbAdd->setEnabled( false ) ;
	m_ui->pbClose->setEnabled( false ) ;
	m_ui->pbDelete->setEnabled( false ) ;
	m_ui->tableWidget->setEnabled( false ) ;
}

void kwalletconfig::failedToOpenWallet()
{
	this->enableAll() ;
	emit couldNotOpenWallet() ;
	this->HideUI() ;
}

void kwalletconfig::ShowWalletEntries()
{
	QStringList entries = m_wallet->readAllKeys() ;

	this->enableAll() ;

	if( entries.empty() ){
		return this->show() ;
	}

	QTableWidget * table = m_ui->tableWidget ;

	/*
	 * each volume gets two entries in kwallet:
	 * First one in the form of  : key<UUID="blablabla">value<uuid passphrase>
	 * Second one in the form of : key<UUID="blablabla"-comment">value<comment>
	 *
	 * This allows to store a a volume UUID, a comment about it and its passphrase.
	 *
	 */

	QStringList s ;
	int j = entries.size() ;
	for( int i = 0 ; i < j ; i++ ){
		const QString& e = entries.at( i ) ;
		if( e.endsWith( COMMENT ) ){
			;
		}else{
			s.clear() ;
			s.append( e ) ;
			s.append( m_wallet->readValue( e + QString( COMMENT ) ) ) ;
			s.append( tr( "<redacted>" ) ) ;
			tablewidget::addRowToTable( table,s ) ;
		}
	}

	table->setFocus() ;
}

void kwalletconfig::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void kwalletconfig::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

kwalletconfig::~kwalletconfig()
{
	m_wallet->deleteLater() ;
	delete m_ui;
}

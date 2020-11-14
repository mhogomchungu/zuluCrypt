/*
 *
 *  Copyright ( c ) 2020
 *  name : Francis Banyikwa
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

#include "showluksslots.h"
#include "ui_showluksslots.h"

#include "utility.h"
#include "openvolume.h"
#include "luksdeletekey.h"
#include "luksaddkey.h"
#include "bin_path.h"
#include "tablewidget.h"

#include <QFileDialog>

showLUKSSlots::showLUKSSlots( QWidget * parent,const QString& p,std::function< void() > function ) :
	QDialog( parent ),
	m_ui( new Ui::showLUKSSlots ),
	m_function( std::move( function ) ),
	m_path( p ),
	m_parent( parent )
{
	m_ui->setupUi( this ) ;
	this->installEventFilter( this ) ;

	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_ui->labelError->setVisible( false ) ;
	m_ui->pbOK->setVisible( false ) ;

	m_ui->checkBox->setChecked( utility::showOnlyOccupiedSlots() ) ;

	m_ui->tableWidget->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;

	m_ui->tableWidget->verticalHeader()->setMinimumSectionSize( 30 ) ;

	m_ui->tableWidget->horizontalHeader()->setStretchLastSection( true ) ;

	m_ui->pbFile->setIcon( QIcon( ":/file.png" ) ) ;

	m_ui->pbDevice->setIcon( QIcon( ":/partition.png" ) ) ;

	m_ui->tableWidget->setContextMenuPolicy( Qt::CustomContextMenu ) ;

	connect( m_ui->checkBox,&QCheckBox::stateChanged,[ this ]( int s ){

		utility::showOnlyOccupiedSlots( s != Qt::Unchecked ) ;

		this->showData() ;
	} ) ;

	connect( m_ui->tableWidget,&QTableWidget::customContextMenuRequested,[ this ]( QPoint s ){

		Q_UNUSED( s )

		auto item = m_ui->tableWidget->currentItem() ;

		if( item ){

			this->showMenu( item ) ;
		}
	} ) ;

	connect( m_ui->pbOK,&QPushButton::clicked,[ this ](){

		m_ui->labelError->setVisible( false ) ;
		m_ui->pbOK->setVisible( false ) ;
		this->enableAll() ;
	} ) ;

	connect( m_ui->tableWidget,&QTableWidget::itemClicked,[ this ]( QTableWidgetItem * item ){

		this->showMenu( item ) ;
	} ) ;

	connect( m_ui->tableWidget,&QTableWidget::currentItemChanged,
		 []( QTableWidgetItem * c,QTableWidgetItem * p ){

		tablewidget::selectRow( c,p ) ;
	} ) ;

	connect( m_ui->pbFile,&QPushButton::clicked,[ this ](){

		auto Z = QFileDialog::getOpenFileName( this,tr( "Encrypted LUkS Volume Path" ),utility::homePath() ) ;

		if( !Z.isEmpty() ){

			m_ui->lineEdit->setText( Z ) ;			

			m_ui->lineEdit->setFocus() ;

			this->showData() ;
		}
	} ) ;

	connect( m_ui->pbDevice,&QPushButton::clicked,[ this ](){

		this->hide() ;

		auto& m = openvolume::instance( this,false ) ;

		m.setOnExit( [ this ](){ this->show() ; } ) ;

		m.showLuksOnly() ;

		m.ShowAllPartitions( [ this ]( const QString& e ){

			m_ui->lineEdit->setText( e ) ;

			this->showData() ;
		} ) ;
	} ) ;

	connect( m_ui->pbClose,&QPushButton::clicked,[ this ](){

		this->HideUI() ;
	} ) ;
}

showLUKSSlots::~showLUKSSlots()
{
	delete m_ui ;
}

void showLUKSSlots::deleteSlot( QTableWidgetItem * item )
{
	auto m = utility::split( item->text(),'\n' ).first() ;

	if( !m.isEmpty() ){

		auto mm = utility::split( m,' ' ) ;

		if( mm.size() > 1 ){

			this->hide() ;

			auto s = m_ui->lineEdit->text() ;

			luksdeletekey::instance( m_parent ).ShowUI( s,mm.at( 2 ),[ this ](){

				this->show() ;

				this->showData() ;
			} ) ;
		}
	}
}

void showLUKSSlots::addSlot( QTableWidgetItem * item )
{
	auto m = utility::split( item->text(),'\n' ).first() ;

	if( !m.isEmpty() ){

		auto mm = utility::split( m,' ' ) ;

		if( mm.size() > 1 ){

			this->hide() ;

			auto s = m_ui->lineEdit->text() ;

			luksaddkey::instance( m_parent ).ShowUI( s,mm.at( 2 ),[ this ](){

				this->show() ;

				this->showData() ;
			} ) ;
		}
	}
}

void showLUKSSlots::showData()
{
	this->disableAll() ;

	tablewidget::clearTable( m_ui->tableWidget ) ;

	auto m = m_ui->lineEdit->text() ;

	auto exe = utility::appendUserUID( "%1 -b -b -d \"%2\"" ) ;

	auto s = utility::Task::run( exe.arg( ZULUCRYPTzuluCrypt,m ) ).await() ;

	bool showOnlyOccupiedSlots = utility::showOnlyOccupiedSlots() ;

	QString tmp ;

	if( s.success() ){

		auto a = utility::split( s.stdOut(),"\n\n" ) ;

		if( a.isEmpty() ){

			m_ui->labelError->setVisible( true ) ;
			m_ui->pbOK->setVisible( true ) ;
			m_ui->labelError->setText( "Failed To Get Data From zuluCrypt-cli" ) ;

			return ;
		}

		m_ui->tableWidget->horizontalHeaderItem( 0 )->setText( a.takeAt( 0 ) ) ;

		for( const auto& it : a ){

			auto b = utility::split( it,"\n" ) ;

			if( b.size() == 2 ){

				if( !showOnlyOccupiedSlots ){

					tmp = b.at( 0 ) + "\n" + b.at( 1 ) ;
				}

			}else if( b.size() > 2 ){

				if( b.at( 1 ).contains( "Active" ) ){

					tmp = b.at( 0 ) + "\n" + b.at( 1 ) + "\n" + b.at( 3 ) ;

					for( int i = 4 ; i < b.size() ; i++ ){

						if( i % 6 == 0 ){

							tmp += "\n" + b.at( i ) ;
						}else{
							tmp += ", " + b.at( i ) ;
						}
					}
				}
			}

			if( !tmp.isEmpty() ){

				tablewidget::addRow( m_ui->tableWidget,{ tmp } ) ;
			}

			tmp.clear() ;
		}

		this->enableAll() ;
	}else{
		m_ui->labelError->setVisible( true ) ;
		m_ui->pbOK->setVisible( true ) ;
		m_ui->labelError->setText( s.stdOut() ) ;
	}
}

void showLUKSSlots::HideUI()
{
	utility::showOnlyOccupiedSlots( m_ui->checkBox->isChecked() ) ;
	this->hide() ;
	m_function() ;
	this->deleteLater() ;
}

void showLUKSSlots::ShowUI()
{
	if( m_path.isEmpty() ){

		this->show() ;
	}else{
		this->show() ;

		m_ui->lineEdit->setText( m_path ) ;

		this->showData() ;
	}
}

void showLUKSSlots::enableAll()
{
	m_ui->tableWidget->setEnabled( true ) ;
	m_ui->pbClose->setEnabled( true ) ;
	m_ui->pbFile->setEnabled( true ) ;
	m_ui->pbDevice->setEnabled( true ) ;
	m_ui->lineEdit->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->checkBox->setEnabled( true ) ;
}

void showLUKSSlots::disableAll()
{
	m_ui->checkBox->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->tableWidget->setEnabled( false ) ;
	m_ui->pbClose->setEnabled( false ) ;
	m_ui->pbFile->setEnabled( false ) ;
	m_ui->pbDevice->setEnabled( false ) ;
	m_ui->lineEdit->setEnabled( false ) ;
}

void showLUKSSlots::showMenu( QTableWidgetItem * item )
{
	QMenu m ;

	auto text = item->text() ;

	if( text.contains( "Slot Status: Active" ) ){

		m.addAction( tr( "Delete Key Slot" ) )->setObjectName( "Delete Key Slot" ) ;

	}else if( text.contains( "Slot Status: Inactive" ) ){

		m.addAction( tr( "Add Key To This Slot" ) )->setObjectName( "Add Key To This Slot" ) ;
	}

	m.addAction( tr( "Cancel" ) )->setObjectName( "Cancel" ) ;

	connect( &m,&QMenu::triggered,[ this,item ]( QAction * ac ){

		auto txt = ac->objectName() ;

		if( txt == "Delete Key Slot" ){

			this->deleteSlot( item ) ;

		}else if( txt == "Add Key To This Slot" ){

			this->addSlot( item ) ;
		}
	} ) ;

	m.exec( QCursor::pos() ) ;
}

void showLUKSSlots::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	this->HideUI() ;
}

bool showLUKSSlots::eventFilter(QObject * watched, QEvent * event)
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

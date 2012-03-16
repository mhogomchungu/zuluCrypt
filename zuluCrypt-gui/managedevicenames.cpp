/*
 * 
 *  Copyright (c) 2011
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

#include "managedevicenames.h"
#include "ui_managedevicenames.h"
#include <iostream>

managedevicenames::managedevicenames(QWidget *parent) :
        QWidget(parent),
	m_ui(new Ui::managedevicenames)
{	
	m_ui->setupUi(this);
	this->setWindowFlags(Qt::Window | Qt::Dialog);
	this->setFont(parent->font());

	connect(m_ui->pbDeviceAddress,SIGNAL(clicked()),this,SLOT(deviceAddress()));
	connect(m_ui->pbAdd,SIGNAL(clicked()),this,SLOT(add()));
	connect(m_ui->pbFileAddress,SIGNAL(clicked()),this,SLOT(fileAddress()));
	connect(m_ui->pbCancel,SIGNAL(clicked()),this,SLOT(cancel()));
	connect(m_ui->tableWidget,SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),this,
		SLOT(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)));
	connect(m_ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,
		SLOT(itemClicked(QTableWidgetItem*)));
	connect(m_ui->lineEditDeviceAddress,SIGNAL(textChanged(QString)),this,SLOT(devicePathTextChange(QString)));

	m_ui->pbFileAddress->setIcon(QIcon(QString(":/keyfile.png")));
	m_ui->pbDeviceAddress->setIcon(QIcon(QString(":/partition.png")));
	m_ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter );
	keys.append( Qt::Key_Return );
	keys.append( Qt::Key_Menu );
	m_ac->setShortcuts(keys) ;
	connect(m_ac,SIGNAL(triggered()),this,SLOT(shortcutPressed())) ;
	this->addAction( m_ac );
}

void managedevicenames::devicePathTextChange(QString txt)
{
	if(txt == QString("")){
		m_ui->lineEditMountPath->setText( "" ) ;
		return ;		
	}
	QString ed = QDir::homePath() + QString("/") + txt.split("/").last() ;
	m_ui->lineEditMountPath->setText( ed );
}

void managedevicenames::shortcutPressed()
{
	QTableWidgetItem *it = m_ui->tableWidget->currentItem() ;
	if( it == NULL )
		return ;
	itemClicked( it,false );
}

void managedevicenames::deviceAddress()
{
	openpartition *openPartition = new openpartition(this) ;
	connect(openPartition,SIGNAL(clickedPartition(QString)),this,SLOT(PartitionEntry(QString)));
	connect(openPartition,SIGNAL(HideUISignal()),openPartition,SLOT(deleteLater()));
	openPartition->ShowAllPartitions();
}

void managedevicenames::ShowUI()
{
	m_ui->tableWidget->setColumnWidth(0,296);
	m_ui->tableWidget->setColumnWidth(1,296);
	while( m_ui->tableWidget->rowCount() > 0)
		m_ui->tableWidget->removeRow(0);
	QStringList entries = miscfunctions::readFavorites() ;
	QStringList line ;	
	for( int i = 0 ; i < entries.size() - 1 ; i++){
		line = entries.at(i).split("\t");
		addEntries(line.at(0),line.at(1));
	}	
	m_ui->lineEditDeviceAddress->clear();
	m_ui->lineEditMountPath->clear();
	m_ui->tableWidget->setFocus();
	this->show();
}

void managedevicenames::HideUI()
{
	this->hide();	
	emit HideUISignal();
}

void managedevicenames::addEntries(QString dev, QString m_point)
{
	QTableWidgetItem *item = new QTableWidgetItem() ;
	int row = m_ui->tableWidget->rowCount() ;
	m_ui->tableWidget->insertRow(row);
	item->setText( dev );
	item->setTextAlignment(Qt::AlignCenter);
	m_ui->tableWidget->setItem(row,0,item);
	item = new QTableWidgetItem() ;
	item->setText( m_point );
	item->setTextAlignment(Qt::AlignCenter);
	m_ui->tableWidget->setItem(row,1,item);
	m_ui->tableWidget->setCurrentCell(row ,1);
}

void managedevicenames::itemClicked(QTableWidgetItem *current)
{
	itemClicked(current,true);
}

void managedevicenames::itemClicked(QTableWidgetItem *current, bool clicked)
{
	QMenu m ;
	m.setFont(this->font());
	connect(m.addAction(tr("remove selected entry")),SIGNAL(triggered()),this,SLOT(removeEntryFromFavoriteList())) ;

	m.addSeparator() ;
	m.addAction(tr("cancel"));

	if( clicked == true )
		m.exec(QCursor::pos()) ;
	else{
		int x = m_ui->tableWidget->columnWidth(0) ;
		int y = m_ui->tableWidget->rowHeight(current->row()) * current->row() + 20 ;
		m.exec(m_ui->tableWidget->mapToGlobal(QPoint(x,y))) ;
	}
}

void managedevicenames::removeEntryFromFavoriteList()
{
	int row = m_ui->tableWidget->currentRow() ;
	QString entry = m_ui->tableWidget->item(row,0)->text() + \
			QString("\t") + \
			m_ui->tableWidget->item(row,1)->text() + \
			QString("\n") ;
	miscfunctions::removeFavoriteEntry(entry);
	m_ui->tableWidget->removeRow(row);
	if( m_ui->tableWidget->rowCount() > 0)
		HighlightRow(m_ui->tableWidget->rowCount() - 1,true);
}

void managedevicenames::cancel()
{
	HideUI();
}

void managedevicenames::add()
{
	QMessageBox m ;
	m.setParent(this);
	m.setWindowTitle(QString("ERROR"));
	m.setWindowFlags(Qt::Window | Qt::Dialog);

	QString dev = m_ui->lineEditDeviceAddress->text() ;
	QString mount_point = m_ui->lineEditMountPath->text() ;
	
	if( dev == QString("")){
		m.setText(tr("device address field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec();
		return ;
	}
	if( mount_point == QString("")){
		m.setText(tr("mount point path field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec();
		return ;
	}
	addEntries(dev,mount_point);
	miscfunctions::addToFavorite(dev,mount_point);
	
	m_ui->lineEditDeviceAddress->clear(); ;
	m_ui->lineEditMountPath->clear() ;
}

void managedevicenames::fileAddress()
{
	QString Z = QFileDialog::getOpenFileName(this,tr("path to an encrypted file"),QDir::homePath(),0);
	m_ui->lineEditDeviceAddress->setText( Z ) ;
}

void managedevicenames::PartitionEntry(QString device)
{
	m_ui->lineEditDeviceAddress->setText( device );
}

managedevicenames::~managedevicenames()
{
	delete m_ui;
	delete m_ac;
}

void managedevicenames::closeEvent(QCloseEvent *e)
{
	e->ignore();
	HideUI();
}

void managedevicenames::currentItemChanged( QTableWidgetItem * current, QTableWidgetItem * previous )
{
	if(current != NULL)
		HighlightRow(current->row(), true) ;
	if(previous != NULL)
		if(current != NULL)
			if(previous->row() != current->row())
				HighlightRow(previous->row(), false) ;
}

void managedevicenames::HighlightRow(int row,bool b)
{
	m_ui->tableWidget->item(row,0)->setSelected(b);
	m_ui->tableWidget->item(row,1)->setSelected(b);
	if(b == true)
		m_ui->tableWidget->setCurrentCell(row,1);
}

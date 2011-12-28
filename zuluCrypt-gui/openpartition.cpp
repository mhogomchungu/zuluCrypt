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

#include "openpartition.h"
#include <QProcess>
#include <iostream>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QObject>
#include <QHeaderView>
#include <QFile>
#include <QThread>
#include <QKeySequence>

#include "../zuluCrypt-cli/executables.h"


openpartition::openpartition(QWidget *parent ) :
	QDialog(parent)
{
	partitionView = new Ui::PartitionView() ;
	partitionView->setupUi(this);
	row = -1 ;
	this->setFixedSize(this->size());
	connect(partitionView->tableWidgetPartitionView,
		SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
		this,
		SLOT(tableEntryDoubleClicked(QTableWidgetItem *))) ;

	connect(partitionView->tableWidgetPartitionView,
		SIGNAL(currentItemChanged( QTableWidgetItem * , QTableWidgetItem * )),
		this,
		SLOT(currentItemChanged( QTableWidgetItem * , QTableWidgetItem * ))) ;

	action = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter );
	keys.append( Qt::Key_Return );
	action->setShortcuts( keys ) ;
	connect(action,
		SIGNAL(triggered()),
		this,
		SLOT(EnterKeyPressed()));
	this->addAction( action );

	QTableWidget *tw = partitionView->tableWidgetPartitionView ;
	tw->setColumnWidth(0,90);
	tw->setColumnWidth(1,90);
	tw->setColumnWidth(2,90);
	tw->setColumnWidth(3,90);
	tw->setColumnWidth(4,250);

	tw->horizontalHeaderItem(0)->setFont(this->font());
	tw->horizontalHeaderItem(1)->setFont(this->font());
	tw->horizontalHeaderItem(2)->setFont(this->font());
	tw->horizontalHeaderItem(3)->setFont(this->font());
	tw->horizontalHeaderItem(4)->setFont(this->font());

	tw->horizontalHeader()->setVisible(true);

	partitionView->checkBoxUUID->setFont(this->font());
}

void openpartition::EnterKeyPressed()
{	
	QTableWidget *tw = partitionView->tableWidgetPartitionView ;
	QTableWidgetItem *it = tw->currentItem() ;
	if( it == NULL )
		return ;
	tableEntryDoubleClicked(tw->item(it->row(),0));
}

void openpartition::ShowNonSystemPartitionsFinished()
{
	delete nonsystempartitionlist ;
	if(partitionView->tableWidgetPartitionView->rowCount() > 0){
		HighlightRow(0,true) ;
		partitionView->tableWidgetPartitionView->setCurrentCell(0,2);
	}
}

void openpartition::ShowSystemPartitionsFinished()
{
	delete partitionlist ;
	if(partitionView->tableWidgetPartitionView->rowCount() > 0){
		HighlightRow(0,true) ;
		partitionView->tableWidgetPartitionView->setCurrentCell(0,2);
	}
}

void openpartition::currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
	HighlightRow(current->row(), true) ;
	if(previous != NULL)
		if(previous->row() != current->row())
			HighlightRow(previous->row(), false) ;
}

void openpartition::HighlightRow(int r, bool b)
{
	partitionView->tableWidgetPartitionView->item(r,0)->setSelected(b);
	partitionView->tableWidgetPartitionView->item(r,1)->setSelected(b);
	partitionView->tableWidgetPartitionView->item(r,2)->setSelected(b);
	partitionView->tableWidgetPartitionView->item(r,3)->setSelected(b);
	partitionView->tableWidgetPartitionView->item(r,4)->setSelected(b);
	if(b==true)
		partitionView->tableWidgetPartitionView->setCurrentCell(r,4);
}

void openpartition::ShowNonSystemPartitions()
{
	this->setWindowTitle(tr("select a partition to create an encrypted volume in"));
	nonsystempartitionlist  = new
			ShowNonSystemPartitionsThread(partitionView,this->font());

	connect(nonsystempartitionlist,
		SIGNAL(finished()),
		this,
		SLOT(ShowNonSystemPartitionsFinished())) ;
	nonsystempartitionlist->start();
	this->show();
}

void openpartition::ShowUI()
{	
	this->setWindowTitle(tr("select an encrypted partition to open"));
	partitionlist = new partitionlistThread(partitionView,this->font()) ;
	connect(partitionlist,
		SIGNAL(finished()),
		this,
		SLOT(ShowSystemPartitionsFinished()));
	partitionlist->start();
	this->show();
}

void openpartition::HideUI()
{
	this->hide();
	emit HideUISignal(this);
}

void openpartition::tableEntryDoubleClicked(QTableWidgetItem * item)
{
	QString dev ;
	QTableWidget *tw = partitionView->tableWidgetPartitionView ;

	if(partitionView->checkBoxUUID->isChecked() == true)
		dev = QString("UUID=\"") + tw->item(item->row(),4)->text() + QString("\"") ;
	else
		dev = tw->item(item->row(),0)->text() ;

	emit clickedPartition( dev ) ;
	HideUI();
}

openpartition::~openpartition()
{
	delete partitionView ;
	delete action ;
}

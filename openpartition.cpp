
/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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
using namespace std ;

openpartition::openpartition(QWidget *parent ) : QDialog(parent)
{
	partitionView.setupUi(this);
	this->setFixedSize(this->size());
	connect(partitionView.tableWidgetPartitionView,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(tableEntryDoubleClicked(int,int))) ;	
}

void openpartition::ShowNonSystemPartitions(QStringList l)
{
	int i ;
	int y = partitionView.tableWidgetPartitionView->rowCount() ;

	for( int i = 0 ; i < y  ; i++ )
	{
		partitionView.tableWidgetPartitionView->removeRow(0);
	}

	partitionView.tableWidgetPartitionView->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("non system partitions")));
	partitionView.tableWidgetPartitionView->removeColumn(1);

	partitionView.tableWidgetPartitionView->setColumnWidth(0,540);

	QTableWidgetItem * t ;

	for ( i = 0 ; i < l.size() - 1 ; i++){

		t = new QTableWidgetItem(l.at(i)) ;
		t->setTextAlignment(Qt::AlignCenter);

		partitionView.tableWidgetPartitionView->insertRow(i);
		partitionView.tableWidgetPartitionView->setItem(i,0,t);
	}
	this->show();
}

void openpartition::ShowUI()
{
	int y = partitionView.tableWidgetPartitionView->rowCount() ;

	for( int i = 0 ; i < y  ; i++ )
	{
		partitionView.tableWidgetPartitionView->removeRow(0);
	}

	partitionView.tableWidgetPartitionView->setColumnWidth(0,540);

	QProcess p ;

	p.start(QString("blkid"));
	p.waitForFinished() ;

	QStringList List = QString(p.readAllStandardOutput()).split("\n") ;

	for( int i = 0 ; i < List.size() - 1  ; i++ )
	{
		partitionView.tableWidgetPartitionView->insertRow(i);
		partitionView.tableWidgetPartitionView->setItem(i,0,new QTableWidgetItem(List.at(i)));
	}
	this->show();
}

void openpartition::HideUI()
{
	this->hide();
}

void openpartition::tableEntryDoubleClicked(int row, int column)
{
	QString i = partitionView.tableWidgetPartitionView->item(row,column)->text().split(":").at(0) ;
	HideUI() ;
	emit clickedPartition(i);	
}

openpartition::~openpartition()
{

}

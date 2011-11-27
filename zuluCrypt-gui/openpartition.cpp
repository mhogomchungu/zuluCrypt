
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
#include <QFile>
#include <QThread>

#include <blkid/blkid.h>

#include "../zuluCrypt-cli/executables.h"


openpartition::openpartition(QWidget *parent ) :
	QDialog(parent)
{
	partitionView = new Ui::PartitionView() ;

	partitionView->setupUi(this);

	this->setFixedSize(this->size());

	connect(partitionView->tableWidgetPartitionView,
		SIGNAL(cellDoubleClicked(int,int)),
		this,
		SLOT(tableEntryDoubleClicked(int,int))) ;
}

void openpartition::ShowNonSystemPartitionsFinished()
{
	delete nonsystempartitionlist ;
}

void openpartition::ShowSystemPartitionsFinished()
{
	delete partitionlist ;
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

QString openpartition::deviceProperties(const char *device)
{
	int i ;

	const char * buffer ;

	QString output = QString(device) + QString(":") ;

	blkid_probe dp = blkid_new_probe_from_filename( device ) ;

	blkid_do_probe( dp ) ;

	i = blkid_probe_lookup_value( dp, "LABEL", &buffer, NULL);

	if( i == 0 )
		output = output + QString(" LABEL=\"") + QString( buffer ) + QString("\"    ") ;
	else
		output = output + QString(" LABEL=\"\"    ") ;

	i = blkid_probe_lookup_value( dp, "TYPE", &buffer, NULL);

	if( i == 0 )
		output = output + QString(" TYPE=\"") + QString( buffer ) + QString("\"") ;
	else
		output = output + QString(" TYPE=\"\"") ;

	blkid_free_probe( dp ) ;

	return output ;
}

void openpartition::HideUI()
{
	this->hide();
}

void openpartition::tableEntryDoubleClicked(int row, int column)
{
	QString i = partitionView->tableWidgetPartitionView->item(row,column)->text().split(":").at(0) ;

	emit clickedPartition(i.split(":").at(0));

	HideUI() ;
}

openpartition::~openpartition()
{
	delete partitionView ;
}

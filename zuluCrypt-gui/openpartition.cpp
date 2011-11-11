
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

#include "executables.h"


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
	QProcess p ;

	QString output = QString(device) + QString(":") ;

	p.start(QString(ZULUCRYPTblkid));

	p.waitForFinished() ;

	char *c  = p.readAllStandardOutput().data() ;

	char * d = strstr( c , device) ;

	if( d == NULL)
		return QString(device) + QString(":");

	char *cN = d ;

	while (*cN++ != '\n') { ; }

	*cN = '\0';

	char *e = strstr( d , "LABEL") ;

	if( e == NULL)
		output = output + QString("  LABEL=\"\"") ;
	else{

		char *f = e + 7;

		char Z ;

		while( *f++ != '"') { ; }

		Z = *f ;

		*f = '\0' ;

		output = output + QString("  LABEL=") + QString( e + 6 ) + QString("") ;

		*f = Z ;
	}

	e = strstr( d , "TYPE") ;

	if( e == NULL )
		output = output + QString("  TYPE=\"\"");
	else{
		char *f = e + 6 ;

		char Z ;

		while( *f++ != '"') { ; }

		Z = *f ;

		*f = '\0' ;

		output = output + QString("  TYPE=") + QString( e + 5 ) + QString("") ;

		*f = Z ;
	}

//	e = strstr( d , "UUID") ;

//	if( e == NULL )
//		output = output + QString("  UUID=\"\"");
//	else{
//		char *f = e ;

//		char Z ;

//		while( *f++ != '"') { ; }

//		Z = *f ;

//		*f = '\0' ;

//		output = output + QString("  UUID=") + QString( e + 5 ) + QString("") ;

//		*f = Z ;
//	}
	p.close();

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


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

#include "executables.h"

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

	delete partitionView.tableWidgetPartitionView->horizontalHeaderItem(0);

	partitionView.tableWidgetPartitionView->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("non system partitions( no active entries in fstab )")));
	partitionView.tableWidgetPartitionView->removeColumn(1);

	partitionView.tableWidgetPartitionView->setColumnWidth(0,540);

	QTableWidgetItem * t ;

	for ( i = 0 ; i < l.size() - 1 ; i++){

		t = new QTableWidgetItem(deviceProperties(l.at(i).toAscii().data())) ;
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
	partitionView.tableWidgetPartitionView->removeColumn(1);

	char buffer[64];
	char *c,*d ;
	int i = 0 ;

	QFile f("/proc/partitions");
	f.open(QIODevice::ReadOnly);
	f.readLine(buffer,64 ) ;
	f.readLine(buffer,64 ) ;

	QTableWidgetItem * t ;

	QString partition ;
	QProcess p ;
	QByteArray b ;

/*
  output of "cat /proc/partitions" that produced below code

major minor  #blocks  name

   8        0   78150744 sda
   8        1   11566768 sda1
   8        2          1 sda2 <--- no idea why this entry is here, it doesnt show up anywhere else,skip it
   8        5   66581361 sda5
   8       16  312571224 sdb
   8       17    1044193 sdb1
   8       18          1 sdb2 <---- no idea why this entry is here,it doesnt show up anywhere else,skit it
   8       21  311524416 sdb5
   8       32     250879 sdc
   8       33     250608 sdc1

   only take partitions(sdX,hdY), skip everything else like /dev/loopX
  */
	while( f.atEnd() != true ){
		f.readLine(buffer,64) ;
		c = buffer ;

		while( *++c != '\n' ) { ; }

		*c = '\0' ;

		d = c ;

		while( *--d != ' ' ) { ; }

		d++ ;

		if( strlen( d ) == 3 )
			continue ;

		//if ( *( d - 3 ) == ' ') // skip sda2 and sdb2 above
		//	continue ;

		if( strncmp( d,"sd",2) != 0 && strncmp( d,"hd",2) != 0 )
			continue ;

		strcpy(buffer,"/dev/");
		strcat(buffer, d ) ;

		t = new QTableWidgetItem( deviceProperties( buffer )) ;

		t->setTextAlignment(Qt::AlignCenter);

		partitionView.tableWidgetPartitionView->insertRow(i);
		partitionView.tableWidgetPartitionView->setItem(i,0,t);
		i++ ;
	}
	f.close();
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
	QString i = partitionView.tableWidgetPartitionView->item(row,column)->text().split(":").at(0) ;
	HideUI() ;
	emit clickedPartition(i.split(":").at(0));
}

openpartition::~openpartition()
{

}

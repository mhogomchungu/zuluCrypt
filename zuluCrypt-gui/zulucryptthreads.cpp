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

#include "zulucryptthreads.h"

#include <QProcess>
#include <QFile>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <iostream>
#include <QDir>
#include <cstdio>
#include "zulucrypt.h"

zuluCryptThreads::zuluCryptThreads(QObject *parent) :
    QThread(parent)
{
}

ShowNonSystemPartitionsThread::ShowNonSystemPartitionsThread(
		Ui::PartitionView * p,QFont f)
{
	partitionView = p ;
	font = f ;
}

ClickedRowHighlight::ClickedRowHighlight()
{

}

void ClickedRowHighlight::update(int it, QTableWidget *t, int *r, int c)
{
	row = it ;
	tableWidget = t ;
	selectedRow = r ;
	item_count = c ;
}

void ClickedRowHighlight::run()
{
	if(item_count == 0)
		return ;

	tableWidget->item(row,0)->setSelected(true);
	tableWidget->item(row,1)->setSelected(true);
	tableWidget->item(row,2)->setSelected(true);
	tableWidget->setCurrentItem(tableWidget->item(row,1));

	if(item_count == 1)
		return ;

	int x = *selectedRow ;

	if( row == x )
		return ;

	tableWidget->item(x,0)->setSelected(false);
	tableWidget->item(x,1)->setSelected(false);
	tableWidget->item(x,2)->setSelected(false);

	*selectedRow = row ;
}


void ShowNonSystemPartitionsThread::run()
{
	QProcess p ;
	p.start(QString(ZULUCRYPTzuluCrypt) + QString(" partitions 3"));
	p.waitForFinished() ;

	QStringList l = QString( p.readAllStandardOutput()).split("\n") ;

	p.close();

	int i ;

	QTableWidget *tw = partitionView->tableWidgetPartitionView ;

	while ( tw->rowCount() > 0 ){
		tw->removeRow(0);
	}

	delete tw->horizontalHeaderItem(0);

	QString q(tr("non system partitions( no active entries in fstab )")) ;

	tw->setHorizontalHeaderItem(0,new QTableWidgetItem(q));
	tw->horizontalHeaderItem(0)->setFont(font);
	tw->setColumnWidth(0,540);

	QTableWidgetItem * t ;

	for ( i = 0 ; i < l.size() - 1 ; i++){

		t = new QTableWidgetItem(openpartition::deviceProperties(l.at(i).toAscii().data())) ;
		t->setTextAlignment(Qt::AlignCenter);
		tw->insertRow(i);
		tw->setItem(i,0,t);
	}
}

partitionlistThread::partitionlistThread(Ui::PartitionView * p,QFont f)
{
	partitionView = p ;
	font = f ;
}

void partitionlistThread::run()
{
	QTableWidget *tw = partitionView->tableWidgetPartitionView ;

	while ( tw->rowCount() > 0 ){
		tw->removeRow(0);
	}

	tw->setColumnWidth(0,540);
	tw->removeColumn(1);

	tw->horizontalHeaderItem(0)->setFont(font);

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
   8        2          1 sda2
   8        5   66581361 sda5
   8       16  312571224 sdb
   8       17    1044193 sdb1
   8       18          1 sdb2
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

		t = new QTableWidgetItem( openpartition::deviceProperties( buffer )) ;

		t->setTextAlignment(Qt::AlignCenter);

		tw->insertRow(i);
		tw->setItem(i,0,t);
		i++ ;
	}
	f.close();
}

createFileThread::createFileThread(QString s,QString f,double l,int t)
{
	source = s ;
	file = f ;
	size = l ;
	type = t ;
}

void createFileThread::run()
{
	char data ;

	//QFile blocked when reading from /dev/random for some reason,
	//going back to C API for file access

	FILE * in = fopen( source.toAscii().data(),"r") ;
	FILE * out = fopen( file.toAscii().data(),"w") ;
	double i ;

	if( type == 0 ){
		for( i = 0 ; i < size ; i++){
			do{
				data = fgetc(in) ;
			}while( data < 32 || data > 126) ;

			fputc(data,out) ;
		}
	}else{
		for( i = 0 ; i < size ; i++){
			data = fgetc(in) ;
			fputc(data,out) ;
		}
	}
	fclose(in) ;
	fclose(out) ;
}

volumePropertiesThread::volumePropertiesThread()
{

}

void volumePropertiesThread::update(QString p,QString z,QString *q)
{
	path = p.replace("\"","\"\"\"") ; ;
	mpoint = z ;
	volProperty = q ;
}

void volumePropertiesThread::run()
{
	QString z = QString(ZULUCRYPTzuluCrypt) ;
	z = z + QString(" status ") ;
	z = z + QString("\"") ;
	z = z + path + QString("\"");

	QProcess p ;
	p.start( z ) ;
	p.waitForFinished() ;
	
	QByteArray t("\n") ;
	QByteArray r = p.readAllStandardOutput() ;

	int start = r.length() - r.indexOf(t) - 2 ;

	p.close();

	p.start("df -PTh ");
	
	p.waitForFinished() ;

	QStringList df = QString(p.readAllStandardOutput()).split("\n").filter(mpoint) ;

	p.close();

	QByteArray c ;

	c = df.at(0).toAscii() ;
	/*
	  Below code will attemp to walk through a line like below without
	  caring how many space characters are involved in separating items:

	  /dev/mapper/zuluCrypt-image-02.img ext4   99G   96G  1.9G  99% /home/ink/image-02.img

	*/
	int i = 0 ;
	int j ;
	while ( c.at(i++) != ' ') { ; }
	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	QString y = QString(" fs=\"") + QString(c.mid(j,i - j)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\"\n size=\"") + QString(c.mid(j,i - j)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\"\n used=\"") + QString(c.mid(j,i - j)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\"\n available=\"") + QString(c.mid(j,i - j)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\"\n used%=\"") + QString(c.mid(j,i - j)) ;

	QStringList l = zuluCrypt::luksEmptySlots(path) ;

	if ( zuluCrypt::isLuks(path) == true){
		QString x =  QString(" ") ;
		x = x + QString( r.right(start) ) ;
		x = x + QString(" occupied key slots=\"") ;
		x = x + l.at(0) + QString(" / ") + l.at(1) + QString("\"\n");

		*volProperty = x + y;
	}else
		*volProperty = QString(" ") + QString( r.right(start) ) + y;
}

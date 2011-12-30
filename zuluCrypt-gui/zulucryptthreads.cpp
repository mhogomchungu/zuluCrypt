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
#include "miscfunctions.h"

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
		Ui::PartitionView * partitionView,QFont font)
{
	m_partitionView = partitionView ;
	m_font = font ;
}

void ShowNonSystemPartitionsThread::run()
{
	QProcess p ;
	p.start(QString(ZULUCRYPTzuluCrypt) + QString(" partitions 3"));
	p.waitForFinished() ;

	QStringList l = QString( p.readAllStandardOutput()).split("\n") ;

	p.close();

	int i ;

	QTableWidget *tw = m_partitionView->tableWidget ;

	while ( tw->rowCount() > 0 ){
		tw->removeRow(0);
	}

	QTableWidgetItem * t ;
	QStringList prp ;

	for ( i = 0 ; i < l.size() - 1 ; i++){
		prp = miscfunctions::deviceProperties(l.at(i)) ;
		tw->insertRow(i);
		t = new QTableWidgetItem( prp.at(0) ) ;
		t->setTextAlignment(Qt::AlignCenter);
		tw->setItem(i,0,t);
		t = new QTableWidgetItem( prp.at(1) ) ;
		t->setTextAlignment(Qt::AlignCenter);
		tw->setItem(i,1,t);
		t = new QTableWidgetItem( prp.at(2) ) ;
		t->setTextAlignment(Qt::AlignCenter);
		tw->setItem(i,2,t);
		t = new QTableWidgetItem( prp.at(3) ) ;
		t->setTextAlignment(Qt::AlignCenter);
		tw->setItem(i,3,t);
		t = new QTableWidgetItem( prp.at(4) ) ;
		t->setTextAlignment(Qt::AlignCenter);
		tw->setItem(i,4,t);
	}
}

partitionlistThread::partitionlistThread(Ui::PartitionView * p,QFont f)
{
	m_partitionView = p ;
	m_font = f ;
}

void partitionlistThread::run()
{
	QTableWidget *tw = m_partitionView->tableWidget ;

	while ( tw->rowCount() > 0 ){
		tw->removeRow(0);
	}
	char buffer[64];
	QFile f("/proc/partitions");
	f.open(QIODevice::ReadOnly);
	f.readLine(buffer,64 ) ;
	f.readLine(buffer,64 ) ;
/*
  output of "cat /proc/partitions" that produced below code

major minor  #blocks  name

   8\t0   78150744 sda
   8\t1   11566768 sda1
   8\t2\t  1 sda2
   8\t5   66581361 sda5
   8       16  312571224 sdb
   8       17    1044193 sdb1
   8       18\t  1 sdb2
   8       21  311524416 sdb5
   8       32     250879 sdc
   8       33     250608 sdc1

   only take partitions(sdX,hdY), skip everything else like /dev/loopX
  */


	QTableWidgetItem * t ;

	QString partition ;
	QProcess p ;
	QByteArray b ;
	QStringList prp ;
	char * c,* d ;
	int i = 0 ;
	strcpy(buffer,"/dev/");
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

		if( strncmp( d,"sd",2) != 0 && strncmp( d,"hd",2) != 0 )
			continue ;

		strcpy(buffer,"/dev/");
		strcat( buffer, d ) ;

		prp = miscfunctions::deviceProperties( QString( buffer ) ) ;
		tw->insertRow(i);
		t = new QTableWidgetItem( prp.at(0) ) ;
		t->setTextAlignment(Qt::AlignCenter);
		tw->setItem(i,0,t);
		t = new QTableWidgetItem( prp.at(1) ) ;
		t->setTextAlignment(Qt::AlignCenter);
		tw->setItem(i,1,t);
		t = new QTableWidgetItem( prp.at(2) ) ;
		t->setTextAlignment(Qt::AlignCenter);
		tw->setItem(i,2,t);
		t = new QTableWidgetItem( prp.at(3) ) ;
		t->setTextAlignment(Qt::AlignCenter);
		tw->setItem(i,3,t);
		t = new QTableWidgetItem( prp.at(4) ) ;
		t->setTextAlignment(Qt::AlignCenter);
		tw->setItem(i,4,t);
		i++ ;
	}
	f.close();
}

createFileThread::createFileThread(QString s,QString f,double l,int t)
{
	m_source = s ;
	m_file = f ;
	m_size = l ;
	m_type = t ;
}

void createFileThread::run()
{
	char data ;

	//QFile blocked when reading from /dev/random for some reason,
	//going back to C API for file access

	FILE * in = fopen( m_source.toAscii().data(),"r") ;
	FILE * out = fopen( m_file.toAscii().data(),"w") ;
	double i ;

	if( m_type == 0 ){
		for( i = 0 ; i < m_size ; i++){
			do{
				data = fgetc(in) ;
			}while( data < 32 || data > 126) ;

			fputc(data,out) ;
		}
	}else{
		for( i = 0 ; i < m_size ; i++){
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
	m_path = p.replace("\"","\"\"\"") ; ;
	m_mpoint = z ;
	m_volProperty = q ;
}

void volumePropertiesThread::run()
{
	QString z = QString(ZULUCRYPTzuluCrypt) ;
	z = z + QString(" status ") ;
	z = z + QString("\"") ;
	z = z + m_path + QString("\"");

	QProcess p ;
	p.start( z ) ;
	p.waitForFinished() ;
	
	QByteArray t("\n") ;
	QByteArray r = p.readAllStandardOutput() ;

	int start = r.length() - r.indexOf(t) - 2 ;

	p.close();

	p.start("df -PTh ");
	
	p.waitForFinished() ;

	QStringList df = QString(p.readAllStandardOutput()).split("\n").filter(m_mpoint) ;

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

	QString y = QString(" fs:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\n size:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\n used:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\n available:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\n used%:\t") + QString(c.mid(j,i - j - 1));

	QStringList l = miscfunctions::luksEmptySlots(m_path) ;

	if ( miscfunctions::isLuks(m_path) == true){
		QString x =  QString(" ") ;
		x = x + QString( r.right(start) ) ;
		x = x + QString(" used slots:\t") ;
		x = x + l.at(0) + QString(" / ") + l.at(1) + QString("\n");

		*m_volProperty = x + y;
	}else
		*m_volProperty = QString(" ") + QString( r.right(start) ) + y;
}

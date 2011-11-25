
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

runInThread::runInThread(QString exe, int * st)
{
	EXE = exe ;
	status = st ;
}

void runInThread::run()
{
	QProcess p ;

	p.start(EXE);

	p.waitForFinished() ;

	*status = p.exitCode() ;

	p.close();
}


rngThread::rngThread(QString rn,QString key)
{
	rng = rn ;
	keyfile = key ;
}

void rngThread::run()
{
	char data ;

	//QFile blocked when reading from /dev/random for some reason,
	//going back to C API for file access

	FILE * in = fopen( rng.toAscii().data(),"r") ;

	FILE * out = fopen( keyfile.toAscii().data(),"w") ;

	for( int i = 0 ; i < 64 ; i++){

		do{
			data = fgetc(in) ;
		}while( data < 32 || data > 126) ;

		fputc(data,out) ;
	}
	fclose(in) ;
	fclose(out) ;
}

volumePropertiesThread::volumePropertiesThread(QString p,QString z,QString *q)
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

	QString exe = QString("df -PTh ")  ;

	p.start(exe);

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

	QString y = QString(" fs:  ") + QString(c.mid(j,i - j)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\n size:  ") + QString(c.mid(j,i - j)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\n used:  ") + QString(c.mid(j,i - j)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\n available:  ") + QString(c.mid(j,i - j)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\n used%:  ") + QString(c.mid(j,i - j)) ;

	QStringList l = zuluCrypt::luksEmptySlots(path) ;

	if ( zuluCrypt::isLuks(path) == true){
		QString x =  QString(" ") ;
		x = x + QString( r.right(start) ) ;
		x = x + QString(" occupied key slots: ") ;
		x = x + l.at(0) + QString(" / ") + l.at(1) + QString("\n");

		*volProperty = x + y;
	}else
		*volProperty = QString(" ") + QString( r.right(start) ) + y;
}

checkSystemTools::checkSystemTools(QString *s)
{
	output = s ;
}

void checkSystemTools::run()
{
	QProcess p ;

	p.start(ZULUCRYPTzuluCrypt);

	p.waitForFinished() ;

	if(  p.exitCode() == 100 ){

		QString warning = tr("this program will not work as expected on your system ");
		warning = warning + tr("because one or more of the following tools are either not present ") ;
		warning = warning + tr("or not where they are expected to be.\n") ;

		warning = warning + QString(ZULUCRYPTcryptsetup) + QString("\n") ;
		warning = warning + QString(ZULUCRYPTblkid) + QString("\n") ;
		warning = warning + QString(ZULUCRYPTecho) + QString("\n") ;
		warning = warning + QString(ZULUCRYPTlosetup) + QString("\n") ;
		warning = warning + QString(ZULUCRYPTmkfs) + QString("\n") ;
		warning = warning + QString(ZULUCRYPTmount) + QString("\n") ;
		warning = warning + QString(ZULUCRYPTumount) + QString("\n") ;
		warning = warning + QString(ZULUCRYPTdd) + QString("\n") ;
		warning = warning + QString(ZULUCRYPTzuluCrypt) + QString("\n") ;

		*output = warning ;
	}else
		*output = QString("") ;
	p.close();
}

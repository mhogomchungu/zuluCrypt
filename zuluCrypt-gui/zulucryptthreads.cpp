
#include "zulucryptthreads.h"
#include "openpartition.h"

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

	int y = tw->rowCount() ;

	for( int i = 0 ; i < y  ; i++ )
	{
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

	int y = tw->rowCount() ;

	for( int i = 0 ; i < y  ; i++ )
	{
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

createvolumeThread::createvolumeThread(Ui::createpartition * p,int *s)
{
	ui = p ;
	status = s ;
}

void createvolumeThread::run()
{
	QString fs ;

	if( ui->rbext3->isChecked() == true)
		fs = QString("ext3") ;
	else if( ui->rbext4->isChecked() == true)
		fs = QString("ext4") ;
	else
		fs = QString("vfat") ;

	QString ct ;

	if(ui->rbPlain->isChecked() == true)
		ct = QString("plain");
	else
		ct = QString("luks");

	QString N ;

	QString passphrase = ui->lineEditPassphrase1->text() ;

	if (ui->rbPassphraseFromFile->isChecked() == true)
		N = QString("-f") ;
	else{
		N = QString("-p") ;

		for( int i = 0 ; i < passphrase.size() ; i++){

			if( passphrase.at(i).toAscii() == '\"'){
				passphrase.insert(i,QString("\"\""));
				i = i + 2 ;
			}
		}
	}

	QFile f(QDir::homePath() + QString("/.zuluCrypt/rng")) ;

	f.open(QIODevice::ReadOnly) ;

	QByteArray q = f.readAll() ;

	f.close();

	QString exe = QString(ZULUCRYPTzuluCrypt) ;
	exe = exe + QString(" create \"") ;
	exe = exe + ui->lineEditVolumePath->text() + QString("\" ") ;
	exe = exe + fs + QString(" ") ;
	exe = exe + ct + QString(" ") ;
	exe = exe +  N + QString(" \"") ;;
	exe = exe + passphrase + QString("\" ") ;;
	exe = exe + QString( q );

	QProcess p ;

	p.start(exe);
	p.waitForFinished();
	*status = p.exitCode() ;
	p.close();
}

luksdeleteKeyThread::luksdeleteKeyThread(Ui::luksdeletekey *UI, int *s)
{
	ui = UI ;
	status = s ;
}

void luksdeleteKeyThread::run()
{
	QString exe = QString(ZULUCRYPTzuluCrypt) ;
	exe = exe + QString(" removekey ")  ;
	exe = exe + QString("\"") +  ui->lineEditVolumePath->text() + QString("\"") ;

	QString pass = ui->lineEditPassphrase->text() ;

	if ( ui->rbPassphraseFromFile->isChecked() == true ){

		exe = exe + QString(" -f ") ;

	}else{
		exe = exe + QString(" -p ") ;

		for( int i = 0 ; i < pass.size() ; i++){

			if( pass.at(i).toAscii() == '\"'){
				pass.insert(i,QString("\"\""));
				i = i + 2 ;
			}
		}
	}

	exe = exe + QString(" \"") + pass + QString("\"") ;

	QProcess Z ;

	Z.start( exe );

	Z.waitForFinished() ;

	*status = Z.exitCode() ;

	Z.close();
}

luksAddKeyThread::luksAddKeyThread(QString e, int *s)
{
	exe = e ;
	status = s ;
}

void luksAddKeyThread::run()
{
	QProcess Z ;

	Z.start(exe);

	Z.waitForFinished() ;

	*status = Z.exitCode() ;
}

openVolumeThread::openVolumeThread(QString e, int *s)
{
	exe = e ;
	status = s ;
}

void openVolumeThread::run()
{
	QProcess p ;

	p.start(exe) ;

	p.waitForFinished() ;

	*status= p.exitCode() ;

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

volumePropertiesThread::volumePropertiesThread(QString p,QString *q)
{
	path = p ;
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

	if ( zuluCrypt::isLuks(path) == true){
		QString x =  QString(" ") ;
		x = x + QString( r.right(start) ) ;
		x = x + QString(" occupied key slots: ") ;
		x = x + zuluCrypt::luksEmptySlots(path) ;
		x = x + QString(" / 8") ;

		*volProperty = x ;
	}else
		*volProperty = ( QString(" ") + QString( r.right(start) ) ) ;
}


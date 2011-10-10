
#include "zulucryptthreads.h"
#include "openpartition.h"
#include "zulucrypt.h"

#include <QProcess>
#include <QFile>
#include <QMessageBox>
#include <QTableWidgetItem>

zuluCryptThreads::zuluCryptThreads(QObject *parent) :
    QThread(parent)
{
}

ShowNonSystemPartitionsThread::ShowNonSystemPartitionsThread(Ui::PartitionView * p)
{
	partitionView = p ;
}

void ShowNonSystemPartitionsThread::run()
{
	QProcess p ;
	p.start(QString(ZULUCRYPTzuluCrypt) + QString(" partitions 3"));
	p.waitForFinished() ;

	QStringList l = QString( p.readAllStandardOutput()).split("\n") ;

	p.close();

	int i ;
	int y = partitionView->tableWidgetPartitionView->rowCount() ;

	for( int i = 0 ; i < y  ; i++ )
	{
		partitionView->tableWidgetPartitionView->removeRow(0);
	}

	delete partitionView->tableWidgetPartitionView->horizontalHeaderItem(0);

	partitionView->tableWidgetPartitionView->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("non system partitions( no active entries in fstab )")));
	partitionView->tableWidgetPartitionView->removeColumn(1);

	partitionView->tableWidgetPartitionView->setColumnWidth(0,540);

	QTableWidgetItem * t ;

	for ( i = 0 ; i < l.size() - 1 ; i++){

		t = new QTableWidgetItem(openpartition::deviceProperties(l.at(i).toAscii().data())) ;
		t->setTextAlignment(Qt::AlignCenter);

		partitionView->tableWidgetPartitionView->insertRow(i);
		partitionView->tableWidgetPartitionView->setItem(i,0,t);
	}
}

partitionlistThread::partitionlistThread(Ui::PartitionView * p)
{
	partitionView = p ;
}

void partitionlistThread::run()
{
	int y = partitionView->tableWidgetPartitionView->rowCount() ;

	for( int i = 0 ; i < y  ; i++ )
	{
		partitionView->tableWidgetPartitionView->removeRow(0);
	}

	partitionView->tableWidgetPartitionView->setColumnWidth(0,540);
	partitionView->tableWidgetPartitionView->removeColumn(1);

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

		t = new QTableWidgetItem( openpartition::deviceProperties( buffer )) ;

		t->setTextAlignment(Qt::AlignCenter);

		partitionView->tableWidgetPartitionView->insertRow(i);
		partitionView->tableWidgetPartitionView->setItem(i,0,t);
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

	QString exe = QString(ZULUCRYPTzuluCrypt) + " create \"" + ui->lineEditVolumePath->text() + "\" " + fs + " " + ct + " " +  N + " \"" + passphrase + "\"" ;

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
	QString exe = QString(ZULUCRYPTzuluCrypt) + QString(" removekey ")  + "\"" +  ui->lineEditVolumePath->text()+ "\"" ;

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

rngThread::rngThread(QFile *i, QFile *o)
{
	in = i ;
	out = o ;
}

void rngThread::run()
{
	char data ;

	for( int i = 0 ; i < 32 ; i++){

		do{
			in->getChar(&data) ;
		}while( data < 32 || data > 126) ;

		out->putChar(data) ;
	}

	in->close();
	out->close();
}

volumePropertiesThread::volumePropertiesThread(QString p,QString *q)
{
	path = p ;
	volProperty = q ;
}

void volumePropertiesThread::run()
{
	QString z = QString(ZULUCRYPTzuluCrypt) + QString(" status ") + QString("\"") + path + QString("\"");

	QProcess p ;

	p.start( z ) ;

	p.waitForFinished() ;

	QByteArray t("\n") ;

	QByteArray r = p.readAllStandardOutput() ;

	int start = r.length() - r.indexOf(t) - 2 ;

	p.close();

	if ( zuluCrypt::isLuks(path) == true)
		*volProperty = ( QString(" ") + QString( r.right(start) )   + QString("  occupied key slots: ") + zuluCrypt::luksEmptySlots(path) + QString(" / 8")) ;
	else
		*volProperty = ( QString(" ") + QString( r.right(start) ) ) ;
}



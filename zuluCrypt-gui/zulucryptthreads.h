#ifndef ZULUCRYPTTHREADS_H
#define ZULUCRYPTTHREADS_H

#include "executables.h"
#include "ui_openpartition.h"
#include "ui_createpartition.h"
#include "ui_luksdeletekey.h"
#include "ui_luksaddkey.h"
#include "ui_password.h"

#include <QThread>
#include <QFile>
#include <QMetaType>



//qRegisterMetaType(Qt::Orientation);

class zuluCryptThreads : public QThread
{
public:
    explicit zuluCryptThreads(QObject *parent = 0);
};

class ShowNonSystemPartitionsThread : public QThread
{
public :
	void run() ;
	ShowNonSystemPartitionsThread(Ui::PartitionView *,QFont) ;
private:
	Ui::PartitionView * partitionView ;
	QFont font ;
};

class partitionlistThread : public QThread
{
public :
	void run() ;
	partitionlistThread(Ui::PartitionView *,QFont) ;
private:
	Ui::PartitionView * partitionView ;
	QFont font ;
};

class createvolumeThread : public QThread
{
public :
	void run() ;
	createvolumeThread(Ui::createpartition *,int *) ;
private:
	Ui::createpartition *ui;
	int * status ;
};

class luksdeleteKeyThread : public QThread
{
public :
	void run() ;
	luksdeleteKeyThread(Ui::luksdeletekey *,int *) ;
private:
	Ui::luksdeletekey *ui;
	int * status ;
};

class luksAddKeyThread : public QThread
{
public :
	void run() ;
	luksAddKeyThread(QString,int *) ;
private:
	QString exe ;
	int * status ;
};

class openVolumeThread : public QThread
{
public :
	void run() ;
	openVolumeThread(QString,int *) ;
private:
	QString exe ;
	int * status ;
};

class rngThread : public QThread
{
public :
	void run() ;
	rngThread(QString,QString) ;
private:
	QString rng ;
	QString keyfile ;

};

class volumePropertiesThread : public QThread
{
public :
	void run() ;
	volumePropertiesThread(QString,QString *) ;
private:
	QString path ;
	QString *volProperty ;

};



#endif // ZULUCRYPTTHREADS_H

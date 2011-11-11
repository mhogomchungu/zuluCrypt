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
	createvolumeThread(QString,int *) ;
private:
	void run() ;
	QString exe;
	int * status ;
};

class luksdeleteKeyThread : public QThread
{
public :	
	luksdeleteKeyThread(QString,int *) ;
private:
	void run() ;
	QString exe;
	int * status ;
};

class luksAddKeyThread : public QThread
{
public :	
	luksAddKeyThread(QString,int *) ;
private:
	void run() ;
	QString exe ;
	int * status ;
};

class openVolumeThread : public QThread
{
public :
	openVolumeThread(QString,int *) ;
private:
	void run() ;
	QString exe ;
	int * status ;
};

class rngThread : public QThread
{
public :
	rngThread(QString,QString) ;
private:
	void run() ;
	QString rng ;
	QString keyfile ;

};

class volumePropertiesThread : public QThread
{
public :
	volumePropertiesThread(QString,QString,QString *) ;
private:
	void run() ;
	QString path ;
	QString mpoint ;
	QString *volProperty ;

};

class checkSystemTools : public QThread
{
public:
	checkSystemTools(QString *) ;
private:
	void run() ;
	QString *output ;
};

#endif // ZULUCRYPTTHREADS_H

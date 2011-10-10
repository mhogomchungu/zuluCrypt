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

class zuluCryptThreads : public QThread
{
public:
    explicit zuluCryptThreads(QObject *parent = 0);
};

class ShowNonSystemPartitionsThread : public QThread
{
public :
	void run() ;
	ShowNonSystemPartitionsThread(Ui::PartitionView *) ;
private:
	Ui::PartitionView * partitionView ;
};

class partitionlistThread : public QThread
{
public :
	void run() ;
	partitionlistThread(Ui::PartitionView *) ;
private:
	Ui::PartitionView * partitionView ;
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
	rngThread(QFile *in,QFile *out) ;
private:
	QFile *in ;
	QFile *out ;

};

Q_DECLARE_METATYPE(Qt::Orientation) ;
#endif // ZULUCRYPTTHREADS_H

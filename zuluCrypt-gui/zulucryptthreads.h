#ifndef ZULUCRYPTTHREADS_H
#define ZULUCRYPTTHREADS_H

#include "../zuluCrypt-cli/executables.h"
#include "ui_openpartition.h"

#include <QThread>
#include <QFile>
#include <QMetaType>

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

class runInThread : public QThread
{
public:
	runInThread(QString,int *) ;
private:
	void run(void) ;
	int * status ;
	QString EXE ;
};

#endif // ZULUCRYPTTHREADS_H

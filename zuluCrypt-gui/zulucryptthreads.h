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

class createFileThread : public QThread
{
public :
	createFileThread(QString source,QString destination,double size,int type) ;
private:
	void run() ;
	QString source ;
	QString file ;
	double size ;
	int type ;

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

#ifndef PARTITIONPROPERTIES_H
#define PARTITIONPROPERTIES_H

#include <QThread>
#include <QTableWidget>
#include <QProcess>
#include <QString>
#include <QStringList>
#include "miscfunctions.h"

class partitionproperties : public QThread
{
	Q_OBJECT
public:
	explicit partitionproperties(int,QObject *parent = 0);
	
signals:
	void finished(partitionproperties *);
	void partitionProperties(QStringList);
public slots:
private:
	void run(void);
	int m_partitionType ;
};

#endif // PARTITIONPROPERTIES_H

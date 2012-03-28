#ifndef PARTITIONPROPERTIES_H
#define PARTITIONPROPERTIES_H

#include <QRunnable>
#include <QTableWidget>
#include <QProcess>
#include <QString>
#include <QStringList>
#include "miscfunctions.h"

class partitionproperties : public QObject, public QRunnable
{
	Q_OBJECT
public:
	explicit partitionproperties(QString);
	~partitionproperties();
signals:
	void finished();
	void partitionProperties(QStringList);
public slots:
private:
	void run(void);
	QString m_partitionType ;
};

#endif // PARTITIONPROPERTIES_H

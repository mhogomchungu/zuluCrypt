#ifndef VOLUMEPROPERTIESTHREAD_H
#define VOLUMEPROPERTIESTHREAD_H

#include <QThread>
#include "miscfunctions.h"
#include <QString>
#include <QStringList>

class volumePropertiesThread : public QThread
{
	Q_OBJECT
public:
	explicit volumePropertiesThread(QString,QString,QObject *parent = 0);
signals:
	void finished(QString,volumePropertiesThread *) ;
public slots:	
private:
	void run(void);
	QString m_path ;
	QString m_mpoint ;
};

#endif // VOLUMEPROPERTIESTHREAD_H

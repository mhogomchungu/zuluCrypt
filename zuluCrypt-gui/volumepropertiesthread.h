#ifndef VOLUMEPROPERTIESTHREAD_H
#define VOLUMEPROPERTIESTHREAD_H

#include <QRunnable>
#include "miscfunctions.h"
#include <QString>
#include <QStringList>

class volumePropertiesThread : public QObject, public QRunnable
{
	Q_OBJECT
public:
	explicit volumePropertiesThread(QString,QString);
signals:
	void finished(QString) ;
public slots:	
private:
	void run(void);
	QString m_path ;
	QString m_mpoint ;
};

#endif // VOLUMEPROPERTIESTHREAD_H

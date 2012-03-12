#ifndef CHECKVOLUMETYPE_H
#define CHECKVOLUMETYPE_H

#include <QRunnable>
#include <QObject>
#include <QString>

#include "miscfunctions.h"

class checkvolumetype : public QObject,public QRunnable
{
	Q_OBJECT
public:
	checkvolumetype(QString);
	~checkvolumetype() ;
signals:
	void done(QString);
private:
	QString m_device ;
	QString m_type ;
	void run(void);
};

#endif // CHECKVOLUMETYPE_H

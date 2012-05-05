#ifndef ERASEDEVICETHREAD_H
#define ERASEDEVICETHREAD_H

#include <QObject>
#include <QRunnable>
#include <QString>
#include <QProcess>
#include <QThreadPool>
#include <QTimer>

#include "miscfunctions.h"
#include "runinthread.h"
#include "../zuluCrypt-cli/constants.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <blkid/blkid.h>
#include <string.h>

class erasedevicethread : public QObject,public QRunnable
{
	Q_OBJECT
public:
	explicit erasedevicethread(QString);
	void start(void);
	~erasedevicethread();
signals:
	void progress(int);
	void exitStatus(int) ;
public slots:
	void cancel(void);
private:
	int openMapper(void);
	void writeJunkThroughMapper(void);
	void closeMapper(void);
	void run(void);
	int m_status ;
	QString m_path ;
};

#endif // ERASEDEVICETHREAD_H

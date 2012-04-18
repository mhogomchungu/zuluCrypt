#ifndef ERASEDEVICETHREAD_H
#define ERASEDEVICETHREAD_H

#include <QThread>
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

class erasedevicethread : public QThread
{
	Q_OBJECT
public:
	explicit erasedevicethread(QString);
	~erasedevicethread();
signals:
	void progress(int);
	void exitStatus(int) ;
public slots:
	void cancel(void);
	void progressTimer(void);
private:
	int openMapper(void);
	void writeJunkThroughMapper(void);
	void closeMapper(void);
	void run(void);
	int m_status ;
	int m_id ;
	QString m_path ;
	QTimer * m_timer ;
	double m_size ;
	double m_size_written ;
	int m_ratio ;
	int m_prev_ratio  ;
};

#endif // ERASEDEVICETHREAD_H

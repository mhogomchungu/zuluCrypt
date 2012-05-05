#ifndef CLOSEVOLUMETHREAD_H
#define CLOSEVOLUMETHREAD_H

#include <QThreadPool>
#include <QRunnable>
#include <QString>
#include <QProcess>
#include <unistd.h>

class closeVolumeThread : public QObject, public QRunnable
{
	Q_OBJECT
public:
	explicit closeVolumeThread(QString);
	void start(void);
	~ closeVolumeThread() ;
signals:
	void finished(int) ;
private:
	void run(void) ;
	QString m_exe ;
	int m_status ;
};

#endif // CLOSEVOLUMETHREAD_H

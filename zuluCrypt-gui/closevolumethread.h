#ifndef CLOSEVOLUMETHREAD_H
#define CLOSEVOLUMETHREAD_H

#include <QThread>
#include <QString>
#include <QProcess>

class closeVolumeThread : public QThread
{
	Q_OBJECT
public:
	explicit closeVolumeThread(QString);
signals:
	void finished( closeVolumeThread *,int) ;
private:
	void run(void) ;
	QString m_exe ;
};

#endif // CLOSEVOLUMETHREAD_H

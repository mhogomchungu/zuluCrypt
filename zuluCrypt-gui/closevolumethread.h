#ifndef CLOSEVOLUMETHREAD_H
#define CLOSEVOLUMETHREAD_H

#include <QRunnable>
#include <QString>
#include <QProcess>

class closeVolumeThread : public QObject, public QRunnable
{
	Q_OBJECT
public:
	explicit closeVolumeThread(QString);
signals:
	void finished(int) ;
private:
	void run(void) ;
	QString m_exe ;
};

#endif // CLOSEVOLUMETHREAD_H

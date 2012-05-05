#ifndef CREATEKEYFILETHREAD_H
#define CREATEKEYFILETHREAD_H

#include <QObject>
#include <QRunnable>
#include <QThreadPool>
#include <QProcess>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

class createkeyfilethread : public QThread,public QRunnable
{
	Q_OBJECT
public:
	explicit createkeyfilethread(QString,QString);
	~createkeyfilethread();
	void start(void);
signals:
	void exitStatus(int);
public slots:
	void cancelOperation(void);
private:
	void run();
	QString m_path ;
	QString m_rng ;
	int m_cancelled ;
	int m_in ;
	int m_out ;
};

#endif // CREATEKEYFILETHREAD_H

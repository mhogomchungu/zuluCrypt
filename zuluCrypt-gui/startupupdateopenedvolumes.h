#ifndef STARTUPUPDATEOPENEDVOLUMES_H
#define STARTUPUPDATEOPENEDVOLUMES_H

#include <QThread>

class startupupdateopenedvolumes : public QThread
{
	Q_OBJECT
public:
	explicit startupupdateopenedvolumes(QObject *parent = 0);

signals:
	void addItemToTable(QString,QString) ;
	void UIMessage(QString title,QString message) ;
public slots:

private:
	void run() ;
};

#endif // STARTUPUPDATEOPENEDVOLUMES_H

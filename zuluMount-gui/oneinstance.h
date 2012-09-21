#ifndef ONEINSTANCE_H
#define ONEINSTANCE_H

#include <Qt>
#include <QObject>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QFile>
#include <QCoreApplication>
#include <QMainWindow>
#include <QString>
#include <QMetaObject>

class oneinstance : public QObject
{
	Q_OBJECT
public:
	explicit oneinstance( QObject * parent,QString socketPath,const char * );
	~oneinstance() ;
	bool instanceExist( void ) ;
signals:
	void raise( void ) ;
public slots:
	void connected( void ) ;
	void gotConnection( void ) ;
private slots:
	void Exit( void ) ;
private:
	void killProcess( void ) ;
	QLocalServer * m_localServer ;
	QLocalSocket * m_localSocket ;
	QString m_serverPath ;
	bool m_instanceExist ;
};

#endif // ONEINSTANCE_H

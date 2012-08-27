#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QObject>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow( QWidget * parent = 0 ) ;
	void SetAddr( QString ) ;
	~MainWindow();
private slots:
	void pbOpen( void ) ;
	void pbCancel( void ) ;
	void pbKeyFile( void ) ;
	void acceptConnection( void ) ;
private:
	void SetFocus( void );
	void Exit( void ) ;
	Ui::MainWindow * m_ui;
	QString m_addr ;
	QString m_keyFile ;
	QLocalSocket * m_client ;
	QLocalServer * m_server ;
};

#endif // MAINWINDOW_H

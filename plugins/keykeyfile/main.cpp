#include <QtGui/QApplication>
#include "mainwindow.h"

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv );
	MainWindow w;
	w.SetAddr( QString( argv[ 3 ] ) ) ;
	w.show();

	return a.exec();
}

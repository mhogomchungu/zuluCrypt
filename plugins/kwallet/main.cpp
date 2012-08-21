

#include <QtCore/QCoreApplication>
#include "wallet.h"

int main( int argc,char * argv[] )
{
	QCoreApplication a( argc,argv );

	wallet w( argc,argv ) ;

	QMetaObject::invokeMethod( &w,"openWallet",Qt::QueuedConnection );

	return a.exec() ;
}

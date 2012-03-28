#include "partitionproperties.h"

partitionproperties::partitionproperties(QString partitionType)
{
	m_partitionType = partitionType ;
}

void partitionproperties::run()
{
	QProcess p ;

	p.start(QString(ZULUCRYPTzuluCrypt) + m_partitionType);

	p.waitForFinished() ;

	QStringList l = QString( p.readAllStandardOutput()).split("\n") ;

	p.close();

	for (int i = 0 ; i < l.size() - 1 ; i++)
		emit partitionProperties(miscfunctions::deviceProperties(l.at(i)));
}

partitionproperties::~partitionproperties()
{
	emit finished();
}

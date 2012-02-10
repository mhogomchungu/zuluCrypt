#include "partitionproperties.h"

partitionproperties::partitionproperties(int partitionType)
{
	m_partitionType = partitionType ;
}

void partitionproperties::run()
{
	QProcess p ;

	p.start(QString(ZULUCRYPTzuluCrypt) + \
		QString(" partitions ") + QString::number(m_partitionType));

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

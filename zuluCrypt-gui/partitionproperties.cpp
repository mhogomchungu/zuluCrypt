/*
 *
 *  Copyright (c) 2011
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "partitionproperties.h"

partitionproperties::partitionproperties(QString partitionType)
{
	m_partitionType = partitionType ;
}

void partitionproperties::run()
{
	QProcess p ;

	/*
	  Root user can create encrypted volumes in all partitions including system partitions.
	  Show all partitions, not only non system.
	  */
	if(m_partitionType == QString(" -N") && getuid() == 0)
		m_partitionType = QString(" -A") ;

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

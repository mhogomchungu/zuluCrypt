/*
 *
 *  Copyright (c) 2014
 *  name : Francis Banyikwa
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

#ifndef ZULUMOUNTTASK_H
#define ZULUMOUNTTASK_H

#include "volumeentryproperties.h"
#include "../zuluCrypt-gui/task.h"
#include "../zuluCrypt-gui/utility.h"

#include <QVector>
#include <QString>
#include <QStringList>

struct volumeMiniPropertiesTaskResult
{
	QString volumeName ;
	bool volumeRemoved ;
	volumeEntryProperties * entry ;
};

namespace zuluMountTask
{
	typedef enum
	{
		device,dm_device,md_device
	}deviceTypes;

	struct event
	{
		deviceTypes deviceType ;
		bool added ;
		QString volumeName ;
	};

	Task::future< QString >& volumeProperties( const QString& volume,const QString& volumeType ) ;
	Task::future< QVector< volumeEntryProperties > >& updateVolumeList( void ) ;
	Task::future< volumeEntryProperties >& getVolumeProperties( const QString& e ) ;
	Task::future< utility::Task >& unmountVolume( const QString& volumePath,const QString& volumeType ) ;

	utility::Task volumeUnmount( const QString& volumePath,const QString& volumeType ) ;

	void checkUnMount( const QString& ) ;
	volumeMiniPropertiesTaskResult volumeMiniProperties( const QString& volume ) ;
	volumeMiniPropertiesTaskResult deviceProperties( const zuluMountTask::event& ) ;
	QStringList mountedVolumeList( void ) ;

	QStringList hiddenVolumeList( void ) ;
	void addVolumeToHiddenVolumeList( const QString& ) ;
	void removeVolumeFromHiddenVolumeList( const QString& ) ;
}

#endif // ZULUMOUNTTASK_H

/*
 *
 *  Copyright (c) 2014
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

#ifndef ZULUMOUNTTASK_H
#define ZULUMOUNTTASK_H

#include "volumeentryproperties.h"

#include <QVector>
#include <QString>
#include <QStringList>

struct zuluMountTaskResult
{
	bool passed ;
	int exitCode ;
	QString outPut ;
};

struct volumeMiniPropertiesResult
{
	QString volumeName ;
	bool volumeRemoved ;
	volumeEntryProperties * entry ;
};

namespace zuluMount
{
	typedef enum
	{
		device,dm_device,md_device
	}deviceTypes;

	struct deviceProperties
	{
		deviceTypes deviceType ;
		bool added ;
		QString volumeName ;
	};

	namespace Task
	{
		volumeEntryProperties getVolumeProperties( const QString& e ) ;
		QString volumeProperties( const QString& volume,const QString& volumeType ) ;
		zuluMountTaskResult unmountVolume( const QString& volumePath,const QString& volumeType ) ;
		zuluMountTaskResult cryptoOpen( void ) ;
		QVector< volumeEntryProperties > updateVolumeList( void ) ;
		void checkUnMount( const QString& ) ;
		volumeMiniPropertiesResult volumeMiniProperties( const QString& volume ) ;
		volumeMiniPropertiesResult deviceProperties( const zuluMount::deviceProperties& ) ;
		QStringList mountedVolumeList( void ) ;
	}
}

#endif // ZULUMOUNTTASK_H

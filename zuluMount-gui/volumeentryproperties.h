/*
 *
 *  Copyright (c) 2014-2015
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
#ifndef VOLUMEENTRYPROPERTIES_H
#define VOLUMEENTRYPROPERTIES_H

#include <QString>
#include <QStringList>

#include <initializer_list>

#include <sys/vfs.h>

#include "../zuluCrypt-gui/utility.h"

#include <QDebug>

class volumeEntryProperties
{
public:
	volumeEntryProperties( const QStringList& l = QStringList(),bool isSystem = false )
	{
		this->setValues( l,isSystem ) ;
	}
	volumeEntryProperties( const std::initializer_list<QString>& l,bool isSystem = false )
	{
		this->setValues( l,isSystem ) ;
	}
	const QString& volumeName() const
	{
		return m_volume ;
	}
	const QString& mountPoint() const
	{
		return m_mountPoint ;
	}
	const QString& fileSystem() const
	{
		return m_fileSystem ;
	}
	const QString& label() const
	{
		return m_label ;
	}
	const QString& volumeSize() const
	{
		return m_volumeSize ;
	}
	const QString& spaceUsedPercentage() const
	{
		return m_usedSpacePercentage ;
	}
	bool isSystem() const
	{
		return m_isSystem ;
	}
	bool isEmpty() const
	{
		return m_volume.isEmpty() ;
	}
	bool isNotEmpty() const
	{
		return !m_volume.isEmpty() ;
	}
	bool isValid() const
	{
		if( m_volume.isEmpty() ){
			return false ;
		}
		if( m_fileSystem == "encfs" || m_fileSystem == "cryfs" ){
			return true ;
		}
		if( m_volumeSize == "1.0 KB" || m_volumeSize == "1,0 KB" || m_volumeSize == "0 B" || m_volumeSize == "Nil" ){
			return false ;
		}
		if( m_mountPoint == "/run/media/public/" ){
			return false ;
		}
		return true ;
	}
	bool encryptedVolume() const
	{
		return  m_fileSystem.startsWith( "crypto" ) ||
			m_fileSystem == "Nil"   ||
			m_fileSystem == "cryptfs" ||
			m_fileSystem == "cryfs" ||
			m_fileSystem == "encfs" ;
	}
	bool mounted() const
	{
		return m_mountPoint != "Nil" ;
	}
	QStringList entryList() const
	{
		return { m_volume,m_mountPoint,m_fileSystem,m_label,m_volumeSize,m_usedSpacePercentage } ;
	}
	volumeEntryProperties& setMountPoint( const QString& m )
	{
		m_mountPoint = m ;
		return *this ;
	}
private:
	void setValues( const QStringList& l,bool isSystem )
	{
		m_isSystem = isSystem ;

		if( l.size() >= 6 ){

			m_volume      = l.at( 0 ) ;
			m_mountPoint  = l.at( 1 ) ;
			m_fileSystem  = l.at( 2 ) ;
			m_label       = l.at( 3 ) ;
			m_volumeSize  = l.at( 4 ) ;
			m_usedSpacePercentage = l.at( 5 ) ;

			int index = m_fileSystem.indexOf( "/" ) ;

			if( index != -1 ){

				m_fileSystem = m_fileSystem.replace( "/","\n(" ) + ")" ;
			}

			m_usedSpacePercentage.remove( '\n' ) ;

			if( m_fileSystem == "cryfs" ){

				m_usedSpacePercentage = [ this ]()->QString{

					struct statfs vfs ;

					auto passed = Task::await< bool >( [ & ](){

						return statfs( m_mountPoint.toLatin1().constData(),&vfs ) == 0 ;
					} ) ;

					if( passed ){

						quint64 s = vfs.f_bsize * ( vfs.f_blocks - vfs.f_bavail ) ;

						m_volumeSize = utility::prettyfySpaceUsage( s ) ;

						if( vfs.f_bfree == 0 ){

							return "100%" ;
						}else{
							quint64 s = vfs.f_blocks - vfs.f_bavail ;

							auto e = double( s ) / double( vfs.f_blocks ) * 100 ;

							if( e < 0.01 ){

								return "0%" ;
							}else{
								return QString::number( e,'f',2 ) + "%" ;
							}
						}
					}else{
						return "Nil" ;
					}
				}() ;
			}
		}
	}

	QString m_volume ;
	QString m_mountPoint ;
	QString m_fileSystem ;
	QString m_label ;
	QString m_volumeSize ;
	QString m_usedSpacePercentage ;
	bool    m_isSystem = true ;
};

#endif // VOLUMEENTRYPROPERTIES_H

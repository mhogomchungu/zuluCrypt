/*
 *
 *  Copyright (c) 2012-2015
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

#include "monitor_mountinfo.h"

#include <QString>
#include <QStringList>
#include <QDebug>

#include "../zuluCrypt-gui/utility.h"
#include "zulumounttask.h"

monitor_mountinfo::monitor_mountinfo( QObject * parent,bool s,std::function< void() > f ) :
	m_parent( parent ),
	m_stop( std::move( f ) ),
	m_announceChanges( s ),
	m_announceEvents( true )
{
}

monitor_mountinfo::~monitor_mountinfo()
{
}

std::function< void() > monitor_mountinfo::stop()
{
	return [ this ](){ utility::stopTask( m_task,m_stop ) ; } ;
}

void monitor_mountinfo::announceEvents( bool s )
{
	m_announceEvents = s ;
}

void monitor_mountinfo::start()
{
	m_task = utility::startTask( [ this ](){ this->run() ; },m_stop ) ;
}

void monitor_mountinfo::run()
{
	auto _unmountProperty = [ & ]( const QString& volume ){

		Task::exec( [ &,volume ](){

			auto r = zuluMountTask::volumeMiniProperties( volume ) ;

			if( r.volumeRemoved ){

				emit volumeRemoved( r.volumeName ) ;
			}else{
				emit volumeMiniProperties( r.entry ) ;
			}
		} ) ;
	} ;

	auto _mountProperty = [ & ]( const QString& volume ){

		Task::exec( [ &,volume ](){

			emit volumeMiniProperties_0( zuluMountTask::volumeMiniProperties( volume ).entry ) ;
		} ) ;
	} ;

	auto oldMountList = [ this ](){

		if( m_announceChanges ){

			return zuluMountTask::mountedVolumeList() ;
		}else{
			return QStringList() ;
		}
	}() ;

	QStringList newMountList ;

	auto _volumeWasUnMounted = [ & ](){ return oldMountList.size() > newMountList.size() ; } ;

	auto _volumeWasMounted   = [ & ](){ return oldMountList.size() < newMountList.size() ; } ;

	auto _unmountedVolume    = [ & ]( const QString& e ){ return !newMountList.contains( e ) ; } ;

	auto _mountedVolume      = [ & ]( const QString& e ){ return !oldMountList.contains( e ) ; } ;

	utility::monitor_mountinfo monitor ;

	while( monitor.gotEvent() ){

		if( m_announceEvents ){

			emit gotEvent() ;
		}

		if( m_announceChanges ){

			newMountList = zuluMountTask::mountedVolumeList() ;

			if( _volumeWasUnMounted() ){

				for( const auto& it : oldMountList ){

					if( _unmountedVolume( it ) ){

						_unmountProperty( it ) ;
					}
				}

			}else if( _volumeWasMounted() ){

				for( const auto& it : newMountList ){

					if( _mountedVolume( it ) ){

						_mountProperty( it ) ;
					}
				}
			}else{
				/*
				 * mount/unmount just happened but volume count remain the same,
				 * possible reason is because of a bind mount
				 */
			}

			oldMountList = newMountList ;
		}
	}
}

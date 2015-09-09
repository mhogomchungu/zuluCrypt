
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

#ifndef MOUNTPARTITION_H
#define MOUNTPARTITION_H

#include <QWidget>
#include <QString>

#include "volumeentryproperties.h"

#include <functional>
#include <memory>

class QTableWidget ;
class QCloseEvent ;
class QMenu ;
class QAction ;

namespace Ui {
class mountPartition;
}

class mountPartition : public QWidget
{
	Q_OBJECT
public:
	static mountPartition * instance( QWidget * parent,
					  QTableWidget * table,
					  std::function< void() > cancel,
					  std::function< void( const QString& ) > success )
	{
		return new mountPartition( parent,table,std::move( cancel ),std::move( success ) ) ;
	}
	mountPartition( QWidget * parent,
			QTableWidget * table,
			std::function< void() >,
			std::function< void( const QString& ) > ) ;
	void ShowUI( const volumeEntryProperties& ) ;
	void HideUI( void ) ;
	void AutoMount( const QStringList& entry ) ;
	~mountPartition() ;
signals:
	void cancel( void ) ;
	void openMountPoint( QString ) ;
private slots:
	void stateChanged( int ) ;
	void pbMount( void ) ;
	void pbOptions( void ) ;
	void pbCancel( void ) ;
	void pbOpenMountPath( void ) ;
	void showOffSetWindowOption( void ) ;
	void showFileSystemOptionWindow( void ) ;
	void checkBoxReadOnlyStateChanged( int ) ;
	void doAction( QAction * ) ;
private:
	void enableAll( void ) ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::mountPartition * m_ui ;
	QString m_path ;
	QString m_label ;
	QString m_point ;
	QString m_deviceOffSet ;
	QString m_key ;
	QString m_options ;
	QTableWidget * m_table ;
	QMenu * m_menu ;
	std::function< void() > m_cancel ;
	std::function< void( const QString& ) > m_success ;
};

#endif // MOUNTPARTITION_H

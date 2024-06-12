
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

#include "volumeproperty.h"
#include "../zuluCrypt-gui/utility.h"

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
	static mountPartition& instance( QWidget * parent,
					  QTableWidget * table,
					  std::function< void() > cancel,
					  std::function< void( const QString& ) > success )
	{
		return *( new mountPartition( parent,table,std::move( cancel ),std::move( success ) ) ) ;
	}
	mountPartition( QWidget * parent,
			QTableWidget * table,
			std::function< void() >,
			std::function< void( const QString& ) > ) ;
	void ShowUI( const volumeProperty& ) ;
	void HideUI() ;
	void AutoMount( const volumeProperty& ) ;
	~mountPartition() ;
signals:
	void cancel() ;
	void openMountPoint( QString ) ;
private:
	void stateChanged( int ) ;
	void pbMount() ;
	void pbCancel() ;
	void pbOpenMountPath() ;
	void checkBoxReadOnlyStateChanged( int ) ;
	void reportError( utility::Task& ) ;
	void enableAll() ;
	void disableAll() ;
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
	std::function< void() > m_cancel ;
	std::function< void( const QString& ) > m_success ;
	utility::label m_Label ;
};

#endif // MOUNTPARTITION_H

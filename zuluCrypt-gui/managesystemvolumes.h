/*
 *
 *  Copyright ( c ) 2011-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MANAGESYSTEMVOLUMES_H
#define MANAGESYSTEMVOLUMES_H

#include <QDialog>
#include <QString>

#include "utility.h"

class QCloseEvent ;
class QAction ;
class QTableWidgetItem ;
class QWidget ;

namespace Ui {
class manageSystemVolumes ;
}

class manageSystemVolumes : public QDialog
{
	Q_OBJECT
public:
	static void instance( QWidget * parent,const char * s )
	{
		new manageSystemVolumes( parent,s ) ;
	}

	explicit manageSystemVolumes( QWidget * parent,const char * s ) ;
	~manageSystemVolumes() ;
	void HideUI() ;
signals:
	void HideUISignal() ;
private:
	void defaultButton() ;
	void pbDone() ;
	void pbFile() ;
	void pbPartition() ;
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	void itemClicked( QTableWidgetItem * current,bool ) ;
	void itemClicked( QTableWidgetItem * current ) ;
	void removeCurrentRow() ;
	void contextMenu() ;
	void readSystemPartitions() ;
	void writeSystemPartitions() ;

	void addItemsToTable( const QStringList& ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::manageSystemVolumes * m_ui ;
	QString m_path ;
	bool m_read ;
};

#endif // MANAGESYSTEMVOLUMES_H

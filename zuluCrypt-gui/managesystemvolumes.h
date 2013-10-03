/*
 *
 *  Copyright ( c ) 2011
 *  name : mhogo mchungu
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
	explicit manageSystemVolumes( QWidget * parent = 0 ) ;
	~manageSystemVolumes() ;
	void ShowUI( const QString& ) ;
	void HideUI( void ) ;
signals:
	void HideUISignal( void ) ;
private slots:
	void defaultButton( void ) ;
	void pbDone( void ) ;
	void pbFile( void ) ;
	void pbPartition( void ) ;
	void clickedPartition( QString ) ;
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	void itemClicked( QTableWidgetItem * current,bool ) ;
	void itemClicked( QTableWidgetItem * current ) ;
	void removeCurrentRow( void ) ;
	void contextMenu( void ) ;
	void setFocusTableWidget( void ) ;
private:
	void readSystemPartitions( void ) ;
	void writeSystemPartitions( void ) ;

	void addItemsToTable( QStringList ) ;
	void addItemsToTable( QString ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::manageSystemVolumes * m_ui ;
	QString m_path ;
	QAction * m_ac ;
};

#endif // MANAGESYSTEMVOLUMES_H

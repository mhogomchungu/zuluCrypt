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

#ifndef MANAGEDEVICENAMES_H
#define MANAGEDEVICENAMES_H

#include <QDialog>
#include <QString>
#include <QStringList>

class QAction ;
class QWidget ;
class QTableWidgetItem ;
class QCloseEvent ;

namespace Ui {
class favorites ;
}

class favorites : public QDialog
{
	Q_OBJECT
public:
	static favorites& instance( QWidget * parent,bool e )
	{
		return *( new favorites( parent,e ) ) ;
	}
	explicit favorites( QWidget * parent,bool e ) ;
	~favorites() ;
signals:
	void ShowPartitionUI( void ) ;
public slots:
	void ShowUI( void ) ;
	void HideUI( void ) ;
private slots:
	void removeEntryFromFavoriteList( void ) ;
	void add( void ) ;
	void cancel( void ) ;
	void deviceAddress( void ) ;
	void fileAddress( void ) ;
	void folderAddress( void ) ;
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	void itemClicked( QTableWidgetItem * current,bool ) ;
	void itemClicked( QTableWidgetItem * current ) ;
	void shortcutPressed( void ) ;
	void devicePathTextChange( QString ) ;
private:
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void addEntries( const QStringList& ) ;
	Ui::favorites * m_ui ;
	QAction * m_ac ;
};

#endif // MANAGEDEVICENAMES_H

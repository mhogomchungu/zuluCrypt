/*
 *
 *  Copyright ( c ) 2020
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

#ifndef SHOWLUKSSLOTS_H
#define SHOWLUKSSLOTS_H

#include <QDialog>

#include <QCloseEvent>
#include <functional>
#include <memory>

class QTableWidgetItem ;

namespace Ui {
class showLUKSSlots;
}

class showLUKSSlots : public QDialog
{
	Q_OBJECT
public:
	static void Show( QWidget * parent,const QString& path,std::function< void() > function )
	{
		auto m = new showLUKSSlots( parent,path,std::move( function ) ) ;
		m->ShowUI() ;
	}
	explicit showLUKSSlots( QWidget * parent,const QString& path,std::function< void() > ) ;
	~showLUKSSlots() ;
private:
	void deleteSlot( QTableWidgetItem * ) ;
	void showData() ;
	void HideUI() ;
	void ShowUI() ;
	void enableAll() ;
	void disableAll() ;
	void showMenu( QTableWidgetItem * ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::showLUKSSlots * m_ui ;
	std::function< void() > m_function ;
	QString m_path ;
	QWidget * m_parent ;
};

#endif // SHOWLUKSSLOTS_H

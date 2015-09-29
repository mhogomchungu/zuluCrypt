/*
 *
 *  Copyright (c) 2015
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


#ifndef VERACRYPTPIMDIALOG_H
#define VERACRYPTPIMDIALOG_H

#include <functional>
#include <memory>

#include <QCloseEvent>
#include <QObject>

#include <QDialog>

class QCloseEvent ;

namespace Ui {
class VeraCryptPIMDialog ;
}

class VeraCryptPIMDialog : public QDialog
{
	Q_OBJECT
public:
	static VeraCryptPIMDialog * instance( QWidget * parent,std::function< void( int ) > function )
	{
		return new VeraCryptPIMDialog( parent,std::move( function ) ) ;
	}
	explicit VeraCryptPIMDialog( QWidget * parent,std::function< void( int ) > ) ;
	~VeraCryptPIMDialog() ;
private slots:
	void pbSet() ;
	void pbCancel() ;
private:
	void Show() ;
	void Hide() ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::VeraCryptPIMDialog * m_ui ;
	std::function< void( int ) > m_function ;
};

#endif // VERACRYPTPIMDIALOG_H

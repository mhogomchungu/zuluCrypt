/*
 *
 *  Copyright (c) 2014
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

#ifndef MOUNTOPTIONS_H
#define MOUNTOPTIONS_H

#include <QDialog>

class QString ;
class QCloseEvent ;

namespace Ui {
class mountOptions;
}

class mountOptions : public QDialog
{
	Q_OBJECT
public:
	explicit mountOptions( QString * options = 0,QWidget * parent = 0 ) ;
	void ShowUI( void ) ;
	~mountOptions() ;
private slots:
	void pbOK( void ) ;
	void pbCancel( void ) ;
	void HideUI( void ) ;
private:
	void closeEvent( QCloseEvent * ) ;
	Ui::mountOptions * m_ui ;
	QString * m_options ;
};

#endif // MOUNTOPTIONS_H

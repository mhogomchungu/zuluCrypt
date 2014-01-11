/*
 *
 *  Copyright (c) 2014
 *  name : mhogo mchungu
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


#ifndef DEVICEOFFSET_H
#define DEVICEOFFSET_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class deviceOffset;
}

class deviceOffset : public QDialog
{
	Q_OBJECT
public:
	explicit deviceOffset( QWidget * parent = 0 ) ;
	void ShowUI( void ) ;
	void ShowUI_1( void ) ;
	~deviceOffset();
signals:
	void offSetValue( QString,QString ) ;
private slots:
	void closeUI( void ) ;
	void pbOK( void ) ;
	void pbCancel( void ) ;
private:
	Ui::deviceOffset * m_ui ;
	void closeEvent( QCloseEvent * ) ;
};

#endif // DEVICEOFFSET_H

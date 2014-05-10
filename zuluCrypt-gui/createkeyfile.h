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

#ifndef CREATEKEYFILE_H
#define CREATEKEYFILE_H

#include <QDialog>
#include <QString>

#include "utility.h"

class QWidget ;
class keyFileTask ;
class QCloseEvent ;

namespace Ui {
    class createkeyfile ;
}

class createkeyfile : public QDialog
{
	Q_OBJECT
public:
	explicit createkeyfile( QWidget * parent = 0 ) ;
	~createkeyfile() ;
signals:
	void HideUISignal( void ) ;
public slots:
	void ShowUI( void ) ;
	void HideUI( void ) ;

private slots:
	void pbCancel( void ) ;
	void pbCreate( void ) ;
	void pbOpenFolder( void ) ;
	void taskStatus( int ) ;
	void keyTextChange( QString ) ;
private:
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void disableAll( void ) ;
	void enableAll( void ) ;
	Ui::createkeyfile * m_ui;
	keyFileTask * m_task ;
};

#endif // CREATEKEYFILE_H

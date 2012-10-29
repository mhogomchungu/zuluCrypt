/*
 *  Copyright ( c ) 2012
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

#ifndef DIALOGMSG_H
#define DIALOGMSG_H

#include <QDialog>
#include <QString>
#include <QMessageBox>
#include <QCloseEvent>
#include <QCheckBox>

//#include "../zuluCrypt-gui/zulucrypt.h"

namespace Ui {
class DialogMsg;
}

class DialogMsg : public QDialog
{
	Q_OBJECT
public:
	explicit DialogMsg( QWidget * parent = 0 );
	void ShowUIOK( QString title,QString msg );
	int  ShowUIYesNo( QString title,QString msg );
	int  ShowUIYesNoDefaultNo( QString title,QString msg );
	void ShowUIInfo( QString title,QString msg );
	void ShowUIVolumeProperties( QString title,QString msg );
	bool ShowUIOKDoNotShowOption( QString title,QString msg ) ;
	void ShowPermissionProblem( QString device ) ;
	void ShowPermissionProblem( QString,QString device ) ;
	void HideUI( void );
	~DialogMsg();

private slots:
	void pbOK( void );
	void pbYes( void );
	void pbNo( void );

private:
	void SetUpButtons( void ) ;
	void ShowLabels( void ) ;
	void HideLabels( void ) ;
	void ShowUI( QString title,QString msg );
	void setDimentions( QString msg );
	void closeEvent( QCloseEvent * ) ;
	Ui::DialogMsg * m_ui;
	int m_status ;
};

#endif // DIALOGMSG_H

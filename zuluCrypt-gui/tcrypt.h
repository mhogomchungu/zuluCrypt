/*
 *
 *  Copyright (c) 2014-2015
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
#ifndef TCRYPT_H
#define TCRYPT_H

#include <QDialog>
#include "utility.h"

class QCloseEvent ;
class QTableWidgetItem ;

#include <QDropEvent>
#include <QDragEnterEvent>
#include <QString>
#include <QStringList>

namespace Ui {
class tcrypt;
}

class tcrypt : public QDialog
{
	Q_OBJECT
public:
	explicit tcrypt( QWidget * parent = 0 ) ;
	void ShowUI( void ) ;
	void ShowUI_1( void ) ;
	void HideUI( void ) ;
	~tcrypt();
signals:
	void Keys( QString passphrase,QStringList keyfiles ) ;
	void cancelled( void ) ;
private slots:
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	void itemClicked ( QTableWidgetItem * item ) ;
	void dragEnterEvent( QDragEnterEvent * e ) ;
	void dropEvent( QDropEvent * ) ;
	void pbSend( void ) ;
	void pbCancel( void ) ;
	void pbAddKeyFIle( void ) ;
private:
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::tcrypt * m_ui ;
};

#endif // TCRYPT_H

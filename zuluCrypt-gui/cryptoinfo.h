/*
 *
 *  Copyright ( c ) 2012-2015
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

#ifndef CRYPTOINFO_H
#define CRYPTOINFO_H

#include <QWidget>
#include <QString>

#include <functional>

#include "utility.h"

class QCloseEvent ;

namespace Ui {
class cryptoinfo ;
}

class cryptoinfo : public QWidget
{
	Q_OBJECT
public:
	cryptoinfo( QWidget * parent,QString path,QString msg ) ;
	void Show( void ) ;
	~cryptoinfo() ;
signals:
	void closeUISignal( void ) ;
private slots:
	void pbOK( void ) ;
	void checkBoxChecked( bool ) ;
private:
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void HideUI( void ) ;
	Ui::cryptoinfo * m_ui ;
	QString m_path ;
	QString m_msg ;
};

#endif // CRYPTOINFO_H

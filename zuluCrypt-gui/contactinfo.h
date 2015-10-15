/*
 *
 *  Copyright ( c ) 2015
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


#ifndef CONTACTINFO_H
#define CONTACTINFO_H

#include <QDialog>

class QCloseEvent ;
class QEvent ;
class QObject ;

namespace Ui {
class contactInfo;
}

class contactInfo : public QDialog
{
	Q_OBJECT
public:
	static contactInfo& instance( QWidget * parent )
	{
		return *( new contactInfo( parent ) ) ;
	}
	explicit contactInfo( QWidget * parent = 0 ) ;
	~contactInfo() ;
private slots:
	void pbOK( void ) ;
private:
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::contactInfo * m_ui ;
};

#endif // CONTACTINFO_H

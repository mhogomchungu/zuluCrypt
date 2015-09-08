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

#ifndef CREATEFILE_H
#define CREATEFILE_H

#include <QDialog>

#include "utility.h"

class DialogMsg ;
class QPushButton ;
class FileTask ;
class QCloseEvent ;
class createfile ;

namespace Ui {
    class createfile ;
}

class createfile : public QDialog
{
	Q_OBJECT
public:
	explicit createfile( QWidget * parent,std::function< void( const QString& ) > ) ;
	~createfile();
signals :
	void sendProgress( int ) ;
public slots:
	void HideUI( void ) ;
	void showUI( void ) ;
	void pbOpenFolder( void ) ;
	void pbCancel( void ) ;
	void pbCreate( void ) ;
private slots:
	void setProgress( int ) ;
	void fileTextChange( QString ) ;
private:
	void enableAll( void ) ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::createfile * m_ui ;
	bool m_exit ;
	bool m_running ;
	std::function< void( const QString& ) > m_function ;
};

#endif // CREATEFILE_H

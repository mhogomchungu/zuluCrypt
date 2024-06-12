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
#ifndef createVolumeDialog_H
#define createVolumeDialog_H

#include <QDialog>
#include <QString>
#include <functional>
#include <memory>
#include <QCloseEvent>

namespace Ui {
class createVolumeDialog ;
}

class createVolumeDialog : public QDialog
{
	Q_OBJECT
public:
	static createVolumeDialog& instance( const QString& path,QWidget * parent,std::function< void( int ) > f )
	{
		return *( new createVolumeDialog( path,parent,std::move( f ) ) ) ;
	}
	explicit createVolumeDialog( const QString& path,QWidget * parent,std::function< void( int ) > ) ;
	~createVolumeDialog() ;
private:
	void pbYes() ;
	void pbNo() ;
	void closeEvent( QCloseEvent * ) ;
	void ShowUI() ;
	Ui::createVolumeDialog * m_ui ;
	int m_opt ;
	int m_opt_count ;
	QString m_path ;
	std::function< void( int ) > m_function ;
};

#endif // createVolumeDialog_H

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
#ifndef CREATEVOLUMEDIALOG_H
#define CREATEVOLUMEDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class CreateVolumeDialog ;
}

class CreateVolumeDialog : public QDialog
{
	Q_OBJECT
public:
	explicit CreateVolumeDialog( const QString& path,QWidget * parent = 0 ) ;
	void ShowUI( void ) ;
	~CreateVolumeDialog() ;
signals:
	void dialogResult( int ) ;
private slots:
	void pbYes( void ) ;
	void pbNo( void ) ;
private:
	Ui::CreateVolumeDialog * m_ui ;
	int m_opt ;
	int m_opt_count ;
	QString m_path ;
};

#endif // CREATEVOLUMEDIALOG_H

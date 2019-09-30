/*
 *
 *  Copyright ( c ) 2019
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

#ifndef CREATEVOLUMEINEXISTINGFILE_H
#define CREATEVOLUMEINEXISTINGFILE_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class createVolumeInExistingFIle;
}

class createVolumeInExistingFIle : public QDialog
{
	Q_OBJECT
public:
	static void instance( QWidget * parent )
	{
		new createVolumeInExistingFIle( parent ) ;
	}
	explicit createVolumeInExistingFIle( QWidget * parent ) ;
	~createVolumeInExistingFIle();
private slots :
	void updateProgress( QString,QString,QString,QString,int ) ;
private:
	void enableAll() ;
	void disableAll() ;
	void displayWarning( bool ) ;
	void create() ;
	void Hide() ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	QString m_warning ;
	std::pair< qint64,QString > m_volumeOffset ;
	Ui::createVolumeInExistingFIle * m_ui ;
};

#endif // CREATEVOLUMEINEXISTINGFILE_H

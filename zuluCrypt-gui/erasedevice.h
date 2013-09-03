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

#ifndef ERASEDEVICE_H
#define ERASEDEVICE_H

#include <QDialog>
#include <QString>

class QCloseEvent ;
class erasedevicethread ;
class QWidget ;

namespace Ui {
class erasedevice;
}

class erasedevice : public QDialog
{
	Q_OBJECT
public:
	explicit erasedevice( QWidget * parent = 0 );
	~erasedevice();
	void ShowUI( void );
	void ShowUI( QString );
	void HideUI( void );
signals:
	void HideUISignal( void );
	void complete( QString );
private slots:
	void enableAll( void );
	void disableAll( void );
	void pbStart( void );
	void pbCancel( void );
	void pbFile( void );
	void pbPartition( void );
	void setProgress( int );
	void setPath( QString );
	void threadExitStatus( int );
private:
	void run( void );
	void closeEvent( QCloseEvent * );
	Ui::erasedevice * m_ui;
	erasedevicethread * m_dt  ;
	bool m_cancelClicked ;
	int m_option ;
};

#endif // ERASEDEVICE_H

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

#ifndef MANAGELUKSHEADER_H
#define MANAGELUKSHEADER_H

#include <QDialog>
#include <QString>

class QWidget ;
class QCloseEvent ;

namespace Ui {
    class manageluksheader;
}

class manageluksheader : public QDialog
{
	Q_OBJECT
public:
	explicit manageluksheader( QWidget * parent = 0 ) ;
	~manageluksheader() ;
signals:
	void HideUISignal( void );
public slots:
	void ShowUI( void ) ;
	void HideUI( void ) ;
	void backUpHeader( void ) ;
	void backUpHeader( QString ) ;
	void restoreHeader( void ) ;

private slots:
	void pbCancel( void ) ;
	void pbCreate( void ) ;
	void pbOpenLuksHeaderBackUp( void ) ;
	void taskFinished( int ) ;
	void pbOpenPartition( void ) ;
	void pbOpenFile( void ) ;
	void selectedPartition( QString );
	void backUpHeaderNameChange( QString );
private:
	void headerBackUp( void ) ;
	void closeEvent( QCloseEvent * ) ;
	void disableAll( void ) ;
	void enableAll( void ) ;
	void success( void ) ;
	Ui::manageluksheader * m_ui;
	QString m_path ;
	bool m_OperationInProgress ;
	QString m_operation ;
	int m_saveHeader ;
};

#endif // CREATEKEYFILE_H

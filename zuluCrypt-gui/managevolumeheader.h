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

#ifndef managevolumeheader_H
#define managevolumeheader_H

#include <QDialog>
#include <QString>

#include "utility.h"
class QWidget ;
class QCloseEvent ;

namespace Ui {
    class managevolumeheader;
}

class managevolumeheader : public QDialog
{
	Q_OBJECT
public:
	static managevolumeheader& instance ( QWidget * parent )
	{
		return *( new managevolumeheader( parent ) ) ;
	}
	explicit managevolumeheader( QWidget * parent = 0 ) ;
	~managevolumeheader() ;
signals:
	void HideUISignal( void ) ;
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
	void pbKeyFile( void ) ;
	void backUpHeaderNameChange( QString ) ;
	void cbKeyType( int ) ;
	void cbVolumeType( int ) ;
	void cbVolumeHeader( int ) ;
private:
	void headerBackUp( void ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void disableAll( void ) ;
	void enableAll( void ) ;
	void success( void ) ;
	Ui::managevolumeheader * m_ui ;
	QString m_path ;
	bool m_OperationInProgress ;
	QString m_operation ;
	int m_saveHeader ;
	utility::veraCryptWarning m_veraCryptWarning ;
};

#endif // CREATEKEYFILE_H

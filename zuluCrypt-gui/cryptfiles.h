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

#ifndef CRYPTFILES_H
#define CRYPTFILES_H

#include <QDialog>
#include <QString>
#include "utility.h"

class QWidget ;
class QCloseEvent ;
class cryptfiles ;

namespace Ui {
    class cryptfiles ;
}

class cryptfiles : public QDialog
{
	Q_OBJECT
public:
	typedef enum{
		unset,
		quit,
		success,
		openMapperReadFail,
		openMapperWriteFail,
		openMapperFail,
		closeMapperFail,
		openKeyFileReadFail,
		QProcessFail,
		md5Fail,
		md5Pass,
		wrongKey,
		encryptSuccess,
		decryptSuccess,
		destinationFileExists,
		createFileFail,
		OpenSourceFail,
		OpenDestinationFail
	}status;

	static cryptfiles& instance( QWidget * parent = 0 )
	{
		return *( new cryptfiles( parent ) ) ;
	}
	explicit cryptfiles( QWidget * parent = 0 ) ;
	~cryptfiles() ;
signals:
	void HideUISignal() ;
	void progressUpdate( int ) ;
public:
	void encrypt() ;
	void decrypt() ;
	void decrypt( const QString& ) ;
	void enableCancel() ;
	void disableCancel() ;
	void pbCancel() ;
	void pbCreate() ;
	void pbOpenFile() ;
	void pbKeyFile() ;
	void pbOpenFolder() ;
	void cbChanged( int ) ;
	void sourceTextChanged( QString ) ;
	void taskFinished( int ) ;
	void progressBarUpdate( int ) ;
	void titleUpdate( QString ) ;
private:
	void cryptFile( const char *,const char *,const char *,unsigned long,bool ) ;
	void HideUI() ;
	void headerBackUp() ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void disableAll() ;
	void enableAll() ;
	QString destinationPath( const QString& e = QString() ) ;
	Ui::cryptfiles * m_ui ;

	QString m_path ;

	bool m_OperationInProgress ;
	QString m_operation ;
	cryptfiles * m_task ;
	utility::label m_label ;
};

#endif // CREATEKEYFILE_H

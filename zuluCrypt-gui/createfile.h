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

#ifndef CREATEFILE_H
#define CREATEFILE_H

#include <QDialog>
#include <QProcess>
#include <QTimer>
#include <QCloseEvent>
#include <QMessageBox>
#include "createfilethread.h"

#include "utility.h"
#include "dialogmsg.h"

namespace Ui {
    class createfile;
}

class createfile : public QDialog
{
	Q_OBJECT
public:
	explicit createfile( QWidget * parent = 0 );
	~createfile();
signals :
	void fileCreated( QString ) ;
	void cancelOperation( void );
	void HideUISignal( void );

public slots:
	void HideUI( void );
	void showUI( void ) ;
	void pbOpenFolder( void ) ;
	void pbCancel( void ) ;
	void pbCreate( void ) ;
private slots:
	void exitStatus( int );
	void doneCreatingFile( void );
	void progress( int );
	void fileTextChange( QString ) ;
private:
	void enableAll( void ) ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::createfile *m_ui;
	double m_fileSize ;
	QString m_path ;
	createFileThread * m_cft ;
	QPushButton * m_yes ;
	QPushButton * m_no  ;

	DialogMsg * m_msg ;
};

#endif // CREATEFILE_H

/*
 *
 *  Copyright (c) 2012
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QAction>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QFileDialog>
#include <QFile>
#include <QVector>
#include <QDir>
#include <QProcess>
#include <QCloseEvent>
#include "../zuluCrypt-gui/dialogmsg.h"
#include <functional>
#include <memory>

namespace Ui {
class MainWindow ;
}

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	using function_t = std::function<QByteArray( const QVector<QString>&,const QString& keyFile,const QString& password )> ;

	MainWindow( MainWindow::function_t,QWidget * parent = 0 ) ;
	void setToken( char * const * ) ;
	void setApplicationName( const QString& ) ;
	void setkeyLabel( const QString& ) ;
	void setheaderPath( const QString& ) ;
	void setkeyFileLabel( const QString& ) ;
	void setButtonIcon( const QString& ) ;
	void setRequireKey( bool = true ) ;
	void setRequireKeyFile( bool = true ) ;
	void setKeyFileAsKey( void ) ;
	void Show( void ) ;
	void setExe( const QVector<QString>& exe ) ;
	void setfindExeFunction( std::function<QString( QVector<QString>&,const QVector<QString>& )> ) ;
	~MainWindow() ;
private slots:
	void defaultButton( void ) ;
	void pbOpen( void ) ;
	void pbCancel( void ) ;
	void pbKeyFile( void ) ;
private:
	void closeEvent( QCloseEvent * ) ;
	void disableAll( void ) ;
	void enableAlll( void ) ;
	void SetFocus( void ) ;
	void Exit( int ) ;
	void cancelled( void ) ;
	Ui::MainWindow * m_ui ;
	QString m_appName ;
	bool m_printToStdOut ;
	bool m_working ;
	bool m_requireKey ;
	bool m_requireKeyFile ;
	bool m_keyfileAsKey = false ;
	QVector<QString> m_exe ;
	void * m_handle ;
	MainWindow::function_t m_function ;
	std::function<QString( QVector<QString>&,const QVector<QString>& )> m_findExecutable ;
};

#endif // MAINWINDOW_H

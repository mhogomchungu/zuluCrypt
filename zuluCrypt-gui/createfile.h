/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu
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

#ifndef CREATEFILE_H
#define CREATEFILE_H

#include <QWidget>
#include <QProcess>
#include <QTimer>
#include <QCloseEvent>
#include <QMessageBox>
#include <QThreadPool>
#include "createfilethread.h"

namespace Ui {
    class createfile;
}

class createfile : public QWidget
{
	Q_OBJECT
public:
	explicit createfile(QWidget *parent = 0);
	~createfile();
signals :
	void fileCreated(QString) ;
	void HideUISignal(createfile *);

public slots:
	void HideUI();
	void showUI(void) ;
	void pbOpenFolder(void) ;
	void pbCancel(void) ;
	void pbCreate(void) ;
private slots:
	void monitorFileGrowth(void);
	void createFileThreadFinished(void) ;
private:
	void UIMessage(QString title, QString message);
	void enableAll(void) ;
	void disableAll(void) ;
	void closeEvent(QCloseEvent *) ;
	Ui::createfile *m_ui;
	QTimer m_time ;
	double m_fileSize ;
	bool m_creating ;
	bool m_terminated ;
	bool m_return ;
	QString m_path ;
	QMessageBox m_mb ;
	createFileThread * m_cft ;
};

#endif // CREATEFILE_H

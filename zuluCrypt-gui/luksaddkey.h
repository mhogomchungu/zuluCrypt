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

#ifndef luksaddkeySUI_H
#define luksaddkeySUI_H

#include <QDialog>
#include <QCloseEvent>

#include "ui_luksaddkey.h"
#include "openpartition.h"
#include "runinthread.h"

class luksaddkey : public QDialog
{
	Q_OBJECT
public:
	explicit luksaddkey(QWidget *parent = 0);
	~luksaddkey();
signals :
	void clickedpbAdd(QString PathToVolume, bool keyfile, QString ExistingKey,bool newkeyfile, QString NewKey ) ;
	void pbOpenPartitionClicked(void) ;
	void HideUISignal(luksaddkey *);
public slots:
	void partitionEntry(QString) ;
	void ShowUI(void) ;
	void HideUI(void) ;
private slots:	
	void pbOpenExisitingKeyFile(void) ;
	void pbOpenNewKeyFile(void) ;
	void pbOpenFile(void) ;
	void pbOpenPartition(void) ;
	void rbExistingPassphrase(void) ;
	void rbExistingPassphraseFromFile(void);
	void rbNewPassphrase(void) ;
	void rbNewPassphraseFromFile(void);
	void pbAdd(void) ;
	void pbCancel(void) ;	
	void threadfinished(runInThread *,int) ;
private:
	void UIMessage(QString,QString);
	void disableAll(void) ;
	void enableAll(void) ;
	void closeEvent(QCloseEvent *) ;
	Ui::luksaddkey *m_ui ;
	openpartition *m_openPartition ;
	QString m_volumePath ;
	runInThread *m_lakt ;
};

#endif // luksaddkeySUI_H

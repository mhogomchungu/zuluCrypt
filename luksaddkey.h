/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

#include "ui_luksaddkey.h"
#include "openpartition.h"

class luksaddkeyUI : public QDialog
{
	Q_OBJECT

public:
	explicit luksaddkeyUI(QWidget *parent = 0);
	~luksaddkeyUI();

signals :

	void clickedpbAdd(QString PathToVolume, bool keyfile, QString ExistingKey,bool newkeyfile, QString NewKey ) ;

	void pbOpenPartitionClicked(void) ;

public slots:

	void partitionEntry(QString) ;

private slots:

	void ShowUI(void) ;

	void pbOpenExisitingKeyFile(void) ;

	void pbOpenNewKeyFile(void) ;

	void pbOpenFile(void) ;

	void pbOpenPartition(void) ;

	void rbExistingPassphrase(void) ;

	void rbExistingPassphraseFromFile(void);

	void rbNewPassphrase(void) ;

	void rbNewPassphraseFromFile();

	void pbAdd(void) ;

	void pbCancel(void) ;

	void HideUI(void) ;

private:

	Ui::luksaddkeyUI ui;
	openpartition pUI ;

};

#endif // luksaddkeySUI_H

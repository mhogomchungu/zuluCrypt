#ifndef luksaddkeySUI_H
#define luksaddkeySUI_H

#include <QDialog>

#include "ui_luksaddkey.h"

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
};

#endif // luksaddkeySUI_H

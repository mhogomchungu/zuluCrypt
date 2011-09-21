#ifndef LUKSDELETEKEY_H
#define LUKSDELETEKEY_H

#include <QDialog>

#include "ui_luksdeletekey.h"

class luksdeletekey : public QDialog
{
	Q_OBJECT
public:
	explicit luksdeletekey(QWidget *parent = 0);
	~luksdeletekey();

signals:

	void pbDeleteClicked(QString volumePath,bool passPhraseIsFile,QString passPhrase) ;

public slots:

	void ShowUI(void) ;

	void HideUI(void) ;

public slots :

	void deleteKey(QString) ;

private slots:

	void pbDelete(void) ;

	void pbCancel(void) ;

	void rbPassphrase(void) ;

	void rbPassphraseFromFile(void);

	void pbOpenKeyFile(void) ;

	void pbOpenVolume(void) ;

private:
    Ui::luksdeletekey ui;
};

#endif // LUKSDELETEKEY_H

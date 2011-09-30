#ifndef CREATEKEYFILE_H
#define CREATEKEYFILE_H

#include <QWidget>

namespace Ui {
    class createkeyfile;
}

class createkeyfile : public QWidget
{
	Q_OBJECT

public:
	explicit createkeyfile(QWidget *parent = 0);
	~createkeyfile();

public slots:

	void ShowUI(void) ;
	void HideUI(void) ;

private slots:

	void pbCancel(void) ;
	void pbCreate(void) ;
	void pbOpenFolder(void) ;

private:

	Ui::createkeyfile *ui;
};

#endif // CREATEKEYFILE_H

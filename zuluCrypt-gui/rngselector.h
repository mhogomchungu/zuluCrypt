#ifndef RNGSELECTOR_H
#define RNGSELECTOR_H

#include <QWidget>

namespace Ui {
    class rngselector;
}

class rngselector : public QWidget
{
    Q_OBJECT

public:
    explicit rngselector(QWidget *parent = 0);
    ~rngselector();

private slots:
	void rbUrandom(void) ;
	void rbRandom(void) ;
	void pbOk(void) ;
	void ShowUI(void) ;
private:
    Ui::rngselector *ui;
};

#endif // RNGSELECTOR_H

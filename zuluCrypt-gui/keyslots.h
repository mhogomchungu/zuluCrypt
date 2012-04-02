#ifndef KEYSLOTS_H
#define KEYSLOTS_H

#include <QWidget>

namespace Ui {
class keyslots;
}

class keyslots : public QWidget
{
	Q_OBJECT
	
public:
	explicit keyslots(QWidget *parent = 0);
	~keyslots();
	
private:
	Ui::keyslots *ui;
};

#endif // KEYSLOTS_H

#ifndef MANAGEFAVORITELIST_H
#define MANAGEFAVORITELIST_H

#include <QWidget>

namespace Ui {
    class managefavoritelist;
}

class managefavoritelist : public QWidget
{
    Q_OBJECT

public:
    explicit managefavoritelist(QWidget *parent = 0);
    ~managefavoritelist();

private:
    Ui::managefavoritelist *ui;
};

#endif // MANAGEFAVORITELIST_H

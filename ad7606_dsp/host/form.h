#ifndef FORM_H
#define FORM_H

#include <QWidget>

namespace Ui {
class form;
}

class form : public QWidget
{
    Q_OBJECT

public:
    explicit form(QWidget *parent = 0);
    ~form();

private slots:
    void on_pushButton_clicked();

private:
    Ui::form *ui;
};

#endif // FORM_H

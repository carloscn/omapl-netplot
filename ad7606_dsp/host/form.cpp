#include "form.h"
#include "ui_form.h"

form::form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::form)
{
    ui->setupUi(this);
}

form::~form()
{
    delete ui;
}

void form::on_pushButton_clicked()
{

}

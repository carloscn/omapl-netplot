/********************************************************************************
** Form generated from reading UI file 'form.ui'
**
** Created: Tue Oct 29 09:36:20 2019
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORM_H
#define UI_FORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_form
{
public:
    QLabel *label;
    QPushButton *pushButton;

    void setupUi(QWidget *form)
    {
        if (form->objectName().isEmpty())
            form->setObjectName(QString::fromUtf8("form"));
        form->resize(521, 329);
        label = new QLabel(form);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(60, 40, 201, 16));
        pushButton = new QPushButton(form);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(130, 150, 80, 23));

        retranslateUi(form);

        QMetaObject::connectSlotsByName(form);
    } // setupUi

    void retranslateUi(QWidget *form)
    {
        form->setWindowTitle(QApplication::translate("form", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("form", "OMAPL138 SAMPLE CONSOLE", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("form", "exit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class form: public Ui_form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORM_H

/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEditInputDoc;
    QPushButton *pushButtonSelectDoc;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEditOutputPath;
    QPushButton *pushButtonSelectPath;
    QPushButton *pushButtonSep;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(541, 140);
        verticalLayout = new QVBoxLayout(Dialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(Dialog);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        lineEditInputDoc = new QLineEdit(Dialog);
        lineEditInputDoc->setObjectName(QStringLiteral("lineEditInputDoc"));

        horizontalLayout->addWidget(lineEditInputDoc);

        pushButtonSelectDoc = new QPushButton(Dialog);
        pushButtonSelectDoc->setObjectName(QStringLiteral("pushButtonSelectDoc"));

        horizontalLayout->addWidget(pushButtonSelectDoc);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(Dialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        lineEditOutputPath = new QLineEdit(Dialog);
        lineEditOutputPath->setObjectName(QStringLiteral("lineEditOutputPath"));

        horizontalLayout_2->addWidget(lineEditOutputPath);

        pushButtonSelectPath = new QPushButton(Dialog);
        pushButtonSelectPath->setObjectName(QStringLiteral("pushButtonSelectPath"));

        horizontalLayout_2->addWidget(pushButtonSelectPath);


        verticalLayout->addLayout(horizontalLayout_2);

        pushButtonSep = new QPushButton(Dialog);
        pushButtonSep->setObjectName(QStringLiteral("pushButtonSep"));

        verticalLayout->addWidget(pushButtonSep);

        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));
        label->setText(QApplication::translate("Dialog", "\350\276\223\345\205\245\346\225\260\346\215\256\346\226\207\344\273\266\357\274\232", nullptr));
        pushButtonSelectDoc->setText(QApplication::translate("Dialog", "\351\200\211\346\213\251\346\226\207\344\273\266", nullptr));
        label_2->setText(QApplication::translate("Dialog", "\350\276\223\345\207\272\346\225\260\346\215\256\346\226\207\344\273\266\357\274\232", nullptr));
        lineEditOutputPath->setText(QApplication::translate("Dialog", "D:/data", nullptr));
        pushButtonSelectPath->setText(QApplication::translate("Dialog", "\351\200\211\346\213\251\350\267\257\345\276\204", nullptr));
        pushButtonSep->setText(QApplication::translate("Dialog", "\345\274\200\345\247\213\345\210\206\345\211\262", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H

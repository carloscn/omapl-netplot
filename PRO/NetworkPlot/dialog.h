#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QFileDialog>
#include <QDebug>
#include <QString>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_pushButtonSelectDoc_clicked();

    void on_pushButtonSelectPath_clicked();

    void on_pushButtonSep_clicked();

private:
    QFile *inputFile;
    QFile *outputFile;
    Ui::Dialog *ui;
    QString input_doc;
    QString output_path;
};

#endif // DIALOG_H

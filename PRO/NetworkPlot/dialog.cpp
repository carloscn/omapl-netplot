#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->inputFile = new QFile();
    this->outputFile = new QFile();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButtonSelectDoc_clicked()
{
    QString fileNameStr;

    fileNameStr = QFileDialog::getOpenFileName(this,                              \
                                               tr("Open File"),                   \
                                               "D:\\data",\
                                               "Binary Document(*.bin;*.hex)",           \
                                               0);
    this->input_doc = fileNameStr;
    ui->lineEditInputDoc->setText(fileNameStr);
}

void Dialog::on_pushButtonSelectPath_clicked()
{
    QString fileNameStr;
    fileNameStr = QFileDialog::getExistingDirectory(this,"Please slect file dir","D:\\data");
    if (fileNameStr.isEmpty())
        return;
    this->output_path = fileNameStr;
    ui->lineEditOutputPath->setText(fileNameStr);
}

void Dialog::on_pushButtonSep_clicked()
{
    QFile input_file(this->input_doc);
    QByteArray input_array;
    input_file.open(QIODevice::ReadOnly);
    input_array.append( input_file.readAll() );

    int header_count = 0;
    int tail_count = 0;

    quint64 packet_number;
    quint64 data_length;
    quint8 *socket_buffer;

    union result {
        quint64 u_64;
        float f_64;
        char c_8[8];
    } r;

    header_count = input_array.count("ab");
    tail_count = input_array.count("cd");

    if ( (header_count != tail_count) ) {
        qDebug() << " header count is :" << header_count;
        qDebug() << " tail count is : " << tail_count;
        header_count -= 1;
    }
    qDebug() << "doc include " << header_count << " packet;";
    QByteArray *seper_array = new QByteArray[header_count];
    int count = 0;
    while (!input_array.isEmpty()) {
        seper_array[count].append( input_array.mid(0, input_array.indexOf("cd") + 2));
        input_array.remove(0,input_array.indexOf("cd") + 2);
        qDebug() << "deal " << count << " packet; length is :" << seper_array[count].length();
        count ++;
    }
    float *f_buffer;
    quint32 f_len = 0;
    this->outputFile->setFileName(this->output_path + "\\float_data.txt");
    this->outputFile->open(QIODevice::Append  | QIODevice::Text | QIODevice::WriteOnly);
    QTextStream out(this->outputFile);

    for ( quint32 k = 0; k < count; k ++ ) {
        socket_buffer = (quint8*)malloc(sizeof(quint8)*seper_array[k].length()*8);
        for (quint64 i = 0; i < seper_array[k].length(); i ++) {
            socket_buffer[i] =  seper_array[k].at(i);
        }

        free(socket_buffer);
        f_len = (seper_array[k].length() - 20) / 8 - 2;
        f_buffer = (float*)malloc(sizeof(float)*f_len);
        for (quint64 i = 0; i < (seper_array[k].length() - 20) / 8; i ++) {
            for (quint64 j = 0; j < 8; j ++)
                r.c_8[j]  = seper_array[k].at(2 + i*8 + j);
            if (i == 0) {
                packet_number = r.u_64;
                qDebug() << "packet number :" << packet_number;
            }
            if (i == 1) {
                data_length = r.u_64;
            }
            if (i >= 2) {
                f_buffer[i - 2] = r.f_64;
//                if (i > 2048) {
//                    break;
//                }
            }
        }
        for (quint32 m = 0; m < f_len; m ++) {
            QString number_str = QString::number(f_buffer[m]);
            out << number_str << ",";
        }
        free(f_buffer);
    }
    this->outputFile->close();
}

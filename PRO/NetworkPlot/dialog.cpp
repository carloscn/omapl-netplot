#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->inputFile = new QFile();
    this->outputFile = new QFile();
    this->output_path = "D:\\data";
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


    union result {
        quint64 u_64;
        float f_64;
        char c_8[8];
    } r;

    header_count = input_array.count("abcd");
    tail_count = input_array.count("cdab");

    if ( (header_count != tail_count) ) {
        qDebug() << " header count is :" << header_count;
        qDebug() << " tail count is : " << tail_count;
        header_count -= 1;
    }else {
        qDebug() << "doc include " << tail_count << " packet;";
    }
    seper_array = new QByteArray[tail_count];
    int count = 0;
    input_array.remove(0,input_array.indexOf("cdab") + 4);
    do {
        seper_array[count].append( input_array.mid(input_array.indexOf("abcd") + 4, input_array.indexOf("cdab") ));
        input_array.remove(0,input_array.indexOf("cdab") + 4);
        qDebug() << "deal " << count << " packet; length is :" << seper_array[count].length();
        count ++;
    }while ( count<tail_count-1 );
    qDebug() << "deal headers ok.";

    float *f_buffer;
    quint32 f_len = 0;
    this->outputFile->setFileName(this->output_path + "\\float_data.txt");
    QFileInfo fileInfo(this->outputFile->fileName());
    if (fileInfo.isFile()) {
        QMessageBox::StandardButton rb = QMessageBox::information(NULL,"询问","float_data.txt文件已经存在，是否覆盖写入？",QMessageBox::Yes | QMessageBox::No);
        if(rb == QMessageBox::Yes) {
            this->outputFile->open(QIODevice::Text | QIODevice::WriteOnly);
        }else {
            this->outputFile->open(QIODevice::Append  | QIODevice::Text | QIODevice::WriteOnly);
        }
    }
    if (!this->outputFile->isOpen()) {
        this->outputFile->open(QIODevice::Append  | QIODevice::Text | QIODevice::WriteOnly);
    }
    QTextStream out(this->outputFile);
    for ( quint32 k = 0; k < count-1; k ++ ) {
        // 获取payload的长度值
        f_len = (seper_array[k].length() - 16 - 8) / 8;
        f_buffer = (float*)malloc(sizeof(float)*f_len);
        qDebug() << "payload buffer long:" << f_len;
        // 提取数据包编号32位，数据长度32位，payload f_len*8位
        for (quint64 i = 0; i < f_len + 2 ; i ++) {
            for (quint64 j = 0; j < 8; j ++)
                r.c_8[j]  = seper_array[k].at( i*8 + j);
            if (i == 0) {
                packet_number = r.u_64;
                qDebug() << "packet number :" << packet_number;
                ui->state->setText( QString("处理") +  QString("packet number :") +  QString::number(packet_number));
            }
            if (i == 1) {
                data_length = r.u_64;
            }
            if (i >= 2) {
                f_buffer[i - 2] = r.f_64;
            }
        }
        for (quint32 m = 0; m < f_len; m ++) {
            QString number_str = QString::number(f_buffer[m]);
            out << number_str << ",";
        }
        free(f_buffer);

    }

    delete[] seper_array;
    this->outputFile->close();
    ui->state->setText("分割完毕....");
}

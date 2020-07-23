#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    //Converting QString to num
    ui->textBrowser->clear();
    bool isOk;
    std::uint16_t x = ui->lineEdit->text().toUInt(&isOk);

    //If conversion failed
    if (!isOk)
    {
        QMessageBox box;
        box.setWindowTitle("Error");
        box.setText("Cannot convert to uint16_t!");
        box.exec();
        return;
    }

    //Swap Bytes with log
    ui->textBrowser->append("Bytes before swap:");
    output(x);
    swap(x);
    ui->textBrowser->append("Bytes after swap:");
    output(x);
    ui->textBrowser->append("Final value: " + QString::number(x));
}

void MainWindow::output(std::uint16_t x)
{
    //Serialization in QT-Style
    QByteArray arr;
    arr.append((const char*) &x, sizeof(x));
    ui->textBrowser->append(arr.toHex(' '));
}

void MainWindow::swap(std::uint16_t &x)
{
    //Serialization in C++ Style
    unsigned char *bytesArr = reinterpret_cast<unsigned char*>(&x);
    std::swap(bytesArr[0],bytesArr[1]);
}

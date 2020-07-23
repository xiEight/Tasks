#include "mainwindow.h"
#include "ui_mainwindow.h"

unsigned floored(unsigned x);

unsigned ceiled(unsigned x);

unsigned findPow(unsigned x);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->funcPtr = floored;
}

MainWindow::~MainWindow()
{
    delete ui;
}



//Choosing Rounding Mode
void MainWindow::on_radioButton_2_clicked()
{
    if (ui->radioButton_2->isChecked())
        this->funcPtr = floored;
}

void MainWindow::on_radioButton_clicked()
{
    if (ui->radioButton->isChecked())
        this->funcPtr = ceiled;
}

//Rounding to power of 2
unsigned floored (unsigned x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return findPow(x - (x >> 1));
}


//Rounding to next power of 2
unsigned ceiled (unsigned x)
{
    x -= 1;
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return findPow(x + 1);
}


//Можно обойтись одной лишь этой функцией, без округления к ближайшим степеням двойки,
//но так я продемонстрировал навыки работы с битовыми операциями и указателями на функции.
unsigned findPow(unsigned x)
{
    unsigned val = 1, power = 0;
    while (val < x)
    {
        val *= 2;
        power++;
    }
    return power;
}
void MainWindow::on_pushButton_clicked()
{
    bool isOk;
    unsigned x = ui->lineEdit->text().toUInt(&isOk);
    if (!isOk)
    {
        QMessageBox box;
        box.setWindowTitle("Error");
        box.setText("Cannot convert!");
        box.exec();
        return;
    }
    ui->label_2->setText(QString::number(x) + " ≈ 2^" + QString::number(this->funcPtr(x)));
}

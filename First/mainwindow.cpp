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
}

MainWindow::~MainWindow()
{
    delete ui;
}



unsigned MainWindow::findPow()
{
    std::string s = std::to_string(x);
    auto l =std::bitset<32>(x).to_string();
    std::reverse(l.begin(), l.end());
    return l.find_last_of('1');
}
void MainWindow::on_pushButton_clicked()
{
    if (ui->lineEdit->text().toInt() <= 0)
    {
        ErrorDialog("Number must be greather than zero");
        return;
    }

    bool isOk;
    x = ui->lineEdit->text().toUInt(&isOk);

    if (!isOk)
    {
        ErrorDialog("Wrong unsigned number");
        return;
    }

    ui->label_2->setText(QString::number(x) + " ≈ 2^" + QString::number(findPow()));
}

void MainWindow::ErrorDialog(QString windowText, QString windowTitle)
{
    QMessageBox box;
    box.setWindowTitle(windowTitle);
    box.setText(windowText);
    box.exec();
}

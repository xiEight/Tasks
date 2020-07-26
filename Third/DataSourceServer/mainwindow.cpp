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


void MainWindow::on_pushButton_3_clicked()
{
    server = new DataServer(lowerBound, upperBound);
    uint32_t port;
    bool isOk;
    port = ui->lineEdit_3->text().toUInt(&isOk);
    if (!isOk)
    {
        ShowError("Wrong Port Format!");
        return;
    }
    if (!server->listen(QHostAddress::Any, port))
    {
        ShowError("Cold Start Server");
        return;
    }
    ui->label_5->setText("Server state: enabled");
    ui->pushButton_2->setEnabled(true);
}

void MainWindow::ShowError(QString windowText, QString windowTitle)
{
    QMessageBox box;
    box.setWindowTitle(windowTitle);
    box.setText(windowText);
    box.exec();
}

//Запускаем вещание
void MainWindow::on_pushButton_2_clicked()
{
    server->thread->setState(true);
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
}

//Приостанавливаем вещание
void MainWindow::on_pushButton_clicked()
{
    server->thread->setState(false);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton->setEnabled(false);

}

//Изменяем нижнюю границу
void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    bool isOk;
    double lB = arg1.toDouble(&isOk);
    if (isOk && server != nullptr)
    {
        lowerBound = lB;
        server->setLowerBound(lB);
    }
    else
        return;
}


//Изменяем верхнюю границу
void MainWindow::on_lineEdit_2_textChanged(const QString &arg1)
{
    bool isOk;
    double uB = arg1.toDouble(&isOk);
    if (isOk && server != nullptr)
    {
        upperBound = uB;
        server->setUpperBound(uB);
    }
    else
        return;
}

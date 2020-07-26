#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dataserver.h"
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_lineEdit_textChanged(const QString &arg1);


    void on_lineEdit_2_textChanged(const QString &arg1);

private:
    DataServer *server; //Указатель на TCP Server
    Ui::MainWindow *ui;
    double lowerBound = -10, upperBound = 10; // Нижние границы
    void ShowError(QString windowText, QString windowTitle = "Error"); //Диалог об ошибке
};
#endif // MAINWINDOW_H

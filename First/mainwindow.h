#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <functional>
#include <QMessageBox>
#include <string>
#include <bitset>
#include <algorithm>

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

    void on_pushButton_clicked();

private:
    unsigned x;
    Ui::MainWindow *ui;
    unsigned findPow();
    void ErrorDialog(QString windowText, QString windowTitle = "Error");

};
#endif // MAINWINDOW_H

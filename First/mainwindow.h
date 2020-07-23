#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <functional>
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
    void on_radioButton_2_clicked();

    void on_radioButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    std::function<unsigned(unsigned)> funcPtr; //Function Pointer C++ 11
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <cstdint>
#include <QMessageBox>
#include <bitset>
#include <qdebug.h>
#include <cmath>

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
    Ui::MainWindow *ui;
    std::uint32_t addr;
    std::uint32_t mask;
    void ShowErrorDialog(QString WindowText, QString WindowTitle = "Error");
    void calculate();
    QString ipv4todotted(std::uint32_t ip);
    QString ipv4tobitswithdot(std::uint32_t ip);
};
#endif // MAINWINDOW_H

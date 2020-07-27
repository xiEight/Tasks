#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <algorithm>
#include <QTcpSocket>
#include <QHostAddress>
#include <regex>
#include <QMessageBox>
#include <QDateTime>
#include <qdebug.h>
#include <functional>
#include <QMutex>
#include <thread>
#include <queue>
#include <condition_variable>


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
    void onReadyRead();
    void onConnected();
    void onDisconnected();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_lineEdit_3_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    void customizePlot(); //Инициализация графика
    QTcpSocket *socket; //Указатель на сокет
    QByteArray socketBuf; //Буфер сокета для приема данных с сервера
    QByteArray toServer; //Буфер сокета для отправки на сервер
    void showError(QString windowText, QString windowTitle = "Error"); //Диалог об ошибке
    QDateTime lastReadTime, connectingTime; //Врема последнего чтения, время подключения(отсчет времени (ось Ox) ведется от этого значения
    double seconds,val, minY = -1 ,maxY = 1, painterStartPoint = 0; //Полученное значение, минимум Oy, максимум Oy, точка отсчета Ox
    void Analog(double x, double y); //Эмуляция аналогового осциллографа
    void Electric(double x, double y); //Электронный электронного осцилографа
    std::function<void(double,double)> painter = ([this](double x, double y){Electric(x,y);}); //Указатель C++11 на функцию отрисовки(Analog, Electic)
    QMutex painterMutex; //Мьютекс для обеспечения потокобезопасности при смене типа осциллографа
    //Данные, поступающие с сервера хранятся в очереди вида (x,y)
    //Функция отрисовки занимает процессорное время, поэтому для того, чтобы значения в сокете не были потеряны
    //(если вещать без паузы или с малой паузой, , байты накладываются друг на друга и потеря все равно происходит),
    //при поступлении данных в сокет, они добавлются в очередь
    std::mutex queueMutex; // Мьютекс очереди
    std::condition_variable qMVar; //Переменная, для приостановки while, если очередь пуста
    void PAINT(); //Функция отрисовки, работающая в отдельном потоке
    std::thread *paintThread = nullptr; //Поток для отрисовки графика
    std::queue<std::pair<double,double>> points;//Очередь значений

    const QString commands[2] = {"PAUSE", "GO"};
    bool mainThreadActive = true;
    QMutex smoothMutex;
    double smoothPrev = 0, smooth = 0.2;
    bool firstValue = true;
};
#endif // MAINWINDOW_H

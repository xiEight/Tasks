#ifndef DATATHREAD_H
#define DATATHREAD_H
#include <QThread>
#include <QTcpSocket>
#include <QTime>
#include <thread>
#include <QMutex>
#include <QDebug>
#include <mutex>
#include <functional>
#include <random>
#include <atomic>
#include <cmath>

//Класс, осуществляющий отправку данных на QTcpSocket в отдельном от GUI потоке
class DataThread : public QThread
{
    Q_OBJECT
public:
    DataThread(qintptr socketDescriptor,double lowerBound, double UpperBound, QObject *parent);
    ~DataThread();
    void run() override; //Переопределенный метод работы в отдельном потоке
    void setState(bool state); //Метод для обновления статуса транслирования данных
    double lowerBound = -10, upperBound = 10; //Нижняя/верхняя
    QMutex boundsMutex; //Мьютекс для изменения границ генерируемых данных
    void setRandType(size_t type);
    void setFunc(size_t type);
    void stop();
signals:
    void error(QTcpSocket::SocketError socketError); //Сигнал об ошибке
private:
    QTcpSocket *tcpSocket;
    QDateTime broadStart;
    qintptr socketDescriptor; // Дескриптор сокета
    double value; //Генерируемое значения
    QByteArray buffer; //Буфер для отправки значения
    bool state = false; //Статус вещания
    std::mutex stateMutex; //Мьютекс статуса вещания
    std::condition_variable cv; //Переменная, останавливающая бесконечный цикл в run, до тех пор, пока state == false
    std::atomic<bool> mainStatus;
    //Тип рандома и указатель на генерирующую функцию
    QMutex randomMutex;
    std::function<double ()> randPtr;
    double uniform();
    double exponential();
    double cauchy();
    double normal();

    QMutex funcMutex;
    std::function<double()> func;
    double _sin();
    double _cos();

private slots:
    void onReadyRead();

};

#endif // DATATHREAD_H

#ifndef DATATHREAD_H
#define DATATHREAD_H
#include <QThread>
#include <QTcpSocket>
#include <QTime>
#include <thread>
#include <QMutex>
#include <QDebug>
#include <mutex>

//Класс, осуществляющий отправку данных на QTcpSocket в отдельном от GUI потоке
class DataThread : public QThread
{
    Q_OBJECT
public:
    DataThread(qintptr socketDescriptor,double lowerBound, double UpperBound, QObject *parent);
    void run() override; //Переопределенный метод работы в отдельном потоке
    void setState(bool state); //Метод для обновления статуса транслирования данных
    double lowerBound, upperBound; //Нижняя/верхняя
    QMutex boundsMutex; //Мьютекс для изменения границ генерируемых данных
signals:
    void error(QTcpSocket::SocketError socketError); //Сигнал об ошибке
private:
    qintptr socketDescriptor; // Дескриптор сокета
    double value; //Генерируемое значения
    QByteArray buffer; //Буфер для отправки значения
    bool state = false; //Статус вещания
    std::mutex stateMutex; //Мьютекс статуса вещания
    std::condition_variable cv; //Переменная, останавливающая бесконечный цикл в run, до тех пор, пока state == false

};

#endif // DATATHREAD_H

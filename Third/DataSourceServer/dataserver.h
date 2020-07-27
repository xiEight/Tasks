#ifndef DATASERVER_H
#define DATASERVER_H
#include <QTcpServer>
#include "datathread.h"

//Класс TCPServer'a
class DataServer : public QTcpServer
{
    Q_OBJECT
public:
    DataServer(double lowerBound, double upperBound,QObject *parent = 0);
    ~DataServer();
    DataThread *thread = nullptr; // Указатель на поток, генерирующий случайные данные
    void setLowerBound(double lowerBound); // Функция для обновления нижней границы интервала случайных данных
    void setUpperBound(double upperBound); // Функция для обновления верхней границы интервала случайных данных
    void setRandType(size_t type);
    void setFuncType(size_t type);
protected:
    void incomingConnection(qintptr handle) override; // Переопределенный сигнал входящего соединения
    double lowerBound = -10, upperBound = 10; // Значения нижней и верхней границы

};

#endif // DATASERVER_H

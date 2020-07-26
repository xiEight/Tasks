#include "dataserver.h"

DataServer::DataServer(double lowerBound, double upperBound,QObject *parent) : lowerBound(lowerBound), upperBound(upperBound),
    QTcpServer(parent)
{
    ;
}

//Подключяем сигналы к слотам и запускаем отдельный поток для вещания
void DataServer::incomingConnection(qintptr handle)
{
    thread = new DataThread(handle,lowerBound, upperBound, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

//Обновляем нижнюю границу
void DataServer::setLowerBound(double lowerBound)
{
    if (thread != nullptr)
    {
        thread->boundsMutex.lock();
        thread->lowerBound = lowerBound;
        thread->boundsMutex.unlock();
    }
}

//Обновляем верхнюю границу
void DataServer::setUpperBound(double upperBound)
{
    if (thread != nullptr)
    {
        thread->boundsMutex.lock();
        thread->upperBound = upperBound;
        thread->boundsMutex.unlock();
    }
}

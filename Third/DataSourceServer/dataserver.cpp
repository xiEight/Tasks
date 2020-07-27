#include "dataserver.h"

DataServer::DataServer(double lowerBound, double upperBound,QObject *parent) : QTcpServer(parent), lowerBound(lowerBound), upperBound(upperBound)
{
    ;
}

//Подключяем сигналы к слотам и запускаем отдельный поток для вещания
void DataServer::incomingConnection(qintptr handle)
{
    thread = new DataThread(handle,lowerBound, upperBound, this);
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


void DataServer::setRandType(size_t type)
{
    if (thread != nullptr)
        thread->setRandType(type);
}

void DataServer::setFuncType(size_t type)
{
    if (thread != nullptr)
        thread->setFunc(type);
}

DataServer::~DataServer()
{
    if (thread != nullptr)
        delete thread;
}


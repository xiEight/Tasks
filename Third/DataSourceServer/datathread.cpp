#include "datathread.h"

DataThread::DataThread(qintptr socketDescriptor,double lowerBound, double UpperBound, QObject *parent)
    : lowerBound(lowerBound), upperBound(UpperBound)
{
    this->socketDescriptor = socketDescriptor;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime())); //Seed для генерации случайных чисел

}

void DataThread::run()
{
    QTcpSocket *tcpSocket = new QTcpSocket;
    //Проверяем состояние сокета
    if (!tcpSocket->setSocketDescriptor(socketDescriptor))
    {
        emit error(tcpSocket->error());
        return;
    }

    while(true)
    {
        //Приостанавливаем выполнения цикла до тех пор, пока state == false
        std::unique_lock<std::mutex> uState(stateMutex);
        cv.wait(uState,[this](){return state;});
        //Генерируем значение
        boundsMutex.lock();
        value = lowerBound +  ((double) qrand() / RAND_MAX) * (upperBound - lowerBound);
        boundsMutex.unlock();
        //Конвертируем значение в байты
        buffer = QByteArray();
        buffer = QByteArray(reinterpret_cast<const char*>(&value), sizeof(value));
        //Отправка данных
        tcpSocket->write(buffer.data());
        tcpSocket->waitForBytesWritten(5000000);
        //Вынужденная пауза в 0.1 секунды, т.к. без нее клиенту приходят "мусорные" значения
        //Пытался исправить путем использования QDataStream - ситуация не изменилась
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

//Установка состояния вещания
void DataThread::setState(bool state)
{
    this->state = state;
    cv.notify_one();
}

#include "datathread.h"

DataThread::DataThread(qintptr socketDescriptor,double lowerBound, double UpperBound, QObject *parent)
    : lowerBound(lowerBound), upperBound(UpperBound)
{
    mainStatus.store(true);
    this->socketDescriptor = socketDescriptor;
    randPtr = [this](){return uniform();};
    func = [this](){return  _sin() + randPtr();};

    this->tcpSocket = new QTcpSocket;
    if (!this->tcpSocket->setSocketDescriptor(socketDescriptor))
    {
        emit error(tcpSocket->error());
        return;
    }
    connect(this->tcpSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void DataThread::run()
{
    //QTcpSocket *tcpSocket = new QTcpSocket;
    //Проверяем состояние сокета
    broadStart = QDateTime::currentDateTimeUtc();
    if (tcpSocket == nullptr)
        return;
    while(true)
    {
        //Приостанавливаем выполнения цикла до тех пор, пока state == false
        std::unique_lock<std::mutex> uState(stateMutex);
        cv.wait(uState,[this](){return state || !mainStatus.load();});
        if (!mainStatus.load())
            return;

        //Генерируем значение
        boundsMutex.lock();
        value = func();
        boundsMutex.unlock();
        //Конвертируем значение в байты
        buffer = QByteArray();
        buffer = QByteArray(reinterpret_cast<const char*>(&value), sizeof(value));
        //Отправка данных
        if (tcpSocket->state() == QTcpSocket::UnconnectedState)
        {
            tcpSocket->close();
            return;
        }
        tcpSocket->write(buffer.data());
        tcpSocket->waitForBytesWritten(5000000);
        //Вынужденная пауза в 0.1 секунды, т.к. без нее клиенту приходят "мусорные" значения
        //Пытался исправить путем использования QDataStream - ситуация не изменилась
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        //Для "красивого/читаемого" графика промежуток между данными лучше ставить в секунду
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

//Установка состояния вещания
void DataThread::setState(bool state)
{
    this->state = state;
    cv.notify_one();
}

//Равномерное распределение
double DataThread::uniform()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(this->lowerBound, this->upperBound);
    return dis(gen);
}

//Экспоненциальное распределение
double DataThread::exponential()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<> d(1);
    return d(gen);
}

//Распределение Коши
double DataThread::cauchy()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::cauchy_distribution<> r(this->lowerBound,this->upperBound);
    return r(gen);
}

//Нормальное распределение
double DataThread::normal()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(this->lowerBound, this->upperBound);
    return d(gen);
}

//Измение типа распределения случайной величины
void DataThread::setRandType(size_t type)
{
    randomMutex.lock();
    switch (type) {
    case 0:
        randPtr = [this](){return uniform();};
        break;
    case 1:
        randPtr = [this](){return normal();};
        break;
    case 2:
        randPtr = [this](){return exponential();};
        break;
    case 3:
        randPtr = [this](){return cauchy();};
        break;
    default:
        break;
    }
    randomMutex.unlock();
}

//Изменение функции, генерирующей сигнал
void DataThread::setFunc(size_t type)
{
    funcMutex.lock();
    switch (type) {
        case 0:
            func = [this](){return _sin() + randPtr();};
            break;
        case 1:
            func = [this](){return _cos() + randPtr();};
            break;
    case 2:
        func = [this](){return _sin();};
        break;
    case 3:
        func = [this] () {return _cos();};
        default:
            break;

    }
    funcMutex.unlock();
}


double DataThread::_sin()
{
    return sin(broadStart.secsTo(QDateTime::currentDateTimeUtc()));
}

double DataThread::_cos()
{
    return cos(broadStart.secsTo(QDateTime::currentDateTimeUtc()));

}

void DataThread::onReadyRead()
{
    QByteArray buf = tcpSocket->readAll();
    QString command = buf;
    if (command == "PAUSE")
    {
        state = false;
        cv.notify_one();
    }
    else if (command == "GO")
    {
        state = true;
        cv.notify_one();
    }
}

DataThread::~DataThread()
{
    if (tcpSocket != nullptr)
        delete tcpSocket;
}

void DataThread::stop()
{
    mainStatus.store(false);
    cv.notify_one();
}

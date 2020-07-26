#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plot->addGraph();
    customizePlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Настройка внешнего вида графика
void MainWindow::customizePlot()
{
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->plot->xAxis->grid()->setPen(QPen(Qt::GlobalColor::black, Qt::DotLine));
    ui->plot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->plot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->plot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->plot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->plot->xAxis->grid()->setSubGridVisible(true);
    ui->plot->yAxis->grid()->setSubGridVisible(true);
    ui->plot->graph(0)->setPen(QPen(Qt::GlobalColor::black));
    ui->plot->graph(0)->addData(0,0);
    ui->plot->rescaleAxes();
    ui->plot->replot();
}

//Ввод IP и порта сервера
void MainWindow::on_pushButton_clicked()
{
    //Reg exp для валидации айпи(формат xxx.xxx.xxx.xxx)
    std::regex pattern("([0-9]{1,3}[\\.]){3}[0-9]{1,3}");
    std::string ip = ui->lineEdit->text().toStdString();
    if (!std::regex_match(ip.cbegin(),ip.cend(), pattern))
    {
        showError("Wrong IP address!");
    }
    bool isOk;
    uint32_t port = ui->lineEdit_2->text().toUInt(&isOk);
    if (!isOk)
    {
        showError("Wrong Port!");
        return;
    }
    //Подключение сокета к серверу
    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress(ui->lineEdit->text()), port);
    connect(socket, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(socket, SIGNAL(connected()), SLOT(onConnected()));
    connect(socket, SIGNAL(disconnected()), SLOT(onDisconnected()));
}

void MainWindow::onReadyRead()
{
    //Обновляем время последнего чтения данных
    lastReadTime = QDateTime::currentDateTimeUtc();
    //Обновляем координату X
    seconds = connectingTime.secsTo(lastReadTime);
    socket->waitForBytesWritten();
    //Чтение и конвертация координаты Y
    socketBuf = socket->readAll();
    val = *reinterpret_cast<const double*>(socketBuf.data());
    //Добавление точки в очередь с блокировкой мьютекса
    queueMutex.lock();
    points.push(std::make_pair(seconds,val));
    queueMutex.unlock();
    //Уведомляем conditional variable о том, что очередь изменилась и вызов предиката предиката
    qMVar.notify_one();
}

//Сигнал подключения к серверу
void MainWindow::onConnected()
{
    connectingTime = QDateTime::currentDateTimeUtc();
    qDebug() << "Connected!";
    ui->label_3->setText("Connecting State: Connected");
    paintThread = new std::thread([this](){PAINT();});
    paintThread->detach();
}

//Сигнал отключения от сервера
void MainWindow::onDisconnected()
{
     ui->label_3->setText("Connectiong State: Disconnected");
}

//Диалог об ошибке
void MainWindow::showError(QString windowText, QString windowTitle)
{
    QMessageBox box;
    box.setWindowTitle(windowTitle);
    box.setText(windowText);
    box.exec();
}

//Аналоговый режим осциллографа
//Из-за того, что значения поступают очень быстро, плохо виден спуск к значению 0 по оси Oy (ширина прямоугольника
//на координатной оси образованного точками (Y,время чтения), (Y, текущее время) стремится к нулю, т.к
//время между получаемыми данными так же стремится к нулю
//Так же из-за этого, прямые накладываются друг на друга
//Единственное решение, которое я придумал: изменить данные генерируемые сервером на пару (сигнал, длительность сигнала)
void MainWindow::Analog(double x, double y)
{
    ui->plot->graph(0)->addData(x,0);
    ui->plot->graph(0)->addData(x, y);
    if (y > maxY)
        maxY = y;
    if (y < minY)
        minY = y;
    ui->plot->yAxis->setRange(minY,maxY);
    ui->plot->graph(0)->addData(connectingTime.secsTo(QDateTime::currentDateTime()), y);
        ui->plot->graph(0)->addData(connectingTime.secsTo(QDateTime::currentDateTime()), 0);
    ui->plot->xAxis->setRange(painterStartPoint,x + 1);
    ui->plot->replot();
}

//Электронный режим осцилографа
void MainWindow::Electric(double x, double y)
{
    ui->plot->graph(0)->addData(x, y);
    if (y > maxY)
        maxY = y;
    if (y < minY)
        minY = y;
    ui->plot->yAxis->setRange(minY,maxY);
    ui->plot->xAxis->setRange(painterStartPoint,x + 1);
    ui->plot->replot();

}

//Изменяем режим осциллографа, очищая при этом график
void MainWindow::on_radioButton_clicked()
{
    ui->plot->graph(0)->data()->clear();
    painterMutex.lock();
    painterStartPoint = seconds;
    painter = ([this](double x, double y){Analog(x,y);});
    painterMutex.unlock();
}

//Изменяем режим осциллографа, очищая при этом график
void MainWindow::on_radioButton_2_clicked()
{
    ui->plot->graph(0)->data()->clear();
    painterMutex.lock();
    painterStartPoint = seconds;
    painter = ([this](double x, double y){Electric(x,y);});
    painterMutex.unlock();
}

//Метод, работающий в отдельном потоке для отрисовки графика
void MainWindow::PAINT()
{
    while(true)
    {
        //Если очередь пуста, ожидаем до тех пор, пока состояние не изменится
        std::unique_lock<std::mutex> ulqM(queueMutex);
        qMVar.wait(ulqM,([this](){return points.size() > 0;}));
        //Отрисовка начала очереди
        auto curr = points.front();
        points.pop();
        painterMutex.lock();
        painter(curr.first,curr.second);
        painterMutex.unlock();
    }
}

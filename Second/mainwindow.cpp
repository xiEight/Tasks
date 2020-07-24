#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


bool comp(const std::pair<uint32_t, uint32_t> &a, const std::pair<uint32_t, uint32_t> &b);

void MainWindow::on_pushButton_clicked()
{
    if (!ui->lineEdit->text().count("/"))
    {
        ShowErrorDialog("Wrong input format!");
        return;
    }

    bool isOk;
    QStringList input = ui->lineEdit->text().split("/");
    this->addr = input.at(0).toUInt(&isOk);
    if (!isOk)
    {
        ShowErrorDialog("Wrong IPv4 address!");
        return;
    }
    this->mask = input.at(1).toUInt(&isOk);
    if (!isOk)
    {
        ShowErrorDialog("Wrong Subnet Mask!");
        return;
    }
    calculate();
}

void MainWindow::ShowErrorDialog(QString WindowText, QString WindowTitle)
{
    QMessageBox box;
    box.setWindowTitle(WindowTitle);
    box.setText(WindowText);
    box.exec();
}


//Method for calculating the network address, broadcast address, number of addresses for a subnet and number of addresses for hosts.
void MainWindow::calculate()
{
    ui->textBrowser->clear();

    //Outputting input data
    ui->textBrowser->append("Bit representation of IPv4 address:\n" + ipv4tobitswithdot(addr));
    ui->textBrowser->append("Decimal representation of IPv4 address:\n" + ipv4todotted(addr));
    ui->textBrowser->append("Bit representation of subnet mask:\n" + ipv4tobitswithdot(mask));
    ui->textBrowser->append("Decimal representation of the subnet mask:\n" + ipv4todotted(mask));

    //IPv4 and Subnet Mask to bitsets
    std::bitset<32> addrbs(addr);
    std::bitset<32> maskbs(mask);

    //Conversion to QString for further calculations
    QString addrstr = QString::fromStdString(addrbs.to_string());
    QString maskStr = QString::fromStdString(maskbs.to_string());
    QString addressString = ipv4todotted(addr);

    //Calculating the number of addresses for a subnet and the last octet of IPv4
    long countAddres = 0;
    for (int i = maskStr.length() - 1; i >= 0 && maskStr.at(i) == '0'; i--)
        countAddres++;

    countAddres = pow(2,countAddres);

    std::bitset<32> networkBS = addrbs & maskbs;
    std::bitset<32> broadkastBS = addrbs | maskbs.flip();
    //Converting Results to QString for output
    QString networkAddr = ipv4todotted(networkBS.to_ulong());
    QString broadAddre = ipv4todotted(broadkastBS.to_ulong());
    //Output results
    ui->textBrowser->append("Number of addresses for a subnet: " + QString::number(countAddres));
    ui->textBrowser->append("Number of addresses for hosts: " + QString::number(countAddres - 2));
    ui->textBrowser->append("Network Address: " + networkAddr);
    ui->textBrowser->append("Broadcast address: " + broadAddre);
}

//Converting IPv4 address to decimal representation with dot separator
QString MainWindow::ipv4todotted(std::uint32_t ip)
{
    std::bitset<32> bs(ip);
    QString bitstring(QString::fromStdString(bs.to_string()));
    std::vector<std::bitset<8>> oktets = {std::bitset<8>(bitstring.mid(0,8).toStdString()),
    std::bitset<8>(bitstring.mid(8,8).toStdString()), std::bitset<8>(bitstring.mid(16,8).toStdString()),
                                         std::bitset<8>(bitstring.mid(24,8).toStdString())};
    return QString::number(oktets[0].to_ulong()) + "." + QString::number(oktets[1].to_ulong())
            + "." + QString::number(oktets[2].to_ulong()) + "." + QString::number(oktets[3].to_ulong());
}

//Converting IPv4 address into bit representation with dot separator
QString MainWindow::ipv4tobitswithdot(std::uint32_t ip)
{
    std::bitset<32> bs(ip);
    QString rez(QString::fromStdString(bs.to_string()));
    rez.insert(8,'.');
    rez.insert(17,'.');
    rez.insert(26,'.');
    return rez;
}

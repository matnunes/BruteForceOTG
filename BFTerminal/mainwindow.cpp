/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"

#include <QDateTime>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_console(new Console),
    m_settings(new SettingsDialog),
    m_serial(new QSerialPort(this))

{
    m_ui->setupUi(this);
    m_console->setEnabled(false);
    setCentralWidget(m_console);

    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);

    m_ui->statusBar->addWidget(m_status);

    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);

    bfWorkerThread = new BFWorker(this);
    connect(this,SIGNAL(setWordlist(QStringList)),bfWorkerThread,SLOT(setWordlist(QStringList)));
    connect(bfWorkerThread,SIGNAL(updateConsole(QString)),this,SLOT(updateConsole(QString)));
}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
}

void MainWindow::openSerialPort()
{
    if (bfWorkerThread->connectBFClient(m_settings->settings()))
    {
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
    showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
        .arg(m_settings->settings().name).arg(m_settings->settings().stringBaudRate).arg(m_settings->settings().stringDataBits)
        .arg(m_settings->settings().stringParity).arg(m_settings->settings().stringStopBits).arg(m_settings->settings().stringFlowControl));
    }
    else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{   
    if (bfWorkerThread->isConnected())
    {
        bfWorkerThread->disconnectBFClient();

        m_console->setEnabled(false);
        m_ui->actionConnect->setEnabled(true);
        m_ui->actionDisconnect->setEnabled(false);
        showStatusMessage(tr("Disconnected"));
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Brute Force OTG"),
                       tr("Brute force OTG password recovery for locked phones."));
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::openWordlist()
{
    QString fileContent;
    QString filename = QFileDialog::getOpenFileName(this, "Open wordlist");

    if (!(filename.isEmpty()))
    {
        QFile file(filename);
        QStringList wordlist;

        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QFile textFile;

            QTextStream in(&file);
            while (!in.atEnd())
                wordlist << in.readLine();

            file.close();
            emit setWordlist(wordlist);
            updateConsole(QString("wordlist loaded "+ QString::number(wordlist.size())+" items."));
        }
    }
}
void MainWindow::wakePhone()
{
    if (bfWorkerThread->isConnected())
    {
        bfWorkerThread->wakePhone();
    }
}

void MainWindow::getPhotosensor()
{
    if (bfWorkerThread->isConnected())
    {
        while (bfWorkerThread->getPhotosensor() == 0)
            bfWorkerThread->getPhotosensor();
        int value = QInputDialog::getInt(this,"Photosensor","Screen off value:");
        bfWorkerThread->screenOff = value;
        updateConsole(QString("screen off default set to "+QString::number(value)+"."));
    }
}

void MainWindow::sendCalibrate()
{
    if (bfWorkerThread->isConnected())
    {
        bfWorkerThread->displayCalibration();
    }
}

void MainWindow::updateConsole(QString text)
{
    m_console->putData(QString(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+": "+text).append("\r\n").toUtf8());
}

void MainWindow::startBFWorker()
{
    if (bfWorkerThread->isConnected())
    {
        int startPos = QInputDialog::getInt(this,"Start index","Start from index:");
        updateConsole(QString("start from index "+QString::number(startPos)+"."));
        bfWorkerThread->startIndex = startPos;
        bfWorkerThread->start();
    }
}

void MainWindow::stopBFWorker()
{
    if (bfWorkerThread->isRunning())
        bfWorkerThread->exit();
}

void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionClear, &QAction::triggered, m_console, &Console::clear);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(m_ui->actionOpenDict, &QAction::triggered, this, &MainWindow::openWordlist);
    connect(m_ui->actionWakePhone, &QAction::triggered, this, &MainWindow::wakePhone);
    connect(m_ui->actionPhotoSensor, &QAction::triggered, this, &MainWindow::getPhotosensor);
    connect(m_ui->actionStartBF, &QAction::triggered, this, &MainWindow::startBFWorker);
    connect(m_ui->actionStopBF, &QAction::triggered, this, &MainWindow::stopBFWorker);
    connect(m_ui->actionCalibration, &QAction::triggered, this, &MainWindow::sendCalibrate);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

#include "networktransfer.h"
#include "serverthread.h"
#include "common.h"
#include <QTimer>

void ServerClass::incomingConnection(int handle)
{
    emit clientConnected(handle);
}

NetworkTransfer::NetworkTransfer(const QString &ipAddress, const quint16 &port, const qint64& dataSize)
{
    logMessage(tr("NetworkTransfer thread = %1").arg( (unsigned long)QThread::currentThread()) );
    m_net = 0;
    sendSize = dataSize;
    connect(&m_server, SIGNAL(clientConnected(int)), this, SLOT(clientConnected(int)));
    connect(&m_server, SIGNAL(clientConnected(int)), this, SLOT(startTransfer()));
    m_server.listen(QHostAddress(
                ipAddress),
                port);
}

NetworkTransfer::~NetworkTransfer()
{
    m_net->deleteLater();
}

void NetworkTransfer::clientConnected(int handle)
{
    m_net = new ServerThread(handle, sendSize);
    connect(m_net, SIGNAL(progressChange(int)), this, SLOT(progressChange(int)));
    connect(m_net, SIGNAL(log(const QString&)), this, SLOT(logMessage(const QString&)));
    m_net->start();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(startTransfer()));
    timer->start(5000);
}

void NetworkTransfer::progressChange(int progress)
{
//    ui.progressBar->setValue(progress);
}

void NetworkTransfer::logMessage(const QString &msg)
{
    QTextStream out(stdout);
    out << "\n" << msg;
}

void NetworkTransfer::startTransfer()
{
    m_net->startTransfer();
}

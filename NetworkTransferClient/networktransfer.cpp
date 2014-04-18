#include "networktransfer.h"
#include "clientthread.h"
#include <QTranslator>
#include <QTimer>

NetworkTransfer::NetworkTransfer(const QString &ipAddress, const quint16 &port, const qint64& dataSize)
{
    logMessage( tr("NetworkTransfer thread = %1").arg( (unsigned long)QThread::currentThread() ) );
    m_net = new ClientThread(ipAddress, port, dataSize);
    connect(m_net, SIGNAL(log(const QString&)), this, SLOT(logMessage(const QString&)));
    m_net->start();
}

NetworkTransfer::~NetworkTransfer()
{
    m_net->deleteLater();
}

void NetworkTransfer::progressChange(int progress)
{
    //ui.progressBar->setValue(progress);
}

void NetworkTransfer::logMessage(const QString &msg)
{
    QTextStream out(stdout);
    out << "\n" << msg;
}

void NetworkTransfer::connectionLost()
{
    QTextStream out(stdout);
    out << "\n Connection lost.";
}

void NetworkTransfer::startTransfer()
{
    m_net->startTransfer();
}

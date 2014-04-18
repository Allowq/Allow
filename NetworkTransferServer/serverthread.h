#pragma once

#include "networkthread.h"
#include <QTime>
#include <QTcpSocket>
#include <QMutex>
#include <QFile>
#include <QCoreApplication>

class ServerThread : public NetworkThread
{
    Q_OBJECT

protected:
    int m_socketDesc;
    qint64 m_sendSize;
    void run();

public:
    ServerThread(int socketDesc, qint64 dataSize);
    void uploadSpeed();

private:
    qint64 sendBigFile(QTcpSocket *socket);
    void receiveCMDtoStartTransfer(QTcpSocket *socket);
};

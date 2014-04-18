#pragma once

#include "networkthread.h"
#include <QTcpSocket>
#include <QTime>
#include <QTimer>
#include <QBuffer>
#include <QFile>

class ClientThread : public NetworkThread
{
    Q_OBJECT

protected:
    QString m_serverAddress;
    quint16 m_serverPort;
    qint64 m_sendSize;

    void run();

public:
    ClientThread(const QString& serverAddress, const quint16& serverPort, const qint64& dataSize);

private:
    void sendCMDtoStartTransfer(QTcpSocket *socket);
};

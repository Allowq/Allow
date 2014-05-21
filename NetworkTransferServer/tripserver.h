#ifndef TRIPSERVER_H
#define TRIPSERVER_H

#include <QTcpServer>
#include <QUdpSocket>
#include <QTcpSocket>
#include "clientsocket.h"

class TripServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TripServer(
            const QString ipAddress = "localhost",
            const quint16 port = 1337,
            const qint64 dataSize = 100,
            const bool applyUDP = false);

private:
    QString m_ipAddress;
    quint16 m_port;
    qint64 m_dataSize;
    bool m_applyUDP;
    void incomingConnection(int socketId);
    void showStartupOptions();

signals:

public slots:

};

#endif // TRIPSERVER_H

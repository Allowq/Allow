#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QCoreApplication>
#include <QAbstractSocket>
#include <QUdpSocket>
#include <QTimer>
#include <iostream>     // std::cout
#include <new>          // std::bad_alloc

class ClientSocket : public QAbstractSocket
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = 0,
                          const SocketType socketType = TcpSocket,
                          const qint64 dataSize = 100,
                          const QString ipAddress = "localhost",
                          const quint16 port = 1337);

private:
    QUdpSocket *UDPSocket;
    QString m_ipAddress;
    quint16 m_port;
    qint64 m_dataSize;
    void sendDataToClient();
    void sendDataToClientUDP();

private slots:
    void readClient();
    void processPendingDatagrams();

signals:
    void log(const QString &msg);

protected slots:
    void logMessage(const QString &msg);
};

#endif // CLIENTSOCKET_H

#ifndef TRIPCLIENT_H
#define TRIPCLIENT_H

#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTime>
#include <QTimer>
#include <windows.h>

class TripClient : public QAbstractSocket
{
    Q_OBJECT
public:
    explicit TripClient(const QString ipAddress = "localhost",
                        const quint16 port = 1337,
                        const SocketType socketType = TcpSocket);

private:
    QUdpSocket *UDPSocket;
    QString m_ipAddress;
    quint16 m_port;
    void showStartupOptions();
    void sendCommandGET();
    void sendUDPCommandGET();
    void sendCommandTime(int);
    void sendUDPCommandTime(int);

signals:
    void log(const QString &msg);

private slots:
    void processPendingDatagrams();

protected slots:
    void receiveData();
    void logMessage(const QString &msg);
};

#endif // TRIPCLIENT_H

#ifndef TRIPSERVER_H
#define TRIPSERVER_H

#include <QTcpServer>
#include <clientsocket.h>

class TripServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TripServer(const QStringList &startupOptions);

private:
    QString m_ipAddress;
    quint16 m_port;
    qint64 m_dataSize;
    void incomingConnection(int socketId);
    void getStartupOptions(const QStringList &startupOptions);
    void showStartupOptions();

signals:

public slots:

};

#endif // TRIPSERVER_H

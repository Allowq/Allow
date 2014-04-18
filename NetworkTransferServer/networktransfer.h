#ifndef NETWORKTRANSFER_H
#define NETWORKTRANSFER_H

#include <QtGui>
#include <QHostInfo>

class NetworkThread;

#include <QTcpServer>

class ServerClass : public QTcpServer
{
    Q_OBJECT
protected:
    void incomingConnection(int handle);

signals:
    void clientConnected(int socketDesc);
};

class NetworkTransfer : public QObject
{
    Q_OBJECT

public:
    NetworkTransfer(const QString&, const quint16&, const qint64&);
    QString getIpAddr(int);
    ~NetworkTransfer();

protected slots:
    void clientConnected(int);
    void progressChange(int progress);
    void logMessage(const QString& msg);
    void startTransfer();

private:
    ServerClass m_server;
    NetworkThread* m_net;
    qint64 sendSize;
};

#endif // NETWORKTRANSFER_H

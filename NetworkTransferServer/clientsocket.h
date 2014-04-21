#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QTcpSocket>
#include <QCoreApplication>

class ClientSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = 0, qint64 dataSize = 100);

private:
    qint64 m_dataSize;
    void sendDataToClient();

private slots:
    void readClient();

signals:
    void log(const QString &msg);

protected slots:
    void logMessage(const QString &msg);
};

#endif // CLIENTSOCKET_H

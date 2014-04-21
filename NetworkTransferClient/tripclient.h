#ifndef TRIPCLIENT_H
#define TRIPCLIENT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QTime>
#include <QTimer>

class TripClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TripClient(const QStringList &startupOptions);

private:
    QString m_ipAddress;
    quint16 m_port;
    void getStartupOptions(const QStringList &startupOptions);
    void showStartupOptions();
    void sendCommandGET();
    void sendCommandTime(int);

signals:
    void log(const QString &msg);

protected slots:
    void receiveData();
    void logMessage(const QString &msg);
};

#endif // TRIPCLIENT_H

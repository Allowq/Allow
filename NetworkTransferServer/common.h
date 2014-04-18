#ifndef COMMON_H
#define COMMON_H

#include <QHostInfo>
#include <QDebug>
#include <QCoreApplication>
#include <QKeyEvent>

class AppOptions : public QObject
{
    Q_OBJECT

public:
    explicit AppOptions(QObject *parent = 0);
    QString getIpAddr(int);
    bool getStartupOptions(const QStringList&, QString&, quint16&, qint64&);

private:
    static void exitQt(int sig);
};

#endif // COMMON_H

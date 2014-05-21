#ifndef COMMON_H
#define COMMON_H

#include <QCoreApplication>
#include <QDebug>
#include <signal.h>

class AppOptions : public QObject
{
    Q_OBJECT

public:
    explicit AppOptions(QObject *parent = 0);

private:
    static void exitQt(int sig);
};



#endif // COMMON_H

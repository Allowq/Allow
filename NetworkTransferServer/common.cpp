#include <common.h>

static AppOptions *pCleanExit;

AppOptions::AppOptions(QObject *parent ) : QObject(parent)
{
    pCleanExit = this;
    signal(SIGINT, &AppOptions::exitQt);
    signal(SIGTERM, &(AppOptions::exitQt));
}

void AppOptions::exitQt(int sig)
{
    qDebug() << "\n\nShutdown application CTRL+C.";
    QCoreApplication::exit(0);
}

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

QString AppOptions::getIpAddr(int indx)
{
    QString ipAddress = "";
    QHostInfo info = QHostInfo::fromName(
                QHostInfo::localHostName()
                );
    QList<QHostAddress> listAddr = info.addresses();
    if(indx >= listAddr.size())
        indx = listAddr.size() - 1;
    for(int i = 0; i < listAddr.size(); i ++)
        if( listAddr.at(i).protocol() == QAbstractSocket::IPv4Protocol && indx == i)
            ipAddress += listAddr.at(i).toString();
    //qDebug() << "Connect to server with ip address - " + ipAddress;
    return ipAddress;
}

bool AppOptions::getStartupOptions(const QStringList &startupOptions, QString &ipAddress, quint16 &port, qint64 &dataSize)
{
    if(startupOptions.size() > 1)
    {
        if(startupOptions.contains("set"))
        {
            ipAddress.clear();
            ipAddress.append(startupOptions.at(2));
        //    ipAddress.append(getIpAddr(2));
        }
        if(startupOptions.size() > 3)
        {
            bool ok;
            quint16 tPort = startupOptions.at(3).toUInt(&ok,10);
            if(ok)
                port = tPort;
        }
        if(startupOptions.size() > 4)
        {
            bool ok;
            quint64 tSize = startupOptions.at(4).toUInt(&ok,10);
            if(ok)
                dataSize = tSize;
        }
        return true;
    }
    return false;
}

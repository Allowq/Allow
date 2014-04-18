#include <QCoreApplication>
#include <QStringList>
#include <common.h>
#include "networktransfer.h"

int main(int argc, char *argv[])
{
    QCoreApplication client(argc, argv);
    AppOptions clientOpt;
    QTextStream out(stdout);
    QStringList startupOptions = client.arguments();
    QString ipAddress = "localhost";
    quint16 port = 1488;
    qint64 dataSize = 50;
    out << "    Client startup options\n"
        << "Address of the connection to the server - ";
    if(clientOpt.getStartupOptions(startupOptions, ipAddress, port, dataSize))
        out << ipAddress << ":" << port << "\n"
            << "Client sends - " << dataSize << "Mbyte\n";
    else
        out << ipAddress << ":" << port << "\n"
            << "Client sends - " << dataSize << "Mbyte\n";
    out.flush();
    NetworkTransfer w(ipAddress, port, dataSize);
    return client.exec();
}

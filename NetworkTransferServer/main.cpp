#include <QCoreApplication>
#include <QStringList>
#include "common.h"
#include "networktransfer.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication server(argc, argv);
    AppOptions clientOpt;
    QTextStream out(stdout);
    QStringList startupOptions = server.arguments();
    QString ipAddress = "localhost";
    quint16 port = 1488;
    qint64 dataSize = 50;
    out << "    Server startup options\n"
        << "The server is running at ";
    if(clientOpt.getStartupOptions(startupOptions, ipAddress, port, dataSize))
        out << ipAddress << ":" << port << "\n"
            << "Server sends - " << dataSize << "Mbyte\n";
    else
        out << ipAddress << ":" << port << "\n"
            << "Server sends - " << dataSize << "Mbyte\n";
    out.flush();
    NetworkTransfer w(ipAddress, port, dataSize);
    return server.exec();
}

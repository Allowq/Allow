#include <QCoreApplication>
#include <QStringList>
#include "tripserver.h"
#include "common.h"

int main(int argc, char *argv[])
{
    QCoreApplication server(argc, argv);
    AppOptions clientOpt;
    TripServer *srv = new TripServer(server.arguments());
    return server.exec();
}

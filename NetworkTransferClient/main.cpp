#include <QCoreApplication>
#include "common.h"
#include "tripclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication client(argc, argv);
    AppOptions clientOpt;
    TripClient *clnt = new TripClient(client.arguments());
    return client.exec();
}

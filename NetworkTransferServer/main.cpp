#include <QCoreApplication>
#include <QStringList>
#include <string.h>
#include "getopt.h"
#include "tripserver.h"
#include "common.h"

void usage();

int main(int argc, char *argv[])
{
    /* Getopt */
    extern char *optarg;
    register int opt;

    /* Flags */
    bool applyUDP = false;
    bool setIP = false;
    bool setPort = false;
    bool setSize = false;

    /* Startup parameters */
    int serverPort = 0;
    QString serverIPAddress;
    qint64 dataSize = 0;

    QCoreApplication server(argc, argv);

    /* Get options */
    while((opt = getopt(argc, argv, "i:p:s:uh")) != EOF)
    {
        switch(opt)
        {
        /* Help */
        case 'h' :
            usage();
            return(EXIT_SUCCESS);

        /* Verbose mode */
        case 'u' :
            applyUDP = true;
            break;

        /* Server port */
        case 'p' :
            serverPort = atoi(optarg);
            setPort = true;
            break;

        /* Server IP address */
        case 'i' :
            if((strlen(optarg) < 7) || (strlen(optarg) > 15)) {
                usage();
                exit(EXIT_FAILURE);
            }

            serverIPAddress = QString::fromStdString(optarg);
            setIP = true;
            break;

        /* Size of data */
        case 's' :
            dataSize = atoi(optarg);
            setSize = true;
            break;
        }
    }

    AppOptions appOpt;
    TripServer *srv;
    if(setIP && setPort && setSize && applyUDP)
        srv = new TripServer(serverIPAddress, serverPort, dataSize, applyUDP);
    else
    {
        if(setIP && setPort && setSize)
            srv = new TripServer(serverIPAddress, serverPort, dataSize);
        else
        {
            if(setIP && setPort)
                srv = new TripServer(serverIPAddress, serverPort);
            else
            {
                if(setIP)
                    srv = new TripServer(serverIPAddress);
                else
                    srv = new TripServer();
            }
        }
    }

    return server.exec();
}

void usage()
{
    printf(">> Network Transfer Server by Allow (AllowL@mail.ru) <<\n");

    printf("\n Usage options:\n");
    printf("  -i <ip>        - ip address server\n");
    printf("  -p <port>      - transfer server port\n");
    printf("  -d <data size> - size of data to send on client\n");
    printf("  -u             - use UDP-protocol for transfer data\n");
    printf("  -h             - show hint (help)\n");
}

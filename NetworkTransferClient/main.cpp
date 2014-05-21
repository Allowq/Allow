#include <QCoreApplication>
#include "getopt.h"
#include "common.h"
#include "tripclient.h"

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

    /* Startup parameters */
    int clientPort;
    QString clientIPAddress;

    /* Get options */
    while((opt = getopt(argc, argv, "i:p:uh")) != EOF)
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
            clientPort = atoi(optarg);
            setPort = true;
            break;

        /* Server IP address */
        case 'i' :
            if((strlen(optarg) < 7) || (strlen(optarg) > 15)) {
                usage();
                exit(EXIT_FAILURE);
            }

            clientIPAddress = QString::fromStdString(optarg);
            setIP = true;
            break;
        }
    }

    QCoreApplication client(argc, argv);
    AppOptions clientOpt;
    TripClient *clnt;
    if(setIP && setPort && applyUDP)
        clnt = new TripClient(clientIPAddress, clientPort, QAbstractSocket::UdpSocket);
    else
    {
        if(setIP && setPort)
            clnt = new TripClient(clientIPAddress, clientPort);
        else
        {
            if(setIP)
                clnt = new TripClient(clientIPAddress);
            else
                clnt = new TripClient();
        }
    }

    return client.exec();
}

void usage()
{
    printf(">> Network Transfer Client by Allow (AllowL@mail.ru) <<\n");

    printf("\n Usage options:\n");
    printf("  -i <ip>        - ip address client\n");
    printf("  -p <port>      - transfer client port\n");
    printf("  -u             - use UDP-protocol for transfer data\n");
    printf("  -h             - show hint (help)\n");
}

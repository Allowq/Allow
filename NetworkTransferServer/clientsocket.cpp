#include "clientsocket.h"


ClientSocket::ClientSocket(QObject *parent, const SocketType socketType, const qint64 dataSize,
                           const QString ipAddress, const quint16 port) :
    QAbstractSocket(socketType, parent), m_dataSize(dataSize)
{
    connect(this, SIGNAL(log(QString)), SLOT(logMessage(QString)));
    if(this->socketType() == QAbstractSocket::TcpSocket)
    {
        connect(this, SIGNAL(readyRead()), SLOT(readClient()));
        emit log(tr("Waiting commands from client to start data transmission."));
    }
    else
    {
        m_ipAddress = ipAddress;
        m_port = port;
        UDPSocket = new QUdpSocket(this);
        if(m_ipAddress.contains("localhost"))
        {
            if( !UDPSocket->bind(QHostAddress::LocalHost, m_port, QUdpSocket::ShareAddress))
            {
                emit log(">> Failed to bind address or port for receive datagrams from client.");
                exit(2);
            }
        }
        else
        {
            if( !UDPSocket->bind(QHostAddress(m_ipAddress), m_port, QUdpSocket::ShareAddress))
            {
                emit log(">> Failed to bind address or port for receive datagrams from client.");
                exit(2);
            }
        }
        emit log(tr("Waiting commands from client to start data transmission."));
        connect(UDPSocket, SIGNAL(readyRead()), SLOT(processPendingDatagrams()));
    }
}

void ClientSocket::processPendingDatagrams()
{
    QByteArray datagram;
    datagram.resize(UDPSocket->pendingDatagramSize());
    UDPSocket->readDatagram(datagram.data(), datagram.size());

    QDataStream in(&datagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);

    QString cmd;
    in >> cmd;



    if( cmd == "get" )
        this->sendDataToClientUDP();
    else
    {
        int timeOfTransmission = cmd.toInt();
        emit log(tr("Transfer ends. Data capacity = %1").arg((m_dataSize * 1024)));
        emit log(tr("Upload speed = %1 Mbit/s (Time of transmission - %2 s)")
                     .arg(m_dataSize * 1024 * 1024 * 8 / timeOfTransmission * 0.001)
                     .arg(float(timeOfTransmission) * 0.001f));
    }
}

void ClientSocket::readClient()
{
    quint16 size;

    QDataStream in( this );
    in.setVersion( QDataStream::Qt_4_5 );
    if( (uint)bytesAvailable() < sizeof( quint16 ) )
    {
        emit log("\n>> Failed to receive command from client.");
        exit(3);
    }

    in >> size;

    if( (int)bytesAvailable() < size )
    {
        emit log("\n>> Failed to receive command from client.");
        exit(3);
    }

    QString cmd;
    in >> cmd;
    if( cmd == "get" )
        this->sendDataToClient();
    else
    {
        int timeOfTransmission = cmd.toInt();
        emit log(tr("Transfer ends. Data capacity = %1byte").arg((m_dataSize * 1024 * 1024)));
        emit log(tr("Upload speed = %1 Mbit/s (Time of transmission - %2 s)")
                     .arg(m_dataSize * 1024 * 1024 * 8 / timeOfTransmission * 0.001)
                     .arg(float(timeOfTransmission) * 0.001f));
    }
}

void ClientSocket::sendDataToClientUDP()
{
    const qint64 chunkSize = 8 * 1024;

    qint64 dataSize = m_dataSize * 1024 * 1024;

    char *data = new char[dataSize];

    int pos = 0;
    int step;

    // Отправляем размер файла
    emit log(tr("\nStart sending data to client. Size of data - %1 byte").arg(dataSize));

    QByteArray datagram;
    QDataStream out( &datagram, QIODevice::WriteOnly );
    out.setVersion( QDataStream::Qt_4_7 );
    out << dataSize;
    emit log("\nSending command to start data transmission, waiting.");

    UDPSocket->writeDatagram(datagram, QHostAddress(m_ipAddress), m_port+1);
    while( pos < dataSize )
    {
        int blockSize = qMin( chunkSize, dataSize - pos );
        QByteArray buffer(data,blockSize);
        QByteArray sendData;
        QDataStream outData( &sendData, QIODevice::WriteOnly);
        outData.setVersion(QDataStream::Qt_4_7);
        outData << buffer.data();
        if( buffer.size() != blockSize )
        {
            emit log("\n>> Error reading buffer.");
            exit(4);
        }

        step = 0;
        while(UDPSocket->writeDatagram(sendData, QHostAddress(m_ipAddress), m_port+1) == -1)
        {
            step++;
            if(step == 10)
            {
                emit log("\n >> Failed send datagram to client.");
                emit log(tr("Error - %1").arg(UDPSocket->errorString()));
                exit(5);
            }
        }

        pos += blockSize;

        QCoreApplication::processEvents();
    }

    free(data);
}

void ClientSocket::sendDataToClient()
{
    const qint64 chunkSize = 1024 * 1024 * 12.5;        //12,5Mbyte == 100Mbit
    const qint64 _100Mbyte = 100 * 1024 * 1024;                 //100Mbyte
    qint64 dataSize = m_dataSize * 1024 * 1024;

    qint64 sendSize = 0;
    int pos = 0;

    // Отправляем размер файла
    emit log(tr("\nStart sending data to client. Size of data - %1byte").arg(dataSize));
    QDataStream out( this );
    out.setVersion( QDataStream::Qt_4_5 );
    out << dataSize;

    if( this->write( (const char*)&dataSize, sizeof( dataSize ) ) != sizeof( dataSize ) )
    {
        emit log("\n>> Failed to send size of data to client.");
        exit(6);
    }

    int numParts = 0;
    if((m_dataSize % 100) > 0)
        numParts++;
    numParts += (float)m_dataSize / 100;
    char *data;
    int writed;

    while(numParts)
    {
        pos = 0;
        sendSize = qMin(_100Mbyte,dataSize);
        (dataSize - _100Mbyte) >= 0 ? dataSize -= _100Mbyte : dataSize = 0;
        data = (char*)malloc(sendSize);
        while( pos < sendSize )
        {
            int blockSize = qMin( chunkSize, sendSize - pos );
            QByteArray buf(data,blockSize);

            if( buf.size() != blockSize )
            {
                emit log("\n>> Error reading buffer.");
                exit(7);
            }

            try
            {
                writed = this->write( buf );
            }
            catch(std::bad_alloc& ba)
            {
                emit log(tr("\n>> Error: Out of memory (%1).").arg(ba.what()));
                exit(12);
            }

            if( writed == -1 )
            {
                emit log("\n>> Failed sendind part of data to client.");
                exit(8);
            }

            pos += blockSize;

            QCoreApplication::processEvents();
        }
        free(data);
        numParts--;
    }
}

void ClientSocket::logMessage(const QString &msg)
{
    QTextStream out(stdout);
    out << "\n" << msg;
}

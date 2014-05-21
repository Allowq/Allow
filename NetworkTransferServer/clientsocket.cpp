#include "clientsocket.h"


ClientSocket::ClientSocket(QObject *parent, const SocketType socketType, const qint64 dataSize,
                           const QString ipAddress, const quint16 port) :
    QAbstractSocket(socketType, parent), m_dataSize(dataSize)
{
    if(this->socketType() == QAbstractSocket::TcpSocket)
    {
        connect(this, SIGNAL(readyRead()), SLOT(readClient()));
        connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                SLOT(transferEnd(QAbstractSocket::SocketState)));
    }
    else
    {
        m_ipAddress = ipAddress;
        m_port = port;
        UDPSocket = new QUdpSocket(this);
        if( !UDPSocket->bind(QHostAddress(m_ipAddress), m_port, QUdpSocket::ShareAddress))
            qDebug() << "Failed to bind port";
        else
        {
            connect(UDPSocket, SIGNAL(readyRead()), SLOT(processPendingDatagrams()));
            connect(UDPSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                SLOT(transferEnd(QAbstractSocket::SocketState)));
        }
    }

    connect(this, SIGNAL(log(QString)), SLOT(logMessage(QString)));
}

void ClientSocket::processPendingDatagrams()
{
    emit log(tr("Waiting commands from client to start data transmission."));

    QByteArray datagram;
    do
    {
        datagram.resize(UDPSocket->pendingDatagramSize());
        UDPSocket->readDatagram(datagram.data(), datagram.size());
    }while(UDPSocket->hasPendingDatagrams());

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

    emit log(tr("Waiting commands from client to start data transmission."));
    QDataStream in( this );
    in.setVersion( QDataStream::Qt_4_5 );
    if( (uint)bytesAvailable() < sizeof( quint16 ) )
    {
        system("cls");
        this->disconnect();
        emit log("Please try again start to transfer data.");
        return;
    }

    in >> size;

    if( (int)bytesAvailable() < size )
    {
        system("cls");
        this->disconnect();
        emit log("Please try again start to transfer data.");
        return;
    }

    QString cmd;
    in >> cmd;
    if( cmd == "get" )
        this->sendDataToClient();
    else
    {
        int timeOfTransmission = cmd.toInt();
        emit log(tr("Transfer ends. Data capacity = %1").arg((m_dataSize * 1024)));
        emit log(tr("Upload speed = %1 Mbit/s (Time of transmission - %2 s)")
                     .arg(m_dataSize * 1024 * 1024 * 8 / timeOfTransmission * 0.001)
                     .arg(float(timeOfTransmission) * 0.001f));
    }
}

void ClientSocket::transferEnd(QAbstractSocket::SocketState state)
{
    if(state == 0)
    {
        system("cls");
        emit log("Please try again start to transfer data.");
        this->disconnect();
    }
}

void ClientSocket::sendDataToClientUDP()
{
    const qint64 chunkSize = 8 * 1024; //1024 * 1024 * 12.5;

    const qint64 dataSize = m_dataSize * 1024 * 1024;
    char *data = new char[dataSize];

    int pos = 0;

    // Отправляем размер файла
    emit log(tr("\nStart sending data to client. Size of data - %1 byte\n\n").arg(dataSize));

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
        out << buffer.data();
        if( buffer.size() != blockSize )
        {
            emit log("Error read buffer.");
            getchar();
            system("cls");
            emit log("Please try again start to transfer data.");
            return;
        }
        int writed = UDPSocket->writeDatagram(datagram, QHostAddress(m_ipAddress), m_port+1);
        if( writed == -1 )
        {
            system("cls");
            emit log("Error sending data to client.");
            emit log("Please try again start to transfer data.");
            return;
        }
    //      qDebug() << "Write size:" << writed;

        pos += blockSize;

        QCoreApplication::processEvents();
    }
    free(data);
}

void ClientSocket::sendDataToClient()
{
    const qint64 chunkSize = 1024 * 1024 * 12.5;

    const qint64 dataSize = m_dataSize * 1024 * 1024;
    char *data = new char[dataSize];

    int pos = 0;

    // Отправляем размер файла
    emit log(tr("\nStart sending data to client. Size of data - %1 byte\n\n").arg(dataSize));
    QDataStream out( this );
    out.setVersion( QDataStream::Qt_4_5 );
    out << dataSize;

    if( this->write( (const char*)&dataSize, sizeof( dataSize ) ) != sizeof( dataSize ) )
    {
        system("cls");
        emit log("Error write size of data.");
        emit log("Please try again start to transfer data.");
        return;
    }

    while( pos < dataSize )
    {
        int blockSize = qMin( chunkSize, dataSize - pos );
        QByteArray buf(data,blockSize);
        if( buf.size() != blockSize )
        {
            system("cls");
            emit log("Error read buffer.");
            emit log("Please try again start to transfer data.");
            return;
        }
        int writed = this->write( buf );
        if( writed == -1 )
        {
            system("cls");
            emit log("Error sending data to client.");
            emit log("Please try again start to transfer data.");
            return;
        }
    //      qDebug() << "Write size:" << writed;

        pos += blockSize;

        QCoreApplication::processEvents();
    }
    free(data);
}

void ClientSocket::logMessage(const QString &msg)
{
    QTextStream out(stdout);
    out << "\n" << msg;
}

#include "tripclient.h"

TripClient::TripClient(const QString ipAddress, const quint16 port, const SocketType socketType) :
    QAbstractSocket(socketType, NULL), m_ipAddress(ipAddress), m_port(port)
{
    this->showStartupOptions();
    connect(this,SIGNAL(log(QString)),SLOT(logMessage(QString)));

    if(this->socketType() == QAbstractSocket::TcpSocket)
    {
    //    connect(this,SIGNAL(disconnected()),SLOT(deleteLater()));
        if(m_ipAddress.contains("localhost"))
            this->connectToHost(QHostAddress::LocalHost, m_port);
        else
            this->connectToHost(QHostAddress(m_ipAddress), m_port);
        if( !this->waitForConnected( 1000 ) )
        {
            emit log("\n>> Failed connect to server.");
            exit(9);
        }
        else
        {
            emit log("Client has connected to the server.");
            this->receiveData();
        }
    }
    else
    {
        UDPSocket = new QUdpSocket(this);
        UDPSocket->bind(QHostAddress(m_ipAddress),m_port+1,QUdpSocket::ShareAddress);
        this->sendUDPCommandGET();
        connect(UDPSocket,SIGNAL(readyRead()),SLOT(incomingConnection()));
    }
}

void TripClient::incomingConnection()
{
    emit log("Client has connected to the server.");
    QByteArray baDatagram;
    baDatagram.resize(UDPSocket->pendingDatagramSize());
    UDPSocket->readDatagram(baDatagram.data(), baDatagram.size());

    QDataStream in(&baDatagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);
    in >> fileSize;
    disconnect(UDPSocket,SIGNAL(readyRead()),this,SLOT(incomingConnection()));
    connect(UDPSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    emit log("\nReceiving data from server, wating.");

    pos = 0;
    step = 0;
    transferStartTime = QTime::currentTime();
}

void TripClient::processPendingDatagrams()
{
    QByteArray buffer;
    QByteArray datagram;
    datagram.resize(UDPSocket->pendingDatagramSize());
    UDPSocket->readDatagram(datagram.data(), datagram.size());

    QDataStream in(&datagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);
    in >> buffer;
    pos += buffer.size();

    int timeOfTransmition = transferStartTime.msecsTo(QTime::currentTime()) - 3000*step;
    this->sendUDPCommandTime(timeOfTransmition);
    system("cls");
    emit log(tr("Transfer ends. Client has received - %1 Mbyte").arg(pos / 1024 / 1024));
    log(tr("\nDownload speed = %1 Mbit/s (Time of transmission - %2 s)")
        .arg(pos * 8 / timeOfTransmition * 0.001)
        .arg(float(timeOfTransmition) * 0.001f));
    emit log("\n\nWould you like to start again this procedure? Push Ctrl+C to exit.");
    //QTimer::singleShot(5000,this,SLOT(incomingConnection()));
}

void TripClient::receiveData()
{
    this->sendCommandGET();

    // Читаем файл
    QDataStream in( this );
    in.setVersion( QDataStream::Qt_4_5 );

    qint64 fileSize;
    do
        if( !this->waitForReadyRead() )
        {
            emit log("\n>> Failed receive size of data from server.");
            exit(10);
        }
    while( this->bytesAvailable() < (int)sizeof( fileSize ) );

    in >> fileSize;

    emit log("Receiving data from server, wating.");

    qint64 pos = 0;
    QTime transferStartTime;
    int timeOfTransmition = 0;
    QByteArray buf;
    transferStartTime = QTime::currentTime();
    while( pos < fileSize )
    {
        if( !this->waitForReadyRead() )
        {
            emit log("\n>> Error. Timeout read data.");
            exit(11);
        }

        buf = this->readAll();
        if( !buf.size() )
            continue;

        pos += buf.size();
    }
    timeOfTransmition = transferStartTime.msecsTo(QTime::currentTime());
    this->sendCommandTime(timeOfTransmition);
    system("cls");
    emit log(tr("Transfer ends. Client has received - %1 Mbyte").arg(fileSize / 1024 / 1024));
    log(tr("Download speed = %1 Mbit/s (Time of transmission - %2 s)")
        .arg(pos * 8 / timeOfTransmition * 0.001)
        .arg(float(timeOfTransmition) * 0.001f));
    emit log("\n\nWould you like to start again this procedure? Push Ctrl+C to exit.");
    QTimer::singleShot(5000,this,SLOT(receiveData()));
}

void TripClient::sendUDPCommandTime(int timeOfTransmission)
{
    QByteArray buffer;
    QDataStream out( &buffer, QIODevice::WriteOnly );
    out.setVersion( QDataStream::Qt_4_7 );
    out << QString::number(timeOfTransmission);
    UDPSocket->writeDatagram(buffer, QHostAddress(m_ipAddress), m_port);
}

void TripClient::sendCommandTime(int timeOfTransmission)
{
    QByteArray buffer;
    QDataStream out( &buffer, QIODevice::WriteOnly );
    out.setVersion( QDataStream::Qt_4_5 );
    out << (quint16)0;
    out << QString::number(timeOfTransmission);
    out.device()->seek( 0 );
    out << (quint16)( buffer.size() - sizeof(quint16));
    this->write( buffer );
    this->flush();
}

void TripClient::sendUDPCommandGET()
{
    QString cmd = "get";
    QByteArray buf;
    QDataStream out( &buf, QIODevice::WriteOnly );
    out.setVersion( QDataStream::Qt_4_7 );
    out << cmd;
    emit log("\nSending command to start data transmission, waiting.");

    UDPSocket->writeDatagram(buf, QHostAddress(m_ipAddress), m_port);
}

void TripClient::sendCommandGET()
{
    QString cmd = "get";
    QByteArray buf;
    QDataStream out( &buf, QIODevice::WriteOnly );
    out.setVersion( QDataStream::Qt_4_5 );

    out << (quint16)0;
    out << cmd;
    out.device()->seek( 0 );
    out << (quint16)( buf.size() - sizeof(quint16) );

    emit log("\nSending command to start data transmission, waiting.");
    this->write( buf );
    this->flush();
}

void TripClient::showStartupOptions()
{
    QTextStream out(stdout);
    out << "    Client startup options\n"
        << "Address of connection to server - "
        << m_ipAddress << ":" << m_port;
    if(this->socketType() == TcpSocket)
        out << " (use TCP-socket)";
    else
        out << " (use UDP-socket)";
    out.flush();
}

void TripClient::logMessage(const QString &msg)
{
    QTextStream out(stdout);
    out << "\n" << msg;
}

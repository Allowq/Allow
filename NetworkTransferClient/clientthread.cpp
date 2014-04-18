#include "clientthread.h"

ClientThread::ClientThread(const QString& serverAddress, const quint16& serverPort, const qint64 &dataSize)
: m_serverAddress(serverAddress), m_serverPort(serverPort), m_sendSize(dataSize)
{
}

void ClientThread::run()
{
    QTcpSocket socket;
    QTime transferStartTime;
    qint64 timeOfTransmition = 0;

    /*
    QTime transferStartTime;
    const qint64 dataSize = m_sendSize*1024*1024; //100Mbyte
    qint64 dataCapacity = 0;
    QTimer timer;
    timer.setInterval(10000);
    timer.start(10000);
    qint64 timeOfTransmition = 0;
    qint64 bufferSize = 65535;
    char* data = new char[dataSize];
    */

    socket.connectToHost(m_serverAddress, m_serverPort);
//    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    if( !socket.waitForConnected(60000) )
    {
        log(tr("Failed to connect!"));
        return;
    }
    log(tr("Client has connected to the server"));

    while(true)
    {
        // client receives data

        sendCMDtoStartTransfer(&socket);
        // Читаем файл
        QDataStream in( &socket );
        in.setVersion( QDataStream::Qt_4_5 );

        qint64 fileSize;
        do
            if( !socket.waitForReadyRead() )
            {
                qDebug() << "Timeout read data";
                return;
            }
        while( socket.bytesAvailable() < (int)sizeof( fileSize ) );

        in >> fileSize;
        qDebug() << fileSize;

        int pos = 0;
        int total = 0;
        transferStartTime = QTime::currentTime();
        while( pos < fileSize )
        {
            if( !socket.waitForReadyRead() )
            {
                emit log("Timeout read data.");
                socket.disconnect();
                emit log("Please try again start to transfer data.");
                return;
            }

            QByteArray buf = socket.readAll();
            if( !buf.size() )
                continue;

            total += buf.size();
            qDebug() << total;
            pos += buf.size();
        }
        timeOfTransmition = transferStartTime.msecsTo(QTime::currentTime());
        QByteArray arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_5);
        out << quint16(0) << timeOfTransmition;
        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));
        socket.write(arrBlock);
        emit log("Transfer ends");
        log(tr("Download speed = %1 Mbit/s (Time of transmission - %2 s)")
            .arg(qint64(total) * 8 / timeOfTransmition * 0.001)
            .arg(float(timeOfTransmition) * 0.001f));

        break;
    }
}

void ClientThread::sendCMDtoStartTransfer(QTcpSocket *socket)
{
    QString cmd = "get";

    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << cmd;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    socket->write(arrBlock);

    emit log("\nSending command to start data transmission.");
}

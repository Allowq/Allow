#include "serverthread.h"
#include <QTcpSocket>
#include <QMutex>

ServerThread::ServerThread(int socketDesc, qint64 dataSize)
: m_socketDesc(socketDesc), m_sendSize(dataSize)
{
}

void ServerThread::run()
{
    emit log(tr("\nConnected"));
    emit log(tr("ServerThread object thread = %1").arg((unsigned long)this->thread()));
    emit log(tr("\nServer thread = %1").arg((unsigned long)QThread::currentThread()));

    QTcpSocket socket;
    qint64 timeOfTransmition = 0;
    qint64 transferDataSize = 0;

    socket.setSocketDescriptor(m_socketDesc);

    emit log(tr("Socket thread = %1").arg((unsigned long)socket.thread()));

    /*

    const qint64 dataSize = m_sendSize * 1024 * 1024;
    char *data = new char[dataSize];
    for(qint64 i = 0; i < dataSize; i++)
        data[i] = qrand() & 0xff;
        */
    QMutex waitMutex;
    while(true)
    {
        waitMutex.lock();
        m_transferStarted.wait(&waitMutex);
        waitMutex.unlock();

        emit log(tr("\nWaiting commands from client to start data transmission."));

        if(socket.state() == 0)
        {
            system("cls");
            emit log("Server lost connection to client.");
        }
        else
        {
            receiveCMDtoStartTransfer(&socket);
            transferDataSize = this->sendBigFile(&socket);
            if(transferDataSize == 0)
            {
                socket.disconnect();
                system("cls");
                emit log("Please try again start to transfer data.");
            }
            socket.waitForBytesWritten();
            QDataStream in(&socket);
            in.setVersion(QDataStream::Qt_4_5);
            quint16 m_nNextBlockSize = 0;
            for (;;)
            {
                if (!m_nNextBlockSize)
                {
                    if (socket.bytesAvailable() < sizeof(quint16))
                        break;
                    in >> m_nNextBlockSize;
                }
                if (socket.bytesAvailable() < m_nNextBlockSize)
                    break;
                in >> timeOfTransmition;
                m_nNextBlockSize = 0;
            }
            emit log(tr("Transfer ends. Data capacity = %1").arg(transferDataSize));
            emit log(tr("Upload speed = %1 Mbit/s (Time of transmission - %2 s)")
                .arg(transferDataSize * 8 / timeOfTransmition * 0.001)
                .arg(float(timeOfTransmition) * 0.001f));
            break;
        }
    }
}

void ServerThread::receiveCMDtoStartTransfer(QTcpSocket *socket)
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_5);
    quint16 m_nNextBlockSize = 0;
    QString cmd;
    for(;;)
    {
        if (!m_nNextBlockSize)
        {
            if (socket->bytesAvailable() < sizeof(quint16))
                break;
            in >> m_nNextBlockSize;
        }
        if (socket->bytesAvailable() < m_nNextBlockSize)
            break;
        in >> cmd;
        if( cmd != "get" )
        {
            emit log("The command from client have't been received.");
            socket->disconnect();
            system("cls");
            emit log("Please try again start to transfer data.");
        }
        m_nNextBlockSize = 0;
    }
}

        /*
        // server sends data
        qint64 written = socket.write(data, dataSize);
        while(qint64 toWrite = socket.bytesToWrite())
        {
            emit progressChange((dataSize - toWrite) * 100 / dataSize);

       //     socket.flush();
            if(socket.state() != 0)
                socket.waitForBytesWritten(1000);
            else
                break;
            yieldCurrentThread();
        }
        // server receives data about time to send
        socket.waitForReadyRead();
        if(socket.state() == 0)
        {
            system("cls");
            emit log("Server lost connection to the client.");
        }
        else
        {
            QDataStream in(&socket);
            in.setVersion(QDataStream::Qt_4_5);
            quint16 m_nNextBlockSize = 0;

            for (;;)
            {
                if (!m_nNextBlockSize)
                {
                    if (socket.bytesAvailable() < sizeof(quint16))
                        break;
                    in >> m_nNextBlockSize;
                }
                if (socket.bytesAvailable() < m_nNextBlockSize)
                    break;
                in >> timeOfTransmition;
            }
            emit log(tr("Upload speed = %1 Mbit/s (%2 KBytes for %3 s)")
                .arg(dataSize * 8 / timeOfTransmition * 0.001)
                .arg(float(dataSize / 512)*0.5f)
                .arg(float(timeOfTransmition) * 0.001f));
        }
        */

qint64 ServerThread::sendBigFile(QTcpSocket *socket)
{
    const qint64 chunkSize = 1024 * 12;

    const qint64 dataSize = m_sendSize * 1024 * 1024;
    char *data = new char[dataSize];
    int pos = 0;
    emit log(tr("Start sending data to client. Size of data - %1").arg(dataSize));
    // Отправляем размер файла
    QDataStream out( socket );
    out.setVersion( QDataStream::Qt_4_5 );
    out << dataSize;
    if( socket->write( (const char*)&dataSize, sizeof( dataSize ) ) != sizeof( dataSize ) )
    {
        emit log("Error write size of data.");
    }
    while( pos < dataSize )
    {
        int blockSize = qMin( chunkSize, dataSize - pos );

        QByteArray buf(data,blockSize);

        if( buf.size() != blockSize )
        {
            emit log("Error read file.");
        }

        int writed = socket->write( buf );
        if( writed == -1 )
        {
            emit log("Error write data");
        }

            //qDebug() << "Write size:" << writed;

        pos += blockSize;
        if(pos % 1024 == 0)
        {
            system("cls");
            emit log(tr("%1%").arg(dataSize/pos));
        }

        QCoreApplication::processEvents();
    }
    return dataSize;

    /*
    emit log(tr("Send data to client."));

    QFile file( "D:/Films/Climbing/Hard Wood - Part 1.mp4" );
    if( !file.open( QIODevice::ReadOnly ) )
    {
        emit log(tr("Error send data"));
    }

  //  m_sendSize;

    int pos = 0;
    qint64 fileSize = file.size();
    emit log(tr("Start sending data to client. Size of data - %1").arg(fileSize));

    // Отправляем размер файла
    QDataStream out( socket );
    out.setVersion( QDataStream::Qt_4_5 );
    out << fileSize;

    qDebug() << "Filesize :" << fileSize;

    if( socket->write( (const char*)&fileSize, sizeof( fileSize ) ) != sizeof( fileSize ) )
    {
        emit log("Error write size of data.");
    }

    while( pos < fileSize )
    {
        int blockSize = qMin( chunkSize, fileSize - pos );
        QByteArray buf = file.read( blockSize );

        if( buf.size() != blockSize )
        {
            emit log("Error read file.");
        }

        int writed = socket->write( buf );
        if( writed == -1 )
        {
            emit log("Error write data");
        }
        qDebug() << "Write size:" << writed;

        pos += blockSize;

        QCoreApplication::processEvents();
    }
    return fileSize;
    */
}

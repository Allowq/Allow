#include "clientsocket.h"

ClientSocket::ClientSocket(QObject *parent, qint64 dataSize) :
    QTcpSocket(parent), m_dataSize(dataSize)
{
    connect(this, SIGNAL(readyRead()), SLOT(readClient()));
    connect(this, SIGNAL(disconnected()), SLOT(deleteLater()));
    connect(this, SIGNAL(log(QString)), SLOT(logMessage(QString)));
}

void ClientSocket::readClient()
{
    quint16 size;

    emit log(tr("Waiting commands from client to start data transmission."));
    QDataStream in( this );
    in.setVersion( QDataStream::Qt_4_5 );
    if( (uint)bytesAvailable() < sizeof( quint16 ) )
    {
        this->disconnect();
        system("cls");
        emit log("Please try again start to transfer data.");
        return;
    }

    in >> size;

    if( (int)bytesAvailable() < size )
    {
        this->disconnect();
        system("cls");
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

void ClientSocket::sendDataToClient()
{
    const qint64 chunkSize = 1024 * 12;

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
        emit log("Error write size of data.");
        system("cls");
        emit log("Please try again start to transfer data.");
        return;
    }

    while( pos < dataSize )
    {
        int blockSize = qMin( chunkSize, dataSize - pos );
        QByteArray buf(data,blockSize);
        if( buf.size() != blockSize )
        {
            emit log("Error read buffer.");
            system("cls");
            emit log("Please try again start to transfer data.");
            return;
        }

        int writed = this->write( buf );
        if( writed == -1 )
        {
            emit log("Error sending data to client.");
            system("cls");
            emit log("Please try again start to transfer data.");
            return;
        }
        qDebug() << "Write size:" << writed;

        pos += blockSize;

        QCoreApplication::processEvents();
    }
}

void ClientSocket::logMessage(const QString &msg)
{
    QTextStream out(stdout);
    out << "\n" << msg;
}

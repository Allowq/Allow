#include "tripclient.h"

TripClient::TripClient(const QStringList &startupOptions)
{
    m_ipAddress = "localhost";
    m_port = 1488;
    this->getStartupOptions(startupOptions);
    this->showStartupOptions();
    connect(this,SIGNAL(log(QString)),SLOT(logMessage(QString)));
    connect(this,SIGNAL(disconnected()),SLOT(deleteLater()));
    this->connectToHost(QHostAddress(m_ipAddress), m_port);
    if( !this->waitForConnected( 1000 ) )
         emit log("Failed to connect!");
    emit log("Client has connected to the server.");
    this->receiveData();
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
            emit log("Timeout read data.");
            this->disconnect();
            emit log("Please try again transfer data.");
            return;
        }
    while( this->bytesAvailable() < (int)sizeof( fileSize ) );

    in >> fileSize;

    emit log("\nReceiving data from server, wating.");

    qint64 pos = 0;
    QTime transferStartTime;
    int timeOfTransmition = 0;
    QByteArray buf;
    transferStartTime = QTime::currentTime();
    while( pos < fileSize )
    {
        if( !this->waitForReadyRead() )
        {
            qDebug() << "Timeout read data";
            this->disconnect();
            emit log("Please try again transfer data.");
            return;
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
    log(tr("\nDownload speed = %1 Mbit/s (Time of transmission - %2 s)")
        .arg(pos * 8 / timeOfTransmition * 0.001)
        .arg(float(timeOfTransmition) * 0.001f));
    emit log("\n\nWould you like to start again this procedure? Push Ctrl+C to exit.");
    QTimer::singleShot(5000,this,SLOT(receiveData()));
}

void TripClient::sendCommandTime(int timeOfTramsmission)
{
    QByteArray buffer;
    QDataStream out( &buffer, QIODevice::WriteOnly );
    out.setVersion( QDataStream::Qt_4_5 );
    out << (quint16)0;
    out << QString::number(timeOfTramsmission);
    out.device()->seek( 0 );
    out << (quint16)( buffer.size() - sizeof(quint16));
    this->write( buffer );
    this->flush();
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

void TripClient::getStartupOptions(const QStringList &startupOptions)
{
    if(startupOptions.size() > 1)
    {
        if(startupOptions.contains("set"))
        {
            m_ipAddress.clear();
            m_ipAddress.append(startupOptions.at(2));
        }
        if(startupOptions.size() > 3)
        {
            bool ok;
            quint16 tPort = startupOptions.at(3).toUInt(&ok,10);
            if(ok)
                m_port = tPort;
        }
    }
}

void TripClient::showStartupOptions()
{
    QTextStream out(stdout);
    out << "    Client startup options\n"
        << "Address of the connection to the server - "
        << m_ipAddress << ":" << m_port;
    out.flush();
}

void TripClient::logMessage(const QString &msg)
{
    QTextStream out(stdout);
    out << "\n" << msg;
}

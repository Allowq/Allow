#include "tripserver.h"

TripServer::TripServer(const QStringList &startupOptions)
{
    m_ipAddress = "localhost";
    m_port = 1488;
    m_dataSize = 100;
    this->getStartupOptions(startupOptions);
    this->showStartupOptions();
    if( !this->listen( QHostAddress(m_ipAddress), m_port ) )
        qDebug() << "Failed to bind port";
}

void TripServer::incomingConnection(int socketID)
{
    ClientSocket *socket = new ClientSocket(this, m_dataSize);
    socket->setSocketDescriptor(socketID);
}

void TripServer::getStartupOptions(const QStringList &startupOptions)
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
        if(startupOptions.size() > 4)
        {
            bool ok;
            quint64 tSize = startupOptions.at(4).toUInt(&ok,10);
            if(ok)
                m_dataSize = tSize;
        }
    }
}

void TripServer::showStartupOptions()
{
    QTextStream out(stdout);
    out << "    Server startup options\n"
        << "The server is running at "
        << m_ipAddress << ":" << m_port << "\n"
        << "Server sends - " << m_dataSize << "Mbyte\n";
    out.flush();
}

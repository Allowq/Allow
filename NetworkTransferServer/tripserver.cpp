#include "tripserver.h"

TripServer::TripServer(const QString ipAddress, const quint16 port,
                       const qint64 dataSize, const bool applyUDP) :

    m_ipAddress(ipAddress), m_port(port), m_dataSize(dataSize), m_applyUDP(applyUDP)
{
    this->showStartupOptions();
    if(m_applyUDP)
    {
        ClientSocket *socket = new ClientSocket(
                    this,
                    QAbstractSocket::UdpSocket,
                    m_dataSize,
                    m_ipAddress,
                    m_port);
    }
    else
        if( !this->listen(QHostAddress(m_ipAddress), m_port) )
            qDebug() << "Failed to bind port";
}

void TripServer::incomingConnection(int socketID)
{
    ClientSocket *socket = new ClientSocket(this, QAbstractSocket::TcpSocket, m_dataSize);
    socket->setSocketDescriptor(socketID);
}

void TripServer::showStartupOptions()
{
    QTextStream out(stdout);
    out << "    Server startup options\n"
        << "The server is running at "
        << m_ipAddress << ":" << m_port;
    if(m_applyUDP)
        out << " (use UDP-socket)\n";
    else
        out << " (use TCP-socket)\n";
    out << "Server sends - " << m_dataSize << "Mbyte\n";
    out.flush();
}

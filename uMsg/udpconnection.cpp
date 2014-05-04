    myLocalIpAdr = new QHostAddress(myLocalIp);
}

QString UdpConnection::ip()
{
    return myLocalIp;
}

/**
 * Sets the broadcast adr by adding 255 to the end of the localAdr
 *
 * @brief UdpConnection::setBroadcastAdr
 * @param localIp the local IP to convert to the broadcast address
 */
void UdpConnection::setBroadcastAdr(QString localIp)
{
    QRegularExpression re("(\\d+\\.\\d+\\.(\\d+)\\.)(\\d+)");
    QRegularExpressionMatch match = re.match(localIp);
    if (match.hasMatch())
    {
        localBcast = match.captured(1) + "255";
        broadcastAdr = new QHostAddress(localBcast);
    }
    else
    {
        throw "Local ip has invalid format";
    }
}

void UdpConnection::broadcastMessage(const QString &msg)
{
    QByteArray d = msg.toLocal8Bit();
    udpSocket->writeDatagram(d.data(), d.size(), *broadcastAdr, uPort);
}

void UdpConnection::sendMessage(const QString &msg, const QString &ip)
{
    QHostAddress adr(ip);
    QByteArray d = msg.toLocal8Bit();
    udpSocket->writeDatagram(d.data(), d.length(), adr, uPort);
}

/**
 * Calls the readIncomingMessage method in the Controller class and passes
 * the datagram provided by the readPendingDatagrams SLOT.
 *
 * @brief UdpConnection::processTheDatagram
 * @param datagram the datagram to be processed
 */
void UdpConnection::processTheDatagram(QByteArray &datagram)
{
    controller->readIncomingMessage(QString::fromLocal8Bit(datagram));
}

/**
 * SLOT which is called when the UDP-socket recieves packets.
 * Checks if the sender address is blocked and returns if so.
 *
 * @brief UdpConnection::readPendingDatagrams
 */
void UdpConnection::readPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

       if(!controller->isBlocked(sender.toString())) processTheDatagram(datagram);
    }
}

UdpConnection::~UdpConnection()
{
    delete myLocalIpAdr;
    delete broadcastAdr;
    udpSocket->close();
    delete udpSocket;
}

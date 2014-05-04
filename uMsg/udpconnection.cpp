#include "udpconnection.h"
#include "controller.h"

UdpConnection* UdpConnection::_me = NULL;

const QString GOOGLE_DNS = "8.8.8.8";
const int GOOGLE_DNS_PORT = 56;
const int GOOGLE_DNS_TIMEOUT = 1000;

/**
 * Private custructor; part of the Singleton pattern.
 *
 * @brief UdpConnection::UdpConnection
 * @param parent
 * @param _cont pointer to the Controller class which facilitates communication with the GUI
 */
UdpConnection::UdpConnection(QObject *parent, Controller* _cont) : QObject(parent), uPort(7755)
{
    controller = _cont;
    initSocket();
}

/**
 * Public getter method for the private me-pointer.
 *
 * @brief UdpConnection::getSingleton
 * @param parent
 * @param _cont pointer to the Controller class which facilitates communication with the GUI
 * @return pointer to this singleton class
 */
UdpConnection* UdpConnection::getSingleton(QObject *parent, Controller* _cont)
{
    if(UdpConnection::_me == NULL)
    {
        UdpConnection::_me = new UdpConnection(parent, _cont);
    }
    return UdpConnection::_me;
}

/**
 * Initializes the socket and sets the needed IPs. Connects the readyREad SIGNAL to the
 * readPendingDiagrams SLOT. This method is called by the custructor.
 *
 * @brief UdpConnection::initSocket
 */
void UdpConnection::initSocket()
{
    udpSocket = new QUdpSocket(this);
    setMyLocalIp();
    setBroadcastAdr(myLocalIp);

    udpSocket->bind(QHostAddress::Any, uPort);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

/**
 * Sets the pointers myLocalIp and myLocalIpAdr by querying Google dns, or if the query times out,
 * gets the local IPs provided byt the system, and selects the first one in this list. This is almost
 * always the right address (tested on multiple computers with 100% success). If you do NOT have a valid internet
 * connection AND the chosen IP is the wrong one; the program will not be able to send messages on the LAN.
 *
 * @brief UdpConnection::setMyLocalIp
 */
void UdpConnection::setMyLocalIp()
{
    QStringList ips;

    QTcpSocket socket;
    socket.connectToHost(GOOGLE_DNS, GOOGLE_DNS_PORT);
    if (socket.waitForConnected(GOOGLE_DNS_TIMEOUT))
    {
        ips.append(socket.localAddress().toString());
    }
    else
    {
        foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
        {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
                ips.append(address.toString());
        }
    }

    if(ips.length() == 0) throw "No local ip detected";

    myLocalIp = ips[0];
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

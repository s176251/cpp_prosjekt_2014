#ifndef UDPCONNECTION_H
#define UDPCONNECTION_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QRegularExpression>
#include "connection.h"

class Controller;

/**
 * Singleton class which controls the application's UDP-socket, and the flow of packets thrgough this resource.
 * Provides the application with the computers local and external IP-addresse
 *
 * @author Jonas Moltzau & Martin W. Løkkeberg
 *
 * @brief The UdpConnection class
 */
class UdpConnection : public QObject, public Connection
{
    Q_OBJECT
    //Pointer to the Controller class which facilitates communication with the GUI
    Controller* controller;
    //The UDP-Socket instance for sending packets locally and on the internet
    QUdpSocket* udpSocket;
    //Local IP and Brodcast address pointers
    QHostAddress *broadcastAdr, *myLocalIpAdr;
    //Local IP and Breadcasr address as strings for conveniance as apposed to calling .ToString on the QHostAddress objects
    QString myLocalIp, localBcast;

    //Static pointer to the Singleton class itself; part of the Singleton Pattern
    static UdpConnection* _me;
    //uMSG Default Port
    const int uPort;

    //Sets the local IP adress by either querying Google DNS or choosing on of local IP's provided by the system
    void setMyLocalIp();
    //Sets the broadcast adr by adding 255 to the end of the localAdr
    void setBroadcastAdr(QString localIp);
    //Initializes the socket. Sets all needed IPs. This method is called by the cunstructor.
    void initSocket();
    //Method for processing the datagrams provided by the readPendingDatagrams SLOT
    void processTheDatagram(QByteArray &datagram);
    //Cunstructor
    explicit UdpConnection(QObject *parent = 0, Controller *_cont = 0);

public:
    //Sends the incoming msg to the broadcast address
    void broadcastMessage(const QString &msg);
    //Sends the incoming msg to the provided IP-address
    void sendMessage(const QString &msg, const QString &ip);
    //Getter method for myLocalIP
    QString ip();
    //Getter for the singleton pointer: me
    static UdpConnection *getSingleton(QObject *parent, Controller *_cont);
    //Destructor
    ~UdpConnection();

signals:

public slots:
    //Is called when the UDP-socket recieves packets
    void readPendingDatagrams();
};

#endif // UDPCONNECTION_H

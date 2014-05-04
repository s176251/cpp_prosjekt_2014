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
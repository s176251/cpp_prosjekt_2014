#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "udpconnection.h"
#include <QNetworkReply>
#include <QSettings>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QMessageBox>

class MainWindow;
class Conversation;
class Peer;

/**
 * Intermediate business logic class for communication between the MainWindow and UdpConnection
 *
 * @author Jonas Moltzau & Martin W. Løkkeberg
 *
 * @brief The Controller class
 */
class Controller : public QObject
{
    Q_OBJECT
    MainWindow* window;
    Connection* conn;
    //List of Conversations with indecies corresponding to the tabs in Mainwindow (-1 because of the ALL LAN tab)
    QList<Conversation*> *convos;
    //Peer objects containing the computers information
    Peer *myInfo, *myExtInfo;
    //The path to the QSettings file
    QString settingsFile;
    //The contact list (saved in QSettings) and block list (only in session)
    QStringList* extContacts, *blockList;
    QNetworkAccessManager* accessManager;

    //Initiates the query for the external ip. Connects the SLOT setExternalIP for the answer.
    //Also adds the local ip to the window title.
    void getIp();
    //Loads the applikations saved settings from file
    void loadSettings();

public:
    //Constructor
    explicit Controller(MainWindow *parent = 0);
    //Processes the incoming messages and calls the appropriate methods for
    //detail processing depending on the flags within the message
    void readIncomingMessage(QString str);
    //Uses UdpConnection to send messages to all participants in the paramater cdata.
    void sendConvoMessage(Conversation *cdata, QString txtMsg);
    //Constructs a logoff message and uses UdpConnection to send it
    void logOff();
    //Notifies all Peers within the paramater convo that this instance has added a new member to the conversation.
    //Also sends all the participants' IPs to the newly added PEER.
    void notifyMembersChanged(Conversation *convo, QString sel);
    //Uses UdpHelper to send a broadcasted message

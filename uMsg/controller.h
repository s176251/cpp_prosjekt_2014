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
    void sendBroadcastMsg(QString msg);
    //Sends a message to all Peers whitin the parameter convo containing the flag FLAG_LEAVE_CONVO,
    //so that this Peer can be stricken from the Peers' conversations lists.
    void leaveConversation(Conversation *convo);
    //Getter for the list of Conversations
    QList<Conversation *> *getConvos();
    //Getter for external and local ip switched on the bool external
    QString &myIp(bool external = false);
    //Getter for the nickname of this instance
    QString &myName();
    //Getter for this instances Peer
    Peer *me();
    //Sets the name variable within Peer *myInfo to the newName paramater
    void setMyName(const QString &newName);
    //Saves the settings for this instance to file
    void saveSettings();
    //Destructor
    ~Controller();
    //Adds the paramater contact as a new external contact in the QStringList* extContacts list.
    void addExternalContact(const QString &contact);
    //Getter for QStringList* extContacts
    QStringList getSavedContacts();
    //Removes an entry corresponding to the paramater toRemove from QStringList* extContacts
    bool removeExtContact(const QString &toRemove);
    //Removes an entry corresponding to the paramater toUnblock from QStringList* blockList
    void unblockContact(const QString &toUnblock);
    //Adds the paramater contact as a new blocked contact in the QStringList* blockList list.
    void blockContact(const QString &toBlock);
    //Finds the entry corresponding to the paramater ip in the QStringList* blockList list, returns true if found, false if not
    bool isBlocked(const QString &ip);
    //Checks if a given ip is external
    bool isExternal(const QString &ip);
signals:

public slots:
    //Uses UdpHelper to broadcast a message with the flag FLAG_LAN_SCAN to the local bcast adr
    void scanLan();

private slots:
    //SLOT for the getIp() method. Called when program gets a reply from the getIp() request.
    void setExternalIp(QNetworkReply *reply);
};

#endif // CONTROLLER_H

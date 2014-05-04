#include "controller.h"
#include "mainwindow.h"
#include "conversation.h"

//Flags for sending system messages between clients
const QString FLAG_LOGGING_OFF = "0";
const QString FLAG_LAN_SCAN = "1";
const QString FLAG_LAN_MSG = "2";
const QString FLAG_LAN_SCAN_ANSWER = "3";
const QString FLAG_PRIVATE_CHAT = "4";
const QString FLAG_ADD_TO_PRIVATE_CHAT = "5";
const QString FLAG_ADDED_TO_PRIVATE_CHAT = "6";
const QString FLAG_BCAST_MSG = "7";
const QString FLAG_LEAVE_CONVO = "8";

//Const variable keys for storing data in QSettings
const QString NICKNAME = "nickname";
const QString CONTACTS = "contacts";

const QString EXTERNAL_IP_URL = "http://myexternalip.com/raw";

/**
 * Constructor.
 *
 * @brief Controller::Controller
 * @param _window pointer to the MainWindow class
 */
Controller::Controller(MainWindow *_window) : QObject(_window), myExtInfo(0)
{
    window = _window;
    blockList = new QStringList;
    conn = UdpConnection::getSingleton(_window, this); //new UdpConnection(_window, this);
    convos = new QList<Conversation*>;
    myInfo = new Peer(conn->ip(), QDir::home().dirName());
    settingsFile = QString(QApplication::applicationDirPath()) + "/uMsg.ini";
    extContacts = new QStringList;
    loadSettings();
    getIp();
}

bool Controller::isExternal(const QString& ip )
{
    QRegularExpression isInternal("(^127\\.)|(^192\\.168\\.)|(^10\\.)|(^172\\.1[6-9]\\.)|(^172\\.2[0-9]\\.)|(^172\\.3[0-1]\\.)");
    QRegularExpressionMatch match = isInternal.match(ip);
    return !match.hasMatch();
}

QStringList Controller::getSavedContacts()
{
    return *extContacts;
}

/**
 * Finds the entry corresponding to the paramater ip in the QStringList* blockList list
 * @brief Controller::isBlocked
 * @param ip the entry to find
 * @return true if found, false if not
 */
bool Controller::isBlocked(const QString& ip)
{
    return blockList->contains(ip);
}

/**
 * Initiates the query to EXTERNAL_IP_URL for the external ip. Connects the SLOT setExternalIP for the answer.
 * Also adds the local ip to the window title
 *
 * @brief Controller::getIp
 */
void Controller::getIp()
{
    window->setWindowTitle("uMsg - " + myIp());
    accessManager = new QNetworkAccessManager(window);
    accessManager->get(QNetworkRequest(QString(EXTERNAL_IP_URL)));
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(setExternalIp(QNetworkReply*)));
}

/**
 * SLOT for the getIp() method. Called when program gets a reply from the getIp() request.
 * Displays a QMessageBox to the user if the program does not have an active internet connection.
 *
 * @brief Controller::setExternalIp
 * @param reply the reply from the query sent by getIp()
 */
void Controller::setExternalIp(QNetworkReply* reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        QString ipAddress(reply->readAll());
        myExtInfo = new Peer(ipAddress.trimmed(), myInfo->getName(), true);
        window->setWindowTitle("uMsg - " + myIp() + " - " + myIp(true));
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Network error");
        msgBox.setInformativeText("Could not get your external IP address.  Your interwebs seem to be down.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
    reply->deleteLater();
}

Peer* Controller::me()
{
    return myInfo;
}

QList<Conversation*>* Controller::getConvos()
{
    return convos;
}

QString& Controller::myName()
{
    return myInfo->getName();
}

void Controller::setMyName(const QString& newName)
{
    myInfo->setName(newName);
}

/**
 * Getter for external and local ip switched on the bool external
 *
 * @brief Controller::myIp
 * @throws if requested ip is external and application has no valid internet connection
 * @param external true if you want the external ip, false for local
 * @return the IP as a QString
 */
QString& Controller::myIp(bool external)
{
    if(external)
    {
        if(myExtInfo == 0) throw "I haz no internetz";
        return myExtInfo->getIp();
    }
    else
        return myInfo->getIp();
}

void Controller::sendBroadcastMsg(QString msg)
{
    QString name = "uMsg|" + FLAG_BCAST_MSG + "|" + myName() + "|" + msg;
    conn->broadcastMessage(name);
}

/**
 * Uses UdpHelper to broadcast a message with the flag FLAG_LAN_SCAN to the local bcast adr.
 * Called when the program starts so that all machines on the netwrk will get this client
 * in their contact list automatically when this client starts the program.
 *
 * @brief Controller::scanLan
 */
void Controller::scanLan()
{
    QString msg("uMsg|" + FLAG_LAN_SCAN + "|" + myIp() + "|" + myName());
    conn->broadcastMessage(msg);
}

/**
 * Notifies all Peers within the paramater convo that this instance has added a new member to the conversation.
 * Also sends all the participants' IPs to the newly added PEER.
 *
 * @brief Controller::notifyMembersChanged
 * @param c the conversation which to notify
 * @param sel the selected contact to send all conversation IPs to
 */
void Controller::notifyMembersChanged(Conversation* c, QString sel)
{
    QString msg("uMsg|" + FLAG_ADD_TO_PRIVATE_CHAT + "|" + c->getCid() + "|" + myIp() + "|" + myName() + "|" + sel);
    QString members_str("uMsg|" + FLAG_ADDED_TO_PRIVATE_CHAT + "|" + c->getCid() + "|" + myIp() + "|" + myName() + "|");
    int nrOfRecipiants = c->count()-1;
    for (int i = 0; i < nrOfRecipiants; ++i)
    {
        Peer* p = c->at(i);
        conn->sendMessage(msg, p->getIp());
        members_str += p->getIp() + "|" + p->getName() + "|";
    }
    conn->sendMessage(members_str, c->at(nrOfRecipiants)->getIp());
}

/**
 * Uses UdpConnection to send messages to all participants in the paramater cdata.
 *
 * @brief Controller::sendConvoMessage
 * @param cdata coversation to send messages to
 * @param txtMsg the message to send
 */
void Controller::sendConvoMessage(Conversation* cdata, QString txtMsg)
{
    if(txtMsg.length() <= 0) return;

    for (int i = 0; i < cdata->count(); i++) //Send to all convo participants.
    {
        Peer* p = cdata->at(i);
        QString msg("uMsg|" + FLAG_PRIVATE_CHAT + "|" + cdata->getCid() + "|" + myIp(p->isExternal()) + "|" + myName() + "|" + txtMsg);
        conn->sendMessage(msg, p->getIp());
    }
}

/**
 * Sends a message to all Peers whitin the parameter convo containing the flag FLAG_LEAVE_CONVO,
 * so that this Peer can be stricken from the Peers' conversations lists
 *
 * @brief Controller::leaveConversation
 * @param convo the conversation to notify when leaving
 */
void Controller::leaveConversation(Conversation* convo)
{
    QString msg("uMsg|" + FLAG_LEAVE_CONVO + "|" + myIp() + "|" + myName() + "|" + convo->getCid());
    for (int i = 1; i < convo->count(); i++)
    {
        conn->sendMessage(msg, convo->at(i)->getIp());
    }
}

/**
 * Constructs a logoff message and uses UdpConnection to send it. Called when program exits to notify
 * all local clients that this client has closed the program and should be stricken from their contact lists
 * @brief Controller::logOff
 */
void Controller::logOff()
{
    QString msg("uMsg|" + FLAG_LOGGING_OFF + "|" + myIp() + "|" + myName());
    conn->broadcastMessage(msg);
}

/**
 * Processes the incoming messages and calls the appropriate methods for detail processing
 * depending on the flags within the message. This method switches based on the FLAGS defined in this class.
 * Uses Regular expressions to divide the method into appropriate chunks and sends the detailed data to the
 * appropriate method.
 *
 * @brief Controller::readIncomingMessage
 * @param str the unprocessed messasge
 */
void Controller::readIncomingMessage(QString str)
{
    QRegularExpression isFrom_uMsg("uMsg\\|([0-9])\\|(.*)");
    QRegularExpressionMatch isFrom_match = isFrom_uMsg.match(str);

    if (isFrom_match.hasMatch()) //From uMsg
    {
        QRegularExpression re("(\\d+\\.\\d+\\.\\d+\\.\\d+)\\|(.*)");
        QRegularExpressionMatch match = re.match(isFrom_match.captured(2));

        if(isFrom_match.captured(1).compare(FLAG_BCAST_MSG) == 0)
        {
            window->addTextToConvo(isFrom_match.captured(2));
        }
        else if (isFrom_match.captured(1).compare(FLAG_LAN_SCAN) == 0)
        {
            if(conn->ip().compare(match.captured(1)) != 0)
            {
                Peer* p = new Peer(match.captured(1), match.captured(2));
                window->addLocalContact(p);
                delete p;
                QString msg("uMsg|" + FLAG_LAN_SCAN_ANSWER + "|" + conn->ip() + "|" + myName());
                conn->sendMessage(msg, match.captured(1));
            }
        }
        else if(isFrom_match.captured(1).compare(FLAG_LAN_SCAN_ANSWER) == 0)
        {
            Peer* p = new Peer(match.captured(1), match.captured(2));
            window->addLocalContact(p);
            delete p;
        }
        else if(isFrom_match.captured(1).compare(FLAG_LOGGING_OFF) == 0)
        {
            Peer* p = new Peer(match.captured(1), match.captured(2));
            window->removeLocalContact(p);
            delete p;
        }
        else if(isFrom_match.captured(1).compare(FLAG_PRIVATE_CHAT) == 0)
        {
            window->displayPrivateMsg(isFrom_match.captured(2));
        }
        else if(isFrom_match.captured(1).compare(FLAG_ADD_TO_PRIVATE_CHAT) == 0)
        {
                                            // (cid) | (sender ip) | (sender name) | (new guy name/ip)
            QRegularExpression newMemberReg("(\\d+)\\|(\\d+\\.\\d+\\.\\d+\\.\\d+)\\|(.*)\\|(.*)");
            QRegularExpressionMatch m = newMemberReg.match(isFrom_match.captured(2));
            window->addToConvo(m.captured(2), m.captured(3), m.captured(4), m.captured(1));
        }
        else if(isFrom_match.captured(1).compare(FLAG_ADDED_TO_PRIVATE_CHAT) == 0)
        {
            window->createExistingConvo(isFrom_match.captured(2));
        }
    }
}

void Controller::addExternalContact(const QString& contact)
{
    if(!extContacts->contains(contact))
        extContacts->append(contact);
}

bool Controller::removeExtContact(const QString& toRemove)
{
    return extContacts->removeOne(toRemove);
}

void Controller::unblockContact(const QString& toUnblock)
{
    QStringList lst = toUnblock.split("/");
    blockList->removeOne(lst.at(1));
}

void Controller::blockContact(const QString& toBlock)
{
    QStringList lst = toBlock.split("/");
    if(!blockList->contains(lst.at(1)))
        blockList->append(lst.at(1));
}


void Controller::loadSettings()
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    QString nname = settings.value(NICKNAME, "").toString();
    if(nname.length() > 0)
        setMyName(nname);

    QString contacts = settings.value(CONTACTS, "").toString();
    if(contacts.length() > 0)
    {
        QStringList lst = contacts.split("|", QString::SkipEmptyParts);
        for (int i = 0; i < lst.count(); ++i)
        {
            extContacts->append(lst.at(i));
        }
    }
}

void Controller::saveSettings()
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.setValue(NICKNAME, myName());
    QString contacts("");
    for (int i = 0; i < extContacts->count(); ++i)
    {
        contacts += extContacts->at(i) + "|";
    }
    settings.setValue(CONTACTS, contacts);
}

Controller::~Controller()
{
    delete conn;
    delete convos;
    delete myInfo;
    delete extContacts;
    delete blockList;
    delete accessManager;
    if(myExtInfo != 0) delete myExtInfo;
}

#include "conversation.h"

/**
 * Constructor. Sets the conversation ID to the current milliseconds since epoch
 *
 * @brief Conversation::Conversation
 * @param parent
 */
Conversation::Conversation(QObject *parent) : QObject(parent)
{
    peers = new QList<Peer*>;
    cid = QString::number(QDateTime::currentMSecsSinceEpoch());
}

/**
 * Constructor. Sets the conversation ID to the incoming cid parameter
 *
 * @brief Conversation::Conversation
 * @param cid conversation ID to be used on this Object
 * @param parent
 */
Conversation::Conversation(QString cid, QObject *parent) : QObject(parent), cid(cid)
{
    peers = new QList<Peer*>;
}

int Conversation::count()
{
    return peers->count();
}

Peer* Conversation::at(int i)
{
    return peers->at(i);
}

QString& Conversation::getCid()
{
    return cid;
}

/**
 * Finds and removes a peer with the proivided IP and Name
 *
 * @brief Conversation::remove
 * @param ip the IP of the Peer to find
 * @param name the Name of the Peer to find
 * @return the Peer* if found and removed, 0 if not
 */
Peer* Conversation::remove(QString& ip, QString& name)
{
    for (int i = 0; i < peers->count(); ++i)
    {
        Peer* p = peers->at(i);
        if(p->getIp().compare(ip) == 0 && p->getName().compare(name) == 0)
        {
            peers->removeAt(i);
            return p;
        }
    }
    return 0;
}

/**
 * Finds and the removes the Peer provided in the argument
 *
 * @brief Conversation::remove
 * @param peer the Peer to be removed
 * @return the Peer* if found and removed, 0 if not
 */
Peer* Conversation::remove(Peer* peer)
{
    for (int i = 0; i < peers->count(); ++i)
    {
        Peer* p = peers->at(i);
        if(p->equals(peer))
        {
            peers->removeAt(i);
            return p;
        }
    }
    return 0;
}

/**
 * Adds a Peer to the list of peers with the incoming paramaters as values.
 *
 * @brief Conversation::add
 * @param ip the new Peers IP-address
 * @param name the new Peers Name
 * @param external true if this is an external Peer, false if Local Peer
 * @return the newly created and added Peer
 */
Peer* Conversation::add(const QString& ip, const QString& name, const bool external)
{
    Peer* p = new Peer(ip, name, external);
    peers->append(p);
    return p;
}

/**
 * Checks if a Peer exists in this list with the provided IP and Name.
 *
 * @brief Conversation::contains
 * @param ip IP-address to use as search paramater
 * @param name Name to use as search paramater
 * @return true if contained in the list, false if not
 */
bool Conversation::contains(QString& ip, QString& name)
{
    for (int i = 0; i < peers->count(); ++i)
    {
        Peer* p = peers->at(i);
        if(p->getIp().compare(ip) == 0 && p->getName().compare(name) == 0)
        {
            return true;
        }
    }
    return false;
}

/**
 * Finds and returns the Peer with the provided IP and Name
 *
 * @brief Conversation::get
 * @param ip IP-address to use as search paramater
 * @param name Name to use as search paramater
 * @return he Peer* if found, 0 if not
 */
Peer* Conversation::get(QString& ip, QString& name)
{
    for (int i = 0; i < peers->count(); ++i)
    {
        Peer* p = peers->at(i);
        if(p->getIp().compare(ip) == 0 && p->getName().compare(name) == 0)
        {
            return p;
        }
    }
    return 0;
}

Conversation::~Conversation()
{
    delete peers;
}

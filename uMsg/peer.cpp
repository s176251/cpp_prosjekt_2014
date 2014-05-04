#include "peer.h"

/**
 * Constructor for Peer that assumes an internal ip
 * @brief Peer::Peer
 * @param ip Ip-adress of peer
 * @param name Name/nickname of peer
 */
Peer::Peer(const QString& ip, const QString& name, QObject *parent) : QObject(parent), ip(ip), name(name), external(false)
{
}

/**
 * Constructor for Peer
 * @brief Peer::Peer
 * @param ip Ip-adress of peer
 * @param name Name/nickname of peer
 * @param external Indicates whether ip is external or internal
 */
Peer::Peer(const QString& ip, const QString& name, bool external, QObject *parent) : QObject(parent), ip(ip), name(name), external(external)
{
}

bool Peer::isExternal()
{
    return external;
}

QString& Peer::getIp()
{
    return ip;
}

QString& Peer::getName()
{
    return name;
}

void Peer::setName(const QString& name)
{
    this->name = name;
}

/**
 * @brief Peer::equals
 * @param p Peer to compare
 * @return true if both peer's names and ip's match.
 */
bool Peer::equals(Peer *p)
{
    if(this == p) return true;
    return (this->ip.compare(p->ip) == 0) && (this->name.compare(p->name) == 0);
}

bool Peer::operator ==(Peer& p)
{
    return this->equals(&p);
}

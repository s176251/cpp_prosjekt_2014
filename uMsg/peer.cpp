#include "peer.h"

Peer::Peer(const QString& ip, const QString& name, QObject *parent) : QObject(parent), ip(ip), name(name), external(false)
{
}

Peer::Peer(const QString& ip, const QString& name, bool external, QObject *parent) : QObject(parent), ip(ip), name(name), external(external)
{
}


bool Peer::equals(Peer *p)
{
    if(this == p) return true;
    return (this->ip.compare(p->ip) == 0) && (this->name.compare(p->name) == 0);
}

bool Peer::operator ==(Peer& p)
{
    return this->equals(&p);
}

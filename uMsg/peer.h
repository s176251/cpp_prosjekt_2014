#ifndef PEER_H
#define PEER_H

#include <QObject>

/**
 * Class that represents a client/peer/contact,
 * consists of an ip, name and whether the contact
 * is an external one.
 *
 * @author Jonas Moltzau & Martin W. Løkkeberg
 *
 * @brief The Peer class
 */

class Peer : public QObject
{
    Q_OBJECT
public:
    // Constructors
    Peer(const QString &ip, const QString &name, QObject *parent = 0);
    Peer(const QString &ip, const QString &name, bool external, QObject *parent = 0);

    //Returns the ip of the peer
    QString& getIp();
    //Returns the name of the peer
    QString& getName();
    //Sets the peer's name
    void setName(const QString &name);
    //Compares this peer with given peer
    bool equals(Peer* p);
    //Calls leftPeer.equals(rightPeer)
    bool operator ==(Peer& p);
    //Returns whether peer is external
    bool isExternal();
private:
    QString ip;
    QString name;
    bool external;

signals:

public slots:

};

#endif // PEER_H

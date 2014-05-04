#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <QObject>
#include <QTime>
#include <QDate>
#include "peer.h"

/**
 * Container class for Peers. Maintaines a list of Peers within the conversation.
 *
 * @author Jonas Moltzau & Martin W. Løkkeberg
 *
 * @brief The Conversation class
 */
class Conversation : public QObject
{
    Q_OBJECT

public:
    //Constructor. Sets the conversation ID to the current milliseconds since epoch
    explicit Conversation(QObject *parent = 0);

    //Constructor. Sets the conversation ID to the incoming cid parameter
    explicit Conversation(QString cid, QObject *parent = 0);

    //Getter method for the conversation ID
    QString& getCid();
    //Finds and removes a peer with the proivided IP and Name
    Peer* remove(QString& ip, QString& name);
    //Finds and the removes the incoming Peer
    Peer* remove(Peer* peer);
    //Adds a Peer to the list of peers with the incoming paramaters as values.

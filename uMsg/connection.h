#ifndef CONNECTION_H
#define CONNECTION_H

class Connection
{
public:
    virtual void broadcastMessage(const QString &msg)=0;
    virtual void sendMessage(const QString &msg, const QString &ip)=0;
#ifndef PEER_H
#define PEER_H

#include <QObject>

class Peer : public QObject
{
    Q_OBJECT
public:
    Peer(const QString &ip, const QString &name, QObject *parent = 0);
    Peer(const QString &ip, const QString &name, bool external, QObject *parent = 0);

    QString& getIp();
    QString& getName();
    QString& getNick();
    void setName(const QString &name);
    bool equals(Peer* p);
    bool operator ==(Peer& p);

    bool isExternal();
private:
    QString ip;
    QString name;
    QString nick;
    bool external;

signals:

public slots:

};

#endif // PEER_H

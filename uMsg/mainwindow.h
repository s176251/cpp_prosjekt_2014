#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QTextEdit>
#include <QStandardItemModel>
#include <QListWidgetItem>
#include <QKeyEvent>
#include <QTime>
#include <QDate>
#include <QFormBuilder>
#include <QTableView>
#include <QSystemTrayIcon>
#include <QInputDialog>
#include <QTimer>
#include <QMessageBox>
#include <QTextCodec>

#include "controller.h"

namespace Ui {
class MainWindow;
}

/**
 * Main window for the application.
 *
 * @author Martin W. Løkkeberg (s176251) & Jonas Moltzau (s176250)
 *
 * @brief Main Window class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    QList<Conversation*> *convos;
    QList<QListWidgetItem*>* smileys;
    Controller* controller;
    QSystemTrayIcon *trayIcon;
    int lastUpdatedTab;
    QMenu* trayMenu;
    QMenu* contactsMenu;

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    //Adds text to a conversation
    void appendToConvo(const QString &text);
    //Listens on events (here key presses)
    bool eventFilter(QObject *object, QEvent *event);
    //Adds a contact to conversation list
    bool addLocalContact(Peer* peer);
    //Removes a contact from the conversation list
    bool removeLocalContact(Peer* peer);
    void addTextToConvo(QString text);
    void displayPrivateMsg(QString data);
    bool addToConvo(QString fromIp, QString fromName, QString member, QString cid);
    void createExistingConvo(QString data);

    void changeEvent(QEvent *e);
    void indicateChange(int index, int flag = 1);
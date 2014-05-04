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
    //Adds text to main-tab
    void addTextToConvo(QString text);
    //Adds a recieved message to an existing or new tab.
    void displayPrivateMsg(QString data);
    //Adds a member to an existing conversation
    bool addToConvo(QString fromIp, QString fromName, QString member, QString cid);
    //Creates a new tab and conversation initialized with multiple members
    void createExistingConvo(QString data);
    //Handles minimize to tray when minimized.
    void changeEvent(QEvent *e);
    //Changes tab headers based on flag.
    void indicateChange(int index, int flag = 1);
private:
    Ui::MainWindow *ui;
    //Sets up ui-elements
    void initializeUiElements();
    //Adds text to currently selected tab
    void addTextToConvo(QTextEdit *convo, QString text);
    //Creates a tab and initializes a new conversation.
    void createTab(QString cid, QString ip, QString name);
    //Adds selected contact to a conversation
    void contextAddToConvo(QString sel);
    //Displays a balloon notification in the tray
    void displayTrayMsg(int index, const QString &text);
    //Unhides the window
    void openWindow();
    //Creates the tray icon
    void createTray();
private slots:
    //Sends broadcast-messages from the main tab.
    void sendMsg();
    //Saves settings and broadcasts shutdown signal.
    void closing();
    //Creates a new tab and conversation
    void newPrivateConvo();
    // Removes the conversation corresponding to the tab being closed.
    void on_tabgrpConversations_tabCloseRequested(int index);
    //Sends message to convo participants
    void sendConvoMessage();
    //Adds smiley tag to message
    void on_lstSmileys_doubleClicked(const QModelIndex &index);
    //Repopulates the participant list
    void on_tabgrpConversations_currentChanged(int index);
    //Creates contact list context menu
    void on_lstContacts_customContextMenuRequested(const QPoint &pos);
    //Adds new contact
    void on_pushButton_clicked();
    //Opens window on tray double-click
    void trayActions(QSystemTrayIcon::ActivationReason reason);
    //Opens window on tray msg click
    void trayMessageClicked();
    //Opens tray menu
    void trayMenuClicked(QAction *action);
    //Sets users nickname
    void setNickname();
    //Adds/Removes/Blocks contact
    void contactsMenuClicked(QAction *action);
    //Empties tab text
    void clearTab();
};

#endif // MAINWINDOW_H

// 2014-05-04 - Martin W. Løkkeberg (s176251) & Jonas Moltzau (s176250)
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "peer.h"
#include "conversation.h"

const int CHANGE_NEW = 1;
const QString APPLICATION_ICON = ":/images/images/icon.ico";
const QString APPLICATION_ICON_RED = ":/images/images/red.ico";
const QString RES_SMILEYS = ":/smileys/images/smileys/";
const QString FORM_TAB = ":/Forms/tab.ui";
const QString CONVO_TAB_TXT_ID = "txtTabConvo";
const QString CONVO_TAB_MSG_ID = "txtTabMessage";
const QString STR_SHOW = "Show";
const QString STR_EXIT = "Exit";
const QString STR_ADD_TO_CONVO = "Add to conversation";
const QString STR_REMOVE = "Remove";
const QString STR_BLOCK_UNBLK = "Block/Unblock";
const QString DATE_FORMAT = "yyyy.MM.dd";
const QString TIME_FORMAT = "HH.mm";

/**
 * Constructor for MainWindow
 * Sets up the ui and main window for the application
 * @brief MainWindow::MainWindow
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    //Make sure all instances of the application sends and recieves using the same encoding.
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    ui->setupUi(this);

    // Create controller, which handles most of the logic and non gui-work.
        controller = new Controller(this);

    // Get pointer to conversation list
        convos = controller->getConvos();

    initializeUiElements();
    ui->lstContacts->addItems(controller->getSavedContacts());

    // Initialize LAN broadcast
        controller->scanLan();
}

/**
 * Set-up additional ui elements
 * @brief MainWindow::initializeUiElements
 */
void MainWindow::initializeUiElements()
{
    ui->txtMessage->installEventFilter(this);

    //Hide close button from the first tab, which should always be visible.
    ui->tabgrpConversations->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);

    //Connect filemenu actions
    connect(ui->actionScan_Lan, SIGNAL(triggered()), controller, SLOT(scanLan()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionSet_nickname, SIGNAL(triggered()), this, SLOT(setNickname()));
    connect(ui->actionClear_current_window, SIGNAL(triggered()), this, SLOT(clearTab()));

    //Create list of smileys and add to current ui-list.
    smileys = new QList<QListWidgetItem*>;
    for (int i = 0; i < 40; ++i)
    {
        QListWidgetItem* qlwi = new QListWidgetItem(QIcon(RES_SMILEYS + QString::number(i) + ".png"), "");
        smileys->append(qlwi);
        ui->lstSmileys->addItem(qlwi);
    }

    createTray();
}

/**
 * Clears text from the currently selected tab
 * @brief MainWindow::clearTab
 */
void MainWindow::clearTab()
{
    if(ui->tabgrpConversations->currentIndex() == 0)
        ui->txtConvo->clear();
    else
    {
        QTextEdit* txtConv = ui->tabgrpConversations->currentWidget()->findChild<QTextEdit*>(CONVO_TAB_TXT_ID);
        txtConv->clear();
    }
}

/**
 * Prompts the user to input a nickname for him/herself. Default is computer username.
 * @brief MainWindow::setNickname
 */
void MainWindow::setNickname()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Set your nickname", "Nickname:", QLineEdit::Normal,
                                         controller->myName(), &ok);

    if (ok && !text.isEmpty())
        controller->setMyName(text);
}

/**
 * Creates a tray icon and tray icon menu for the application.
 * Connects actions for clicking on tray icon, message box and right-click.
 * @brief MainWindow::createTray
 */
void MainWindow::createTray()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(APPLICATION_ICON));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActions(QSystemTrayIcon::ActivationReason)));
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(trayMessageClicked()));

    trayMenu = new QMenu(this);
    trayMenu->addAction(new QAction(STR_SHOW, this));
    trayMenu->addAction(new QAction(STR_EXIT, this));
    connect(trayMenu, SIGNAL(triggered(QAction*)), this, SLOT(trayMenuClicked(QAction*)));
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}

/**
 * Opens window if "Show"-action is clicked in tray icon context menu, otherwise closes it.
 * @brief MainWindow::trayMenuClicked
 * @param action Action that has been clicked.
 */
void MainWindow::trayMenuClicked(QAction* action)
{
   if( trayMenu->actions().at(0) == action)
   {
       if(!isVisible()) openWindow();
   }
   else close();
}

/**
 * Conveniance method for sending broadcast-messages from the main tab.
 * @brief MainWindow::sendMsg
 */
void MainWindow::sendMsg()
{
    QString newMsg = ui->txtMessage->toPlainText();
    if(newMsg.length() > 0)
    {
        controller->sendBroadcastMsg(newMsg);
        ui->txtMessage->clear();
    }
}

/**
 * Adds a Peer to the list of contacts, if it doesnt exist from before.
 * @brief MainWindow::addLocalContact
 * @param peer Peer to add
 * @return false if Peer already is in contact list, else true
 */
bool MainWindow::addLocalContact(Peer* peer)
{
    QListWidget* list =  ui->lstContacts;
    QString str(peer->getName() + "/" + peer->getIp());

    for (int i = 0; i < list->count(); ++i)
    {
        if (list->item(i)->data(Qt::DisplayRole).toString().compare(str) == 0) return false;
    }

    list->addItem(str);
    return true;
}

/**
 * Removes a peer from the list of contacts.
 * @brief MainWindow::removeLocalContact
 * @param p Peer to remove
 * @return true if found and deleted
 */
bool MainWindow::removeLocalContact(Peer* p)
{
    QListWidget* list =  ui->lstContacts;
    QString str(p->getName() + "/" + p->getIp());

    for (int i = 0; i < list->count(); ++i)
    {
        if (list->item(i)->data(Qt::DisplayRole).toString().compare(str) == 0)
        {
            delete (list->takeItem(i));
            return true;
        }
    }
    return false;
}

/**
 * Conveniance method for adding text to the main-tab conversation.
 * @brief MainWindow::addTextToConvo
 * @param text "{name}|{msg}"
 */
void MainWindow::addTextToConvo(QString text)
{
    addTextToConvo(ui->txtConvo, text);
    if(ui->tabgrpConversations->currentIndex() != 0)
        indicateChange(0);
    if(!isVisible()) displayTrayMsg(0, text);
}

/**
 * Adds given text to the given QTextEdit-field, formats the text with html.
 * @brief MainWindow::addTextToConvo
 * @param text "{name}|{msg}"
 */
void MainWindow::addTextToConvo(QTextEdit* convo, QString text)
{
    QStringList list = text.split("|", QString::SkipEmptyParts);

    QString color("Blue");

    // If message originates from "me", display it in green.
    if(list[0] == controller->myName())
    {
        color = "Green";
    }

    QString str("<font color=\"" + color + "\"><b>" + list[0] + "</b></font><font color=\"#C0C0C0\">");
    str += " (" + QDate::currentDate().toString(DATE_FORMAT) + " " + QTime::currentTime().toString(TIME_FORMAT) + ")</font>:<pre style=\"margin:0px;padding:0px;\">\t";
    for (int i = 1; i < list.size(); ++i)
    {
        str += list[i];
    }
    str += "</pre>";
    str.replace("\n", "\n\t");

    // Make sure everything is appended at the end (not default when inserting html).
    QTextCursor qtc = convo->textCursor();
    qtc.movePosition(QTextCursor::End);
    convo->setTextCursor(qtc);

    convo->insertHtml(str);
    convo->append("");

    qtc.movePosition(QTextCursor::End);
    convo->setTextCursor(qtc);
}

/**
 * Eventfilter for the main-window.
 * Used to differentiate between "Enter", which should send a message,
 * and "Shift-Enter" which should create a newline.
 * Other events are passed on to the base class.
 * @brief MainWindow::eventFilter
 */
bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if ((object == ui->txtMessage || object->objectName().compare(CONVO_TAB_MSG_ID) == 0)  && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_Return && keyEvent->modifiers() == Qt::ShiftModifier)
        {
            return QMainWindow::eventFilter(object, event);
        }
        else if(keyEvent->key() == Qt::Key_Return)
        {
            if(ui->tabgrpConversations->currentIndex() == 0)
                sendMsg();
            else
                sendConvoMessage();
            return true;
        }
        else
            return QMainWindow::eventFilter(object, event);
    }
    else
        return QMainWindow::eventFilter(object, event);
}

/**
 * Hides the window when it is minimized, so that it's only visible in the tray.
 * @brief MainWindow::changeEvent
 */
void MainWindow::changeEvent(QEvent* e)
{
    switch (e->type())
    {
        case QEvent::WindowStateChange:
            {
                if (this->windowState() & Qt::WindowMinimized)
                {
                    // Use a timer to make sure all events in the event-loop has finished.
                    QTimer::singleShot(0, this, SLOT(hide()));
                }
                break;
            }
        default:
            break;
    }
    QMainWindow::changeEvent(e);
}

/**
 * Unhides the window and puts it in a focused state.
 * @brief MainWindow::openWindow
 */
void MainWindow::openWindow()
{
    show();
    setWindowState(Qt::WindowActive);
    trayIcon->setIcon(QIcon(APPLICATION_ICON));
}

/**
 * Unhides the main window when tray icon is double-clicked.
 * @brief MainWindow::trayActions
 */
void MainWindow::trayActions(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            break;
        case QSystemTrayIcon::DoubleClick:
            openWindow();
            break;
        case QSystemTrayIcon::MiddleClick:
            break;
        default:
            break;
    }
}

/**
 * Saves settings to ini file, and sends a log off signal.
 * @brief MainWindow::closing
 */
void MainWindow::closing()
{
    controller->saveSettings();
    controller->logOff();
}

/**
 * Gets the conversation corresponding to the
 * currently selected tab, and sends whatever's in its
 * message text-box to all participants of the conversation.
 * @brief MainWindow::sendConvoMessage
 */
void MainWindow::sendConvoMessage()
{
    Conversation *cdata = convos->at(ui->tabgrpConversations->currentIndex()-1);
    QTextEdit* txtMsg = ui->tabgrpConversations->currentWidget()->findChild<QTextEdit*>(CONVO_TAB_MSG_ID);
    QTextEdit* txtConv = ui->tabgrpConversations->currentWidget()->findChild<QTextEdit*>(CONVO_TAB_TXT_ID);

    if(txtMsg->toPlainText().length() > 0)
    {
        controller->sendConvoMessage(cdata, txtMsg->toPlainText());
        addTextToConvo(txtConv, controller->myName() + "|" + txtMsg->toPlainText());
        txtMsg->clear();
    }
}

/**
 * Called when double-clicking a contact in the contact list.
 * Creates a new conversation with a new id, and a new tab.
 * @brief MainWindow::newPrivateConvo
 */
void MainWindow::newPrivateConvo()
{
    QString pConvo(ui->lstContacts->selectedItems().at(0)->data(Qt::DisplayRole).toString());
    QTabWidget* grp = ui->tabgrpConversations;

    QStringList t = pConvo.split("/", QString::SkipEmptyParts);
    createTab(QString::number(QDateTime::currentMSecsSinceEpoch()), t.at(1), t.at(0));

    //Set selected tab to newly created
    grp->setCurrentIndex(grp->count()-1);
}

/**
 * Creates a new tab, and initializes a new conversation with given ip and name.
 * @brief MainWindow::createTab
 * @param cid Conversation id.
 * @param ip IPv4 address
 * @param name Nickname
 */
void MainWindow::createTab(QString cid, QString ip, QString name)
{
    // Inflate tablelayout from file
        QFormBuilder builder;
        QFile file(FORM_TAB);
        file.open(QFile::ReadOnly);
        QWidget *tabLayout = builder.load(&file, this);
        file.close();

    // Add layout to a new tab
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(tabLayout);
        QWidget *newTab = new QWidget(ui->tabgrpConversations);
        newTab->setLayout(layout);
        ui->tabgrpConversations->addTab(newTab, name + "/" + ip);

    //Add eventlisteners to new tab button, message text-box and smiley-list.
    QPushButton* btnSend = tabLayout->findChild<QPushButton*>("btnTabSend");
    connect(btnSend, SIGNAL(clicked()), this, SLOT(sendConvoMessage()));

    QTextEdit* txtMsg = tabLayout->findChild<QTextEdit*>(CONVO_TAB_MSG_ID);
    txtMsg->installEventFilter(this);

    QListWidget* lstSmil = tabLayout->findChild<QListWidget*>("lstTabSmileys");
    connect(lstSmil, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_lstSmileys_doubleClicked(QModelIndex)));
    for (int i = 0; i < smileys->count(); i++)
    {
        QListWidgetItem* qlwi = new QListWidgetItem(QIcon(RES_SMILEYS + QString::number(i) + ".png"), "");
        lstSmil->addItem(qlwi);
    }

    Conversation* c = new Conversation(cid);
    if(controller->isExternal(ip)) //External ip
    {
        c->add(ip, name, true);
    }
    else c->add(ip, name);

    convos->append(c);
}

/**
 * Called when recieving a private message.
 * Adds message to a tab if the tab corresponds to the given convo id,
 * otherwise creates a new tab.
 * @brief MainWindow::displayPrivateMsg
 * @param data "{convo_id}|{from_ip}|{message_txt}"
 */
void MainWindow::displayPrivateMsg(QString data)
{
    QRegularExpression re("(\\d+)\\|(\\d+\\.\\d+\\.\\d+\\.\\d+)\\|(.*)");
    QRegularExpressionMatch match = re.match(data);

    for (int i = 0; i < convos->count(); i++)
    {
        if(convos->at(i)->getCid().compare(match.captured(1)) == 0)
        {
            QTextEdit* tmp = ui->tabgrpConversations->widget(i+1)->findChild<QTextEdit*>(CONVO_TAB_TXT_ID);
            addTextToConvo(tmp, match.captured(3));

            if(ui->tabgrpConversations->currentIndex() != i+1) //if tab not focused, set header color to red.
                indicateChange(i+1, CHANGE_NEW);
            if(!isVisible())
                displayTrayMsg(i+1, match.captured(3));
            return;
        }
    }
    createTab(match.captured(1), match.captured(2), match.captured(3).split("|").at(0));
    QTextEdit* tmp = ui->tabgrpConversations->widget(convos->count())->findChild<QTextEdit*>(CONVO_TAB_TXT_ID);
    addTextToConvo(tmp, match.captured(3));
    indicateChange(convos->count(), CHANGE_NEW);
    if(!isVisible()) displayTrayMsg(convos->count(), match.captured(3));
}

/**
 * Called to indicate a tab has changed content or recieved focus.
 * @brief MainWindow::indicateChange
 * @param index Tabindex
 * @param flag Type of change
 */
void MainWindow::indicateChange(int index, int flag)
{
    if(flag == CHANGE_NEW)
    {
        ui->tabgrpConversations->tabBar()->setTabTextColor(index, Qt::red);
    }
    else
        ui->tabgrpConversations->tabBar()->setTabTextColor(index, Qt::black);
}

/**
 * Called if window is hidden when it recieves a message.
 * Displays the message in a ballon-popup in the tray.
 * Also sets tray icon to be red, to indicate an unread message.
 * @brief MainWindow::displayTrayMsg
 * @param index Tabindex
 * @param text "{sender_name}|{message}"
 */
void MainWindow::displayTrayMsg(int index, const QString& text)
{
    lastUpdatedTab = index;
    if(ui->actionBalloon_Notification->isChecked())
    {
        QStringList lst = text.split("|", QString::SkipEmptyParts);
        if(lst.count() > 1)
        {
            QStringRef txt(&lst.at(1), 0, lst.at(1).length());
            if(lst.at(1).length() > 20) txt = QStringRef(&lst.at(1), 0, 20);
            trayIcon->showMessage(lst.at(0) + " says:", txt.toString());
        }
    }
    trayIcon->setIcon(QIcon(APPLICATION_ICON_RED));
}

/**
 * Focuses the tab with the most recent message,
 * when a ballon-popup is clicked.
 * @brief MainWindow::trayMessageClicked
 */
void MainWindow::trayMessageClicked()
{
    openWindow();
    ui->tabgrpConversations->setCurrentIndex(lastUpdatedTab);
}

/**
 * Removes the conversation corresponding to the tab being closed.
 * @brief MainWindow::on_tabgrpConversations_tabCloseRequested
 * @param index Tabindex
 */
void MainWindow::on_tabgrpConversations_tabCloseRequested(int index)
{
    Conversation *c = convos->at(index-1);
    controller->leaveConversation(c);
    convos->removeAt(index-1);
    delete c;
    delete ui->tabgrpConversations->widget(index);
}

/**
 * Inserts an image tag corresponding to the clicked smiley
 * into the message-textbox.
 * @brief MainWindow::on_lstSmileys_doubleClicked
 */
void MainWindow::on_lstSmileys_doubleClicked(const QModelIndex &index)
{
    QString img = "<img src=\"images/smileys/small/" + QString::number(index.row()) + ".png\" />";
    QTextEdit* txtMsg = ui->txtMessage;

    if(ui->tabgrpConversations->currentIndex() != 0)
    {
        txtMsg = ui->tabgrpConversations->currentWidget()->findChild<QTextEdit*>(CONVO_TAB_MSG_ID);
    }
    txtMsg->insertPlainText(img);
    txtMsg->setFocus();
}

/**
 * Called when tab is switched, empties the participant list,
 * and populates it with the participants in the newly selected
 * tab.
 * @brief MainWindow::on_tabgrpConversations_currentChanged
 */
void MainWindow::on_tabgrpConversations_currentChanged(int index)
{
    ui->lstInConvo->clear();

    indicateChange(index, Qt::black);
    if(index == 0) return;

    Conversation* lst = convos->at(index-1);

    for (int i = 0; i < lst->count() ; ++i)
    {
        Peer* p = lst->at(i);
        ui->lstInConvo->addItem(p->getName() + "/" + p->getIp());
    }
}

/**
 * Called when user has been added to an existing conversation
 * between two or more participants. Creates a new tab, and
 * adds all participants to the corresponding conversation list.
 * @brief MainWindow::createExistingConvo
 * @param data "{convo_id}|{ip}|{name}|{ip}|{name}|..."
 */
void MainWindow::createExistingConvo(QString data)
{
    QStringList stuff = data.split('|', QString::SkipEmptyParts);

    createTab(stuff.at(0), stuff.at(1), stuff.at(2));
    indicateChange(convos->count());
    Conversation* newConvo = convos->at(convos->count()-1);

    for (int i = 3; i < stuff.count(); i++)
    {
        QString ip = stuff.at(i++); //Separated to avoid possibly undefined behavior.
        newConvo->add(ip, stuff.at(i));
    }
}

/**
 * Called when context menu on the contact list has been clicked.
 * Calls methods to respond to clicked action.
 * @brief MainWindow::contactsMenuClicked
 */
void MainWindow::contactsMenuClicked(QAction *action)
{
    QString sel(ui->lstContacts->selectedItems().at(0)->data(Qt::DisplayRole).toString());

    if(action->text().compare(STR_ADD_TO_CONVO) == 0) //Add contact to conversation
    {
        contextAddToConvo(sel);
    }
    if(action->text().compare(STR_REMOVE) == 0) //Remove contact from list
    {
        ui->lstContacts->takeItem(ui->lstContacts->currentRow());
        controller->removeExtContact(sel);
    }
    if(action->text().compare(STR_BLOCK_UNBLK) == 0) //Block contact
    {
        // Indicate a blocked contact with grey text and strikethrough.
        QFont f = ui->lstContacts->selectedItems().at(0)->font();
        if(f.strikeOut())
        {
            ui->lstContacts->selectedItems().at(0)->setTextColor(Qt::black);
            controller->unblockContact(sel);
        }
        else
        {
            ui->lstContacts->selectedItems().at(0)->setTextColor(Qt::gray);
            controller->blockContact(sel);
        }
        f.setStrikeOut(!f.strikeOut());
        ui->lstContacts->selectedItems().at(0)->setFont(f);
    }
}

/**
 * Adds selected participant to a conversation, and notifies participants.
 * @brief MainWindow::contextAddToConvo
 * @param sel Member to add, formatted: "{name}/{ip}"
 */
void MainWindow::contextAddToConvo(QString sel)
{
    Conversation* convo = convos->at(ui->tabgrpConversations->currentIndex()-1);
    if(addToConvo("","",sel, convo->getCid()))
    {
        controller->notifyMembersChanged(convo, sel);
    }
}

/**
 * Adds a participant to a convo, if that conversation dont exist, the tab and convo is created.
 * @brief MainWindow::addToConvo
 * @param fromIp ip of user that added someone to a conversation, empty if originating user is the current user.
 * @param fromName name of user that added someone to a conversation, empty if originating user is the current user.
 * @param member new participant to be added: "{name}/{ip}"
 * @param cid Conversation id.
 * @return false if member already exists in the conversation, true otherwise.
 */
bool MainWindow::addToConvo(QString fromIp, QString fromName, QString member, QString cid)
{
    Conversation* lst = 0;
    QStringList tmp = member.split("/", QString::SkipEmptyParts);
    int i = 0;
    for (; i < convos->count(); ++i)
    {
        if(convos->at(i)->getCid().compare(cid) == 0)
        {
            lst = convos->at(i);
            break;
        }
    }

    if(lst == 0)
    {
        createTab(cid, fromIp, fromName);
        lst = convos->at(convos->count()-1);
    }
    else
    {
        for (int i = 0; i < lst->count(); ++i)
        {
            if(tmp.at(1).compare(lst->at(i)->getIp()) == 0) return false;
        }
    }

    lst->add(tmp.at(1), tmp.at(0));

    if(ui->tabgrpConversations->currentIndex() == (i+1))
        ui->lstInConvo->addItem(member);

    return true;
}

/**
 * Creates a context menu when user right-clicks on a contact.
 * @brief MainWindow::on_lstContacts_customContextMenuRequested
 * @param pos Cooridinates of right-click
 */
void MainWindow::on_lstContacts_customContextMenuRequested(const QPoint &pos)
{
    if(ui->lstContacts->selectedItems().count() < 1) return;

    contactsMenu = new QMenu("Context menu", this);
    QAction* add = new QAction(STR_ADD_TO_CONVO, this);
    if(ui->tabgrpConversations->currentIndex() == 0) add->setDisabled(true);
    contactsMenu->addAction(add);
    contactsMenu->addAction(new QAction(STR_REMOVE, this));
    contactsMenu->addAction(new QAction(STR_BLOCK_UNBLK, this));
    connect(contactsMenu, SIGNAL(triggered(QAction*)), this, SLOT(contactsMenuClicked(QAction*)));
    contactsMenu->exec(ui->lstContacts->mapToGlobal(pos));
    contactsMenu->deleteLater();
}

/**
 * Called when user presses a button to add a contact.
 * Prompts user with an input dialog to name his new contact.
 * @brief MainWindow::on_pushButton_clicked
 */
void MainWindow::on_pushButton_clicked()
{
    QString str(ui->addContact->text());
    QRegularExpression re("^\\d+\\.\\d+\\.\\d+\\.\\d+$");
    QRegularExpressionMatch match = re.match(str);
    if(match.hasMatch())
    {
        bool ok;
        QString text = QInputDialog::getText(this, "Name the contact", "Name:", QLineEdit::Normal, str, &ok);

        if (!ok) return;

        QString contact(str + "/" + str);
        if(!text.isEmpty()) contact = text + "/" + str;

        ui->lstContacts->addItem(contact);
        controller->addExternalContact(contact);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Invalid ip");
        msgBox.setInformativeText("Please provide a valid IPv4 adress");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
}

/**
 * Destructor
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete trayMenu;
    delete trayIcon;
    delete smileys;
    delete ui;
    delete controller;
}

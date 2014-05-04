// 2014-05-04 - Martin W. Løkkeberg (s176251) & Jonas Moltzau (s176250)
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "peer.h"
#include "conversation.h"

const int COLOR_RED = 1;

/**
 * Constructor for MainWindow
 * Sets up the ui and main window for the application
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

// Set-up additional ui elements
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
        QListWidgetItem* qlwi = new QListWidgetItem(QIcon(":/smileys/images/smileys/" + QString::number(i) + ".png"), "");
        smileys->append(qlwi);
        ui->lstSmileys->addItem(qlwi);
    }

    createTray();
}

// Clears text from the currently selected tab
void MainWindow::clearTab()
{
    if(ui->tabgrpConversations->currentIndex() == 0)
        ui->txtConvo->clear();
    else
    {
        QTextEdit* txtConv = ui->tabgrpConversations->currentWidget()->findChild<QTextEdit*>("txtTabConvo");
        txtConv->clear();
    }
}

// Prompts the user to input a nickname for him/herself. Default is computer username.
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
 */
void MainWindow::createTray()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/images/icon.ico"));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActions(QSystemTrayIcon::ActivationReason)));
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(trayMessageClicked()));

    trayMenu = new QMenu(this);
    trayMenu->addAction(new QAction("Show", this));
    trayMenu->addAction(new QAction("Exit", this));
    connect(trayMenu, SIGNAL(triggered(QAction*)), this, SLOT(trayMenuClicked(QAction*)));
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}

// Opens window if "Show"-action is clicked in tray icon context menu, otherwise closes it.
void MainWindow::trayMenuClicked(QAction* action)
{
   if( trayMenu->actions().at(0) == action)
   {
       if(!isVisible()) openWindow();
   }
   else close();
}

// Conveniance method for sending broadcast-messages from the main tab.
void MainWindow::sendMsg()
{
    QString newMsg = ui->txtMessage->toPlainText();
    if(newMsg.length() > 0)
    {
        controller->sendBroadcastMsg(newMsg);
        ui->txtMessage->clear();
    }
}

// Adds a Peer to the list of contacts, if it doesnt exist from before.
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

// Removes a peer from the list of contacts.
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

// Conveniance method for adding text to the main-tab conversation.
void MainWindow::addTextToConvo(QString text)
{
    addTextToConvo(ui->txtConvo, text);
    if(ui->tabgrpConversations->currentIndex() != 0)
        indicateChange(0);
    if(!isVisible()) displayTrayMsg(0, text);
}

// Adds given text to the given QTextEdit-field, formats the text with html.
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
    str += " (" + QDate::currentDate().toString("yyyy.MM.dd") + " " + QTime::currentTime().toString("HH.mm") + ")</font>:<pre style=\"margin:0px;padding:0px;\">\t";
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
 */
bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if ((object == ui->txtMessage || object->objectName().compare("txtTabMessage") == 0)  && event->type() == QEvent::KeyPress)
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

// Hides the window when it is minimized, so that it's only visible in the tray.
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

// Unhides the window and puts it in a focused state.
void MainWindow::openWindow()
{
    show();
    setWindowState(Qt::WindowActive);
    trayIcon->setIcon(QIcon(":/images/images/icon.ico"));
}

// Unhides the main window when tray icon is double-clicked.
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

// Saves settings to ini file, and sends a log off signal.
void MainWindow::closing()
{
    controller->saveSettings();
    controller->logOff();
}

/**
 * Gets the conversation corresponding to the
 * currently selected tab, and sends whatever's in its
 * message text-box to all participants of the conversation.
 */
void MainWindow::sendConvoMessage()
{
    Conversation *cdata = convos->at(ui->tabgrpConversations->currentIndex()-1);
    QTextEdit* txtMsg = ui->tabgrpConversations->currentWidget()->findChild<QTextEdit*>("txtTabMessage");
    QTextEdit* txtConv = ui->tabgrpConversations->currentWidget()->findChild<QTextEdit*>("txtTabConvo");

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

// Creates a new tab, and initializes a new conversation with given ip and name.
void MainWindow::createTab(QString cid, QString ip, QString name)
{
    // Inflate tablelayout from file
        QFormBuilder builder;
        QFile file(":/Forms/tab.ui");
        file.open(QFile::ReadOnly);
        QWidget *tabLayout = builder.load(&file, this);
        file.close();

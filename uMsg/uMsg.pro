#-------------------------------------------------
#
# Project created by QtCreator 2014-04-22T12:40:06
#
#-------------------------------------------------

QT       += core gui\
            network\
            designer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = uMsg
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    udpconnection.cpp \
    controller.cpp \
    conversation.cpp \
    peer.cpp

HEADERS  += mainwindow.h \
    udpconnection.h \
    controller.h \
    conversation.h \
    peer.h \
    connection.h

FORMS    += mainwindow.ui \
    tab.ui

RESOURCES += \
    res.qrc

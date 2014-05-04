// 2014-05-04 - Martin W. Løkkeberg (s176251) & Jonas Moltzau (s176250)
#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>

/*
 * Entry point for the application.
 * Creates and displays the main window.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // If the application can't get a local ip (should never occur),
    // it will throw an exception and the program should not start.
    try
    {
        MainWindow w;

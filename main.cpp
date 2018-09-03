#include "mainwindow.h"
#include <QApplication>

#include <qDebug>
#include <QLabel>
#include <QDir>
#include <QString>
#include "cdatabasemanager.h"
#include "ctextdatareader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}

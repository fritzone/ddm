#include <QtGui/QApplication>
#include<QWindowsStyle>
#include <QStyle>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //QApplication::setStyle(new QWindowsStyle);
    MainWindow w;
    w.show();
    return a.exec();
}

#include <QtGui/QApplication>
#include<QWindowsStyle>
#include<QMacStyle>
#include <QStyle>
#include <QCleanlooksStyle>
#include <QPlastiqueStyle>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

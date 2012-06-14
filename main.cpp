#include <QtGui/QApplication>
#include<QWindowsStyle>
#include<QMacStyle>
#include <QStyle>
#include <QCleanlooksStyle>
#include <QPlastiqueStyle>
#include <QDebug>

#include "MainWindow.h"

#include "QHtmlDocument.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QHtmlDocument doc("My document");
    qDebug() << doc.html();

//    MainWindow w;
//    w.show();
    return a.exec();
}

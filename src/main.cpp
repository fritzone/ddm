#include <QtGui/QApplication>

#include "Workspace.h"
#include "MainWindow.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

#include <QMessageBox>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("The Unauthorized Frog");
    QApplication::setApplicationName("ddm");
    QApplication::setApplicationVersion("0.1");

//    QString s = QSqlDatabase::drivers().join(" ");
//    QMessageBox::information(0, "a", s, QMessageBox::Ok);
//    QSqlDatabase cubridDb = QSqlDatabase::addDatabase("QCUBRID");
//    cubridDb.setHostName("127.0.0.1");
//    cubridDb.setUserName("dba");
//    cubridDb.setPassword("");
//    cubridDb.setPort(30000);
//    cubridDb.setDatabaseName("");

//    if(!cubridDb.open())
//    {
//        QSqlError error = cubridDb.lastError();
//        QMessageBox::information(0, "cannot connect",
//                                 error.databaseText() + " " + error.driverText() + " --> "  + QString::number(error.number()),
//                                 QMessageBox::Ok);
//    }
//    else
//    {
//        QSqlQuery q(cubridDb);
//        q.prepare("select LIST_DBS()");
//        if(!q.exec())
//        {
//            QMessageBox::information(0, "no exec", "a", QMessageBox::Ok);
//        }
//        else

//        {
//            QString v = q.value(0).toString();
//            qDebug() << "XXXXXXXX" << q.size();
//            QMessageBox::information(0, "a", v, QMessageBox::Ok);
//        }
//        cubridDb.close();

//    }

    QObject::connect(&a, SIGNAL(aboutToQuit()), Workspace::getInstance(), SLOT(cleanup()));

    MainWindow w;
    w.show();
    return a.exec();
}


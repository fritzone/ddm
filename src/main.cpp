#include <QtGui/QApplication>

#include "Workspace.h"
#include "MainWindow.h"

#include <QSqlDatabase>
#include <QMessageBox>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("The Unauthorized Frog");
    QApplication::setApplicationName("ddm");
    QApplication::setApplicationVersion("0.1");

    QString s = QSqlDatabase::drivers().join(" ");

 //   QMessageBox::information(0, "a", s, QMessageBox::Ok);

    QObject::connect(&a, SIGNAL(aboutToQuit()), Workspace::getInstance(), SLOT(cleanup()));

    MainWindow w;
    w.show();
    return a.exec();
}

#ifndef DYNAMICACTIONHANDLERFORMAINWINDOW_H
#define DYNAMICACTIONHANDLERFORMAINWINDOW_H

#include <QObject>
#include <QAction>

#include "MainWindow.h"

class DynamicActionHandlerforMainWindow : public QObject
{
    Q_OBJECT

public:

    DynamicActionHandlerforMainWindow(const QString& a, MainWindow* w, MainWindow::dynamicAction);


public slots:

    void called();

private:

    QString actionName;
    MainWindow* mainWindow;
    MainWindow::dynamicAction callMe;

};


#endif // DYNAMICACTIONHANDLERFORMAINWINDOW_H

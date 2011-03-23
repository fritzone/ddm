#ifndef DYNAMICACTIONHANDLERFORMAINWINDOW_H
#define DYNAMICACTIONHANDLERFORMAINWINDOW_H

#include <QObject>
#include <QAction>

class MainWindow;

class DynamicActionHandlerforMainWindow : public QObject
{
    Q_OBJECT

public:

    DynamicActionHandlerforMainWindow(const QString& a, MainWindow* w);


public slots:

    void called();

private:

    QString actionName;
    MainWindow* mainWindow;

};


#endif // DYNAMICACTIONHANDLERFORMAINWINDOW_H

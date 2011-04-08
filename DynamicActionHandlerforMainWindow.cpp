#include "DynamicActionHandlerForMainWindow.h"
#include "MainWindow.h"

DynamicActionHandlerforMainWindow::DynamicActionHandlerforMainWindow(const QString &a, MainWindow *w) : actionName(a), mainWindow(w)
{}

void DynamicActionHandlerforMainWindow::called()
{
    mainWindow->instantiateTable(actionName, QStringList());
}


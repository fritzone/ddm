#include "DynamicActionHandlerForMainWindow.h"
#include "MainWindow.h"

DynamicActionHandlerforMainWindow::DynamicActionHandlerforMainWindow(const QString &a, MainWindow *w, MainWindow::dynamicAction as) : actionName(a), mainWindow(w), callMe(as)
{}

void DynamicActionHandlerforMainWindow::called()
{
    (mainWindow->*callMe)(actionName);
}


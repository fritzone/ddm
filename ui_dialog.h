/********************************************************************************
** Form generated from reading ui file 'dialog.ui'
**
** Created: Wed Nov 25 14:13:35 2009
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_StartupDialog
{
public:

    void setupUi(QDialog *StartupDialog)
    {
        if (StartupDialog->objectName().isEmpty())
            StartupDialog->setObjectName(QString::fromUtf8("StartupDialog"));
        StartupDialog->resize(339, 301);

        retranslateUi(StartupDialog);

        QMetaObject::connectSlotsByName(StartupDialog);
    } // setupUi

    void retranslateUi(QDialog *StartupDialog)
    {
        StartupDialog->setWindowTitle(QApplication::translate("StartupDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(StartupDialog);
    } // retranslateUi

};

namespace Ui {
    class StartupDialog: public Ui_StartupDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H

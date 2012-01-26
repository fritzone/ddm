#ifndef MAINWINDOWBUTTONDIALOG_H
#define MAINWINDOWBUTTONDIALOG_H

#include <QtGui/QDialog>
#include <QCommandLinkButton>

namespace Ui {
    class MainWindowButtonDialog;
}

class MainWindowButtonDialog : public QDialog {
    Q_OBJECT
public:
    MainWindowButtonDialog(QWidget *parent = 0);
    ~MainWindowButtonDialog();

public slots:

    // when the user clicked the "new" button
    void onNewProject();
    void onCloseWindow();
    void onOpenProject();
    void onNewConnection();
    void onHelp();

private slots:
    void onQuick1();
    void onQuick2();
    void onQuick3();
    void onQuick4();
    void onQuick5();
    void onBtnClose();

protected:
    void changeEvent(QEvent *e);

private:

    void onQuick(const QString&);
    void fixButton(QCommandLinkButton*, const QString&);

private:
    Ui::MainWindowButtonDialog *m_ui;
};

#endif // MAINWINDOWBUTTONDIALOG_H

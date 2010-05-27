#ifndef MAINWINDOWBUTTONDIALOG_H
#define MAINWINDOWBUTTONDIALOG_H

#include <QtGui/QDialog>

class MainWindow;

namespace Ui {
    class MainWindowButtonDialog;
}

class MainWindowButtonDialog : public QDialog {
    Q_OBJECT
public:
    MainWindowButtonDialog(QWidget *parent = 0);
    ~MainWindowButtonDialog();
    void setMainWindow(MainWindow*);

public slots:

    // when the user clicked the "new" button
    void onNewProject();


protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindowButtonDialog *m_ui;
    MainWindow* m_mw;

};

#endif // MAINWINDOWBUTTONDIALOG_H

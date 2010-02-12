#ifndef NEWTABLEFORM_H
#define NEWTABLEFORM_H

#include <QWidget>

namespace Ui {
    class NewTableForm;
}

class MainWindow;
class DatabaseEngine;
class Project;

class NewTableForm : public QWidget {
    Q_OBJECT
public:
    NewTableForm(DatabaseEngine* engine, Project* prj, QWidget *parent = 0);
    ~NewTableForm();
    void setMainWindow(MainWindow* mw);
    void focusOnName();

public slots:

    void onAddColumn();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::NewTableForm *m_ui;
    MainWindow* m_mw;
    DatabaseEngine* m_dbEngine;
    Project* m_project;
};

#endif // NEWTABLEFORM_H

#ifndef NEWTABLEFORM_H
#define NEWTABLEFORM_H

#include <QWidget>

namespace Ui {
    class NewTableForm;
}

class MainWindow;
class DatabaseEngine;

class NewTableForm : public QWidget {
    Q_OBJECT
public:
    NewTableForm(DatabaseEngine* engine, QWidget *parent = 0);
    ~NewTableForm();
    void setMainWindow(MainWindow* mw);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::NewTableForm *ui;
    MainWindow* m_mw;
    DatabaseEngine* m_dbEngine;
};

#endif // NEWTABLEFORM_H

#ifndef INJECTSQLDIALOG_H
#define INJECTSQLDIALOG_H

#include <QDialog>

namespace Ui {
    class InjectSqlDialog;
}

class InjectSqlDialog : public QDialog
{
    Q_OBJECT
public:
    InjectSqlDialog(QWidget *parent = 0);
    ~InjectSqlDialog();

public slots:

    void onConnect();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::InjectSqlDialog *ui;
};

#endif // INJECTSQLDIALOG_H

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

    QString getDatabase();

    QString getUser();

    QString getPassword();

    QString getHost();

public slots:

    void onConnect();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::InjectSqlDialog *ui;
};

#endif // INJECTSQLDIALOG_H

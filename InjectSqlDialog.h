#ifndef INJECTSQLDIALOG_H
#define INJECTSQLDIALOG_H

#include <QDialog>

class DatabaseEngine;

namespace Ui
{
    class InjectSqlDialog;
}

class InjectSqlDialog : public QDialog
{
    Q_OBJECT
public:
    InjectSqlDialog(DatabaseEngine* engine, QWidget *parent = 0);
    ~InjectSqlDialog();

    QString getDatabase();

    QString getUser();

    QString getPassword();

    QString getHost();

    QString getCodepage();

    bool getRollbackOnError();

    bool getCreateOnlyIfNotExist();

    void setupForReverseEngineering();


    // loads the codepages from the codepages
    void populateCodepageCombo();

public slots:

    void onConnect();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::InjectSqlDialog *ui;
    DatabaseEngine* m_dbEngine;

private:

    static QString previousHost;
    static QString previousUser;
};

#endif // INJECTSQLDIALOG_H

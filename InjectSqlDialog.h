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

    QString getDatabase() const;
    QString getUser() const;
    QString getPassword() const;
    QString getHost() const;
    QString getName() const;
    QString getCodepage() const;
    bool getRollbackOnError() const;
    bool getAutoConnect() const;
    bool getCreateOnlyIfNotExist() const;
    void setupForReverseEngineering();
    void setupForConnectionStorage();

    // loads the codepages from the codepages
    void populateCodepageCombo();

    void selectCodePage(int);

public slots:

    void onConnect();
    void onCreateDatabase();

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

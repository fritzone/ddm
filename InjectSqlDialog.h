#ifndef INJECTSQLDIALOG_H
#define INJECTSQLDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

class DatabaseEngine;
class Connection;


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
    QStringList getSelectedConnections() const;

    QString getCodepage() const;
    bool getRollbackOnError() const;
    bool getAutoConnect() const;

    void setupForReverseEngineering();
    void setupForConnectionStorage();

    // loads the codepages from the codepages
    void populateCodepageCombo();

    void selectCodePage(int);
    void populateConnectionDetails(Connection* c);
    void clearConnectionDetails();

public slots:

    void onConnect();
    void onCreateDatabase();
    void onSelectConnection(QListWidgetItem*);
    void onHostChange(QString);
    void onUserChange(QString);

protected:
    void changeEvent(QEvent *e);

private:

    void populateConnections();

private:
    Ui::InjectSqlDialog *ui;
    DatabaseEngine* m_dbEngine;
    bool m_nameWasChanged;

private:

    static QString previousHost;
    static QString previousUser;
};

#endif // INJECTSQLDIALOG_H

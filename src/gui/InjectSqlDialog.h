#ifndef INJECTSQLDIALOG_H
#define INJECTSQLDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QSignalMapper>
#include <QTreeWidgetItem>

class DatabaseEngine;
class Connection;
class Version;

namespace Ui
{
    class InjectSqlDialog;
}

class InjectSqlDialog : public QDialog
{
    Q_OBJECT

    enum Mode
    {
        MODE_MYSQL = 0,
        MODE_SQLITE = 1,
        MODE_CUBRID = 2,
        MODE_POSTGRES = 4
    };

public:
    InjectSqlDialog(DatabaseEngine* engine, QWidget *parent, Version* v, const QString& objNameToDeploy);
    ~InjectSqlDialog();

    QString getDatabase() const;

    // ugly, and returns the SQLITE file name
    QString getFileName() const;
    QString getUser() const;
    QString getPassword() const;
    QString getHost() const;
    QString getName() const;
    int getPort() const;

    QStringList getSelectedConnections() const;

    bool getRollbackOnError() const;
    bool getAutoConnect() const;
    bool injectMetadataRequired() const;

    void setupForReverseEngineering();
    void setupForConnectionStorage();
    void setupForBindToDatabase();

    void populateConnectionDetails(Connection* conn);
    void clearConnectionDetails();

    QStringList getUidsToDeploy() const
    {
        return m_UidsToDeploy;
    }

    QStringList getUidsToDrop() const
    {
        return m_UidsToDrop;
    }

    QString getSDbEngine() const
    {
        return m_strDbEngine;
    }

    int getSqliteVersion() const;

    Connection* provideConnection();

private slots:

    void onConnect();
    void onCreateDatabase();
    void onSelectConnection(QListWidgetItem*);
    void onHostChange(QString);
    void onUserChange(QString);
    void checkBoxToggled(QString);
    void onDbChange(QString);
    void onConnectionNameEdited(QString);
    void onDbTypeChange(QString);
    void onSelectFileForSqlite();
    void onSqliteFileNameChange(QString);
    void onTestConnection();
    void onHelp();

protected:
    void changeEvent(QEvent *e);

private:

    void populateConnections();
    void createTreeItem(QTreeWidgetItem* parent, const QString& text, const QString& uid, const QIcon& icon);
    void toggleUidBelongeness(QStringList&, const QString&);

    // various layouts for various databases
    // TODO: this feels ugly, re-think
    void setSqliteLayout();
    void setMysqlLayout();
    void setCUBRIDLayout();
    void setPostgresLayout();

    void disableOkButton();
    void enableOkButton();

private:
    Ui::InjectSqlDialog *ui;
    DatabaseEngine* m_dbEngine;
    bool m_nameWasChanged;
    bool m_injectMetadata;
    QSignalMapper* m_signalMapper;
    QStringList m_UidsToDeploy;
    QStringList m_UidsToDrop;
    QString m_objName;
    bool m_alreadyConnected;
    bool m_cubridTested;
    QString m_strDbEngine;
    InjectSqlDialog::Mode m_mode;

private:

    static QString previousHost;
    static QString previousUser;
};

#endif // INJECTSQLDIALOG_H

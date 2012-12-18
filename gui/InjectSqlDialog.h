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
public:
    InjectSqlDialog(DatabaseEngine* engine, QWidget *parent, Version* v, const QString& objNameToDeploy);
    ~InjectSqlDialog();

    QString getDatabase() const;
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

    void populateConnectionDetails(Connection* c);
    void clearConnectionDetails();

    QStringList getUidsToDeploy() const
    {
        return m_UidsToDeploy;
    }

    QStringList getUidsToDrop() const
    {
        return m_UidsToDrop;
    }

private slots:

    void onConnect();
    void changeConnection();
    void onCreateDatabase();
    void onSelectConnection(QListWidgetItem*);
    void onHostChange(QString);
    void onUserChange(QString);
    void checkBoxToggled(QString);

protected:
    void changeEvent(QEvent *e);

private:

    void populateConnections();
    void createTreeItem(QTreeWidgetItem* parent, const QString& text, const QString& uid, const QIcon& icon);
    void toggleUidBelongeness(QStringList&, const QString&);

private:
    Ui::InjectSqlDialog *ui;
    DatabaseEngine* m_dbEngine;
    bool m_nameWasChanged;
    bool m_injectMetadata;
    QSignalMapper* m_signalMapper;
    QStringList m_UidsToDeploy;
    QStringList m_UidsToDrop;
    QString m_objName;

private:

    static QString previousHost;
    static QString previousUser;
};

#endif // INJECTSQLDIALOG_H

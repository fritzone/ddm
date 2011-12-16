#ifndef CORE_CONNECTION_H
#define CORE_CONNECTION_H

#include "SerializableElement.h"
#include "NamedItem.h"
#include "TreeItem.h"
#include "IssueOriginator.h"

#include <QString>
#include <QSqlDatabase>

class DatabaseEngine;
class Table;

class Connection : virtual public SerializableElement, virtual public TreeItem, virtual public IssueOriginator
{
public:

    enum ConnectionState
    {
        CONNECTED=1,
        FAILED=2,
        DID_NOT_TRY=3
    };

    Connection(const QString& name, const QString& host, const QString& user, const QString& pass, const QString& db, bool savePw, bool autoConnect);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual Table* getIssueTable() const
    {
        return 0;
    }
    virtual QString getFullLocation() const
    {
        return m_db+"@"+m_host;
    }

    QString getHost() const
    {
        return m_host;
    }
    QString getDb() const
    {
        return m_db;
    }

    bool tryConnect();

    ConnectionState getState() const
    {
        return m_state;
    }
    QString getUser() const
    {
        return m_user;
    }
    QString getPassword() const
    {
        return m_pass;
    }
    void resetTo(const QString& name, const QString& host, const QString& user, const QString& pass, const QString& db, bool savePw, bool autoConnect);
    DatabaseEngine* getEngine() const
    {
        return m_engine;
    }
    QSqlDatabase getQSqlDatabase();
    QStringList getTables()
    {
        return m_tables;
    }

    void addTable(const QString& tabName)
    {
        m_tables.append(tabName);
    }


private:

    QString m_host;
    QString m_user;
    QString m_pass;
    QString m_db;
    QString m_dbType;
    bool m_savePw;
    bool m_autoConnect;
    DatabaseEngine* m_engine;
    ConnectionState m_state;
    QStringList m_tables;

};

#endif // CORE_CONNECTION_H

#ifndef CORE_CONNECTION_H
#define CORE_CONNECTION_H

#include "SerializableElement.h"
#include "NamedItem.h"
#include "TreeItem.h"
#include "IssueOriginator.h"
#include "core_enums.h"

#include <QString>
#include <QSqlDatabase>

class DatabaseEngine;
class Table;

class Connection :
        public SerializableElement,
        public IssueOriginator,
        public NamedItem,
        public TreeItem
{
public:

    Connection(const QString& name, const QString& host, const QString& user, const QString& pass, const QString& db, bool savePw, bool autoConnect, int port);
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

    void setState(ConnectionState s)
    {
        m_state = s;
    }

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
    void resetTo(const QString& name, const QString& host, const QString& user, const QString& pass, const QString& db, int port, bool savePw, bool autoConnect);
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

    void clearTables()
    {
        m_tables.clear();
    }

    QString getDbType() const
    {
        return m_dbType;
    }

    bool getAC() const
    {
        return m_autoConnect;
    }

    int getPort() const
    {
        return m_port;
    }

private:

    QString m_host;
    QString m_user;
    QString m_pass;
    QString m_db;
    QString m_dbType;
    int m_port;
    bool m_savePw;
    bool m_autoConnect;
    DatabaseEngine* m_engine;
    ConnectionState m_state;
    QStringList m_tables;

};

#endif // CORE_CONNECTION_H

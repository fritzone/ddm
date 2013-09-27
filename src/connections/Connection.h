#ifndef CORE_CONNECTION_H
#define CORE_CONNECTION_H

#include "core_SerializableElement.h"
#include "core_NamedItem.h"
#include "TreeItem.h"
#include "IssueOriginator.h"
#include "core_enums.h"

#include <QString>
#include <QSqlDatabase>
#include <QSettings>

class DatabaseEngine;
class Table;

class Connection :
        public SerializableElement,
        public IssueOriginator,
        public NamedItem,
        public TreeItem
{
public:

    Connection(const QString& name, bool autoConnect);

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;
    virtual QString getFullLocation() const = 0;
    virtual void saveIntoSettings(QSettings& s) = 0;
    virtual QString getDbType() const = 0;
    virtual Table* getIssueTable() const { return 0; }
    virtual QString getDb() const
    {
        return "";
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

    bool getAC() const
    {
        return m_autoConnect;
    }

    QString getLastError() const
    {
        return m_lastError;
    }

protected:

    bool m_autoConnect;
    DatabaseEngine* m_engine;
    ConnectionState m_state;
    QStringList m_tables;
    QString m_lastError;
};

#endif // CORE_CONNECTION_H

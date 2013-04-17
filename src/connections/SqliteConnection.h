#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H

#include "Connection.h"

class SqliteConnection : public Connection
{
public:

    SqliteConnection(const QString& name, const QString& filename, bool autoConnect, int sqliteVersion);

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual void saveIntoSettings(QSettings& s);
    virtual QString getFullLocation() const;
    virtual QString getDbType() const
    {
        return "Sqlite";
    }

    virtual Table* getIssueTable() const { return 0; }


    void resetTo(const QString& fileName)
    {
        m_fileName = fileName;
    }

    QString getFileName() const
    {
        return m_fileName;
    }

    int getVersion() const
    {
        return m_sqliteVersion;
    }

private:
    QString m_fileName;
    int m_sqliteVersion;
};

#endif // SQLITECONNECTION_H

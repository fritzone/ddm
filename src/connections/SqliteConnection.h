#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H

#include "Connection.h"

class SqliteConnection : public Connection
{
public:

    SqliteConnection(const QString& name, const QString& filename, bool autoConnect);

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

private:
    QString m_fileName;
};

#endif // SQLITECONNECTION_H

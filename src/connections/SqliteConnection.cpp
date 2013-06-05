#include "SqliteConnection.h"
#include "strings.h"
#include "db_DatabaseEngineManager.h"

SqliteConnection::SqliteConnection(const QString &name, const QString &filename, bool autoConnect, int sqliteVersion) : Connection(name, autoConnect),
    m_fileName(filename), m_sqliteVersion(sqliteVersion)
{
    qDebug() << "Name=" << name;
    m_engine = DatabaseEngineManager::instance().engine(strSqlite);
}

void SqliteConnection::serialize(QDomDocument& /*doc*/, QDomElement& /*parent*/) const
{

}

void SqliteConnection::saveIntoSettings(QSettings& s)
{
    s.setValue(strName, getName());
    s.setValue(strFile, getFileName());
    s.setValue("Version", m_sqliteVersion);
}


QString SqliteConnection::getFullLocation() const
{
    return m_fileName;
}


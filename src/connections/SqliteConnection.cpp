#include "SqliteConnection.h"
#include "strings.h"

SqliteConnection::SqliteConnection(const QString &name, const QString &filename, bool autoConnect) : Connection(name, autoConnect),
    m_fileName(filename)
{
}

void SqliteConnection::serialize(QDomDocument& doc, QDomElement& parent) const
{

}

void SqliteConnection::saveIntoSettings(QSettings& s)
{
    s.setValue(strName, getName());
    s.setValue(strFile, getFileName());
}


QString SqliteConnection::getFullLocation() const
{
    return m_fileName;
}


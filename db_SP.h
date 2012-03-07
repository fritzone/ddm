#ifndef DB_SP_H
#define DB_SP_H

#include "core_ObjectWithUid.h"
#include "NamedItem.h"

#include <QString>
#include <QUuid>

class SpInstance;

/**
 * This class represents a Special Property of a database object (Table, Column, Index, ...)
 * which will be:
 * 1. Presented in the GUI for each Database in a specific Tab
 * 2. The value from above will be inserted in the SQL query
 * Each Sp object has a property specifying where to put it in the query.
 */
class Sp : public ObjectWithUid, public NamedItem
{
public:

    Sp(const QString& sqlPropertyUid, const QString& dbObjectUid, const QString& propertyName, const QString& propertyGuiText);

    QString getDbObjectUid() const
    {
        return m_destinationUid;
    }

    QString getSqlPropertyUid() const
    {
        return m_sqlPropertyUid;
    }

    QString getPropertyGuiText() const
    {
        return m_propertyGuiText;
    }

    virtual SpInstance* instantiate() = 0;

    virtual QUuid getClassUid() const = 0;

private:

    // this tells the sql query generator where to insert this property in the SQL query
    QUuid m_sqlPropertyUid;

    // this tells the app where to put this object in the GUI
    QUuid m_destinationUid;

    // the property's text for the GUI
    QString m_propertyGuiText;
};

#endif // DB_SP_H

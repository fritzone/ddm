#ifndef DB_SP_H
#define DB_SP_H

#include "core_ObjectWithUid.h"
#include "NamedItem.h"

#include <QString>
#include <QUuid>

class SpInstance;

/**
 * This class represents a Special Property of a database object (Table, Column, Index, ...)
 * which will be presented in the GUI for each Database in a specific Tab
 * When creating an object With Sps the instantiate method os being called to create the instances
 * of the SPs for the given object.
 * Each Sp object has a property specifying where to put it in the query.
 */
class Sp : public ObjectWithUid, public NamedItem
{
public:

    Sp(const QString& sqlRoleUid, const QString& guiClassUid, const QString& referringObjectClassUid, const QString& propertyName, const QString& propertyGuiText);

    QString getDbObjectUid() const
    {
        return m_guiClassUid;
    }

    QString getSqlRoleUid() const
    {
        return m_sqlRoleUid;
    }

    QString getPropertyGuiText() const
    {
        return m_propertyGuiText;
    }

    virtual SpInstance* instantiate() = 0;

    /**
     * Returns the class UID of this. The class UID is NOT the one identifying the object in the GUI.
     * This is a normal class UID.
     */
    virtual QUuid getClassUid() const = 0;

    /**
     * This UID is the same uid as the object this SP refers to. For example this should return Table UID
     * for properties referring to a table.
     */
    virtual QUuid getReferredObjectClassUid() const
    {
        return m_referredObjectClasUid;
    }

private:

    // this tells the sql query generator where to insert this property in the SQL query
    QUuid m_sqlRoleUid;

    // this tells the app where to put this object in the GUI, ie. to which component this is referring
    QUuid m_guiClassUid;

    // this is the class UID of the "class" this SP ois referring to. For example: Table -> uidTable
    QUuid m_referredObjectClasUid;

    // the property's text for the GUI
    QString m_propertyGuiText;
};

#endif // DB_SP_H

#ifndef DB_SP_H
#define DB_SP_H

#include "core_ObjectWithUid.h"
#include "NamedItem.h"
#include "SerializableElement.h"

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
class Sp : public ObjectWithUid, public NamedItem, public SerializableElement
{
public:

    /**
     * @param sqlRoleUid - the exact uid of the property, such as uidMysqlTemporaryTable represents the TEMPORARY property of the mysql table.
     * @param referringObjectClassUid - the uid of the "class" this Sp is referring to. Such as "Table" -> uidTable
     * @param propertyName - the name of the property
     * @param propertyGuiText - the text that will be shown on the GUI for this property
     */
    Sp(const QString& sqlRoleUid, const QString& referringObjectClassUid, const QString& propertyName, const QString& propertyGuiText);

    QString getSqlRoleUid() const
    {
        return m_sqlRoleUid;
    }

    QString getPropertyGuiText() const
    {
        return m_propertyGuiText;
    }

    /**
     * This UID is the same uid as the object this SP refers to. For example this should return Table UID
     * for properties referring to a table.
     */
    QUuid getReferredObjectClassUid() const
    {
        return m_referredObjectClasUid;
    }

    virtual SpInstance* instantiate(const Sp*) = 0;

    /**
     * Returns the class UID of this. The class UID is NOT the one identifying the object in the GUI.
     * This is a normal class UID.
     */
    virtual QUuid getClassUid() const = 0;


    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

private:

    // this tells the sql query generator where to insert this property in the SQL query
    QUuid m_sqlRoleUid;
    // this is the class UID of the "class" this SP ois referring to. For example: Table -> uidTable
    QUuid m_referredObjectClasUid;

    // the property's text for the GUI
    QString m_propertyGuiText;
};

#endif // DB_SP_H

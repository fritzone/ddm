#ifndef USERDATATYPE_H
#define USERDATATYPE_H

#include <QtXml>

#include "TreeItem.h"
#include "SerializableElement.h"
#include "IconFactory.h"
#include "NamedItem.h"
#include "core_CloneableElement.h"
#include "core_ObjectWithUid.h"

/**
 * Class representing a data type
 */
class UserDataType : public NamedItem, public TreeItem, public SerializableElement, public ObjectWithUid, public CloneableElement
{
public:

    UserDataType(const QString& name, DT_TYPE type, const QString& uid) :
        NamedItem(name),
        TreeItem(),
        SerializableElement(),
        ObjectWithUid(uid),
        CloneableElement(),
        sqlType(""),
        size(""), defaultValue(""), miscStuff(),
        unsignedDT(false), description(""), canBeNull(true), m_type(type)
    {}


    virtual ~UserDataType()
    {}

    UserDataType(const QString& name, const QString& typeString,
                 const QString& _sqlType, const QString& _s,
                 const QString& _defaultValue,
                 const QStringList& _mvs, bool unsi, const QString& desc,
                 bool nullable, const QString& uid);

    UserDataType& operator = (const UserDataType& other);

    // the size of this user defiend data type as a nice string
    QString sizeAsString() const;

    QString sqlAsString() const
    {
        return sqlType + (size>0?"(" + size + ")":"");
    }

    const QString& getSqlType() const
    {
        return sqlType;
    }

    const QString& getSize() const
    {
        return size;
    }

    bool isUnsigned() const
    {
        return unsignedDT;
    }

    const QString getDefaultValue() const
    {
        return defaultValue ;
    }

    const QStringList& getMiscValues() const
    {
        return miscStuff;
    }

    const QString& getDescription() const
    {
        return description;
    }

    bool isNullable() const
    {
        return canBeNull;
    }

    /**
     * Serializes this UserDataType to be saved in the final XML.
     * Modifies the doc object, this is why it's passed in by reference.
     * XML will look like:
     * <DataType Name = NAME, Type=DT_TYPE, SqlType=SQL_TYPE, Size=SIZE, DefaultValue=DEFAULT_VALUE, Unsigned=0/1, CanBeNull=0/1>
     *  <Description> Description </Description>
     *  <Values>
     *   <Value> VALUE </Value>
     *  </Values>
     * </DataType>
     */
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

    virtual QWidget* getDefaultsTableWidget() const;

    /**
     * Checks if the passed in value (from the table instance default values) is valid for this data type.
     * TODO: This design is really ugly, we should have string, numeric, other inheriting this, and those should have this method. Rewrite when there's some time.
     */
    bool isValid(const QString&) const;


    QString typeAsString() const;

    bool supportsAutoIncrement() const;


    DT_TYPE getType() const
    {
        return m_type;
    }

    static DT_TYPE getDT_TYPE(const QString& typeString);

    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);

    virtual QUuid getClassUid() const;
    virtual void updateGui();


private:

    // the SQL type of this Data Type
    QString sqlType;

    // the size of this Data Type
    QString size;

    // the default value for this UDT
    QString defaultValue;

    // the stuff for the ENUMs, SETs, etcs...
    QStringList miscStuff;

    // if the data type is numeric and this will be unsigned
    bool unsignedDT;

    QString description;

    bool canBeNull;

    DT_TYPE m_type;


};

#endif // USERDATATYPE_H

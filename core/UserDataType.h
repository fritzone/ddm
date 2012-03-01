#ifndef USERDATATYPE_H
#define USERDATATYPE_H

#include <QtXml>

#include "DataType.h"
#include "TreeItem.h"
#include "SerializableElement.h"
#include "IconFactory.h"
#include "NamedItem.h"

/**
 * Class representing a data type
 */
class UserDataType : virtual public NamedItem, virtual public TreeItem, virtual public SerializableElement
{
public:

    UserDataType():NamedItem(""), sqlType(""),
        size(""), defaultValue(""), miscStuff(), codePage(""),
        unsignedDT(false), description(""), canBeNull(true), autoIncrement(false), m_type(DT_INVALID)
    {}

    UserDataType(const QString& name, DT_TYPE type):NamedItem(name), sqlType(""),
        size(""), defaultValue(""), miscStuff(), codePage(""),
        unsignedDT(false), description(""), canBeNull(true), autoIncrement(false), m_type(type)
    {}

    UserDataType(const UserDataType& other): NamedItem(other.getName()), TreeItem(), SerializableElement(), DataType(other.name, other.type),
    sqlType(other.sqlType), size(other.size),
    defaultValue(other.defaultValue), miscStuff(other.miscStuff),
    codePage(other.codePage),unsignedDT(other.unsignedDT),
    description(other.description), canBeNull(other.canBeNull), autoIncrement(other.autoIncrement)
    {}

    ~UserDataType()
    {}

    UserDataType(const QString& name, const QString& typeString,
                 const QString& _sqlType, const QString& _s,
                 const QString& _defaultValue, const QString& _cp,
                 const QStringList& _mvs, bool unsi, const QString& desc,
                 bool nullable, bool autoInc);

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

    const QString& getCodepage() const
    {
        return codePage;
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
     * <DataType Name = NAME, Type=DT_TYPE, SqlType=SQL_TYPE, Size=SIZE, DefaultValue=DEFAULT_VALUE, Codepage=CODEPAGE, Unsigned=0/1, CanBeNull=0/1 AutoIncrement=0/1>
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
        return type;
    }

    static DT_TYPE getDT_TYPE(const QString& typeString);

private:

    // the SQL type of this Data Type
    QString sqlType;

    // the size of this Data Type
    QString size;

    // the default value for this UDT
    QString defaultValue;

    // the stuff for the ENUMs, SETs, etcs...
    QStringList miscStuff;

    // the codepage (if any)
    QString codePage;

    // if the data type is numeric and this will be unsigned
    bool unsignedDT;

    QString description;

    bool canBeNull;

    // if the data type is numeric it can be auto increment or not ...
    bool autoIncrement;

    DT_TYPE m_type;


};

#endif // USERDATATYPE_H

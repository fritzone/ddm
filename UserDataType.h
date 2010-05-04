#ifndef USERDATATYPE_H
#define USERDATATYPE_H

#include <QMetaType>
#include <QtXml>

#include "DataType.h"
#include "TreeItem.h"
#include "SerializableElement.h"

/**
 * Class representing a data type
 */
class UserDataType : public DataType, virtual public TreeItem, public SerializableElement
{
public:

    UserDataType():DataType("", DataType::DT_INVALID), sqlType(""),
    icon(0), size(""), defaultValue(""), miscStuff(), codePage(""),
    unsignedDT(false)
    {}

    UserDataType(const UserDataType& other):DataType(other.name, other.type),
    sqlType(other.sqlType), icon(other.icon), size(other.size),
    defaultValue(other.defaultValue), miscStuff(other.miscStuff),
    codePage(other.codePage),unsignedDT(other.unsignedDT)
    {}

    ~UserDataType()
    {}

    UserDataType(const QString& name, const QString& typeString,
                 const QString& _sqlType, const QString& _s,
                 const QString& _defaultValue, const QString& _cp,
                 const QStringList& _mvs, bool unsi);

    UserDataType& operator = (const UserDataType& other);

    // returns the icon
    const QIcon& getIcon() const
    {
        //assert(icon);
        return *icon;
    }

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

    const QString& getDefaultValue() const
    {
        return defaultValue;
    }

    const QStringList& getMiscValues() const
    {
        return miscStuff;
    }

    /**
     * Serializes this UserDataType to be saved in the final XML.
     * Modifies the doc object, this is why it's passed in by reference.
     * XML will look like:
     * <DataType>
     *  <Name> NAME </Name>
     *  <Type> DT_TYPE </Type> -- From the DataType.h
     *  <SQLType> SQL_TYPE </SQLType>
     *  <Size> SIZE </Size>
     *  <DefaultValue> DEFAULT_VALUE </DefaultValue>
     *  <CodePage> CODEPAGE </CodePage>
     *  <Unsigned> YES/NO </Unsigned>
     *  <Values>                -- In case this is ENUM or SET or anything
     *   <Value> VALUE </Value>
     *  </Values>
     * </DataType>
     */
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;


private:

    // the SQL type of this Data Type
    QString sqlType;

    // the icon of this data type
    const QIcon* icon;

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

};

Q_DECLARE_METATYPE(UserDataType)

#endif // USERDATATYPE_H

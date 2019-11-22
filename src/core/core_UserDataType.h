#ifndef USERDATATYPE_H
#define USERDATATYPE_H

#include "TreeItem.h"
#include "core_SerializableElement.h"
#include "IconFactory.h"
#include "core_NamedItem.h"
#include "core_CloneableElement.h"
#include "core_ObjectWithUid.h"
#include "core_VersionElement.h"

/**
 * Class representing a data type
 */
class UserDataType : public NamedItem,
        public SerializableElement,
        public VersionElement,
        public CloneableElement,
        public TreeItem
{
public:

    UserDataType(const QString& name, DT_TYPE type, const QString& uid, Version *v) :
        NamedItem(name),
        SerializableElement(),
        VersionElement(uid, v),
        CloneableElement(),
        m_sqlType(""),
        m_size(""), m_defaultValue(""), m_miscStuff(),
        m_unsigned(false), m_description(""), m_canBeNull(true), m_type(type)
    {}


    virtual ~UserDataType()
    {}

    UserDataType(const QString& name, const QString& typeString,
                 const QString& _sqlType, const QString& _s,
                 const QString& _defaultValue,
                 const QStringList& _mvs, bool unsi, const QString& desc,
                 bool nullable, const QString& uid, Version *v);

    UserDataType& operator = (const UserDataType& other);

    // the size of this user defiend data type as a nice string
    QString sizeAsString() const;

    QString sqlAsString() const
    {
        return m_sqlType + (m_size>0?"(" + m_size + ")":"");
    }

    const QString& getSqlType() const
    {
        return m_sqlType;
    }

    const QString& getSize() const
    {
        return m_size;
    }

    bool isUnsigned() const
    {
        return m_unsigned;
    }

    const QString getDefaultValue() const
    {
        return m_defaultValue ;
    }

    const QStringList& getMiscValues() const
    {
        return m_miscStuff;
    }

    const QString& getDescription() const
    {
        return m_description;
    }

    bool isNullable() const
    {
        return m_canBeNull;
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
    static DT_TYPE toDtType(int);
    static QString getTypeStringFromDT_TYPE(DT_TYPE type);

    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);

    virtual QUuid getClassUid() const;
    virtual void updateGui();
    void setSqlType(const QString& t)
    {
        m_sqlType = t;
    }

    void setSize(int s)
    {
        m_size = QString::number(s);
    }

private:

    // the SQL type of this Data Type
    QString m_sqlType;

    // the size of this Data Type
    QString m_size;

    // the default value for this UDT
    QString m_defaultValue;

    // the stuff for the ENUMs, SETs, etcs...
    QStringList m_miscStuff;

    // if the data type is numeric and this will be unsigned
    bool m_unsigned;

    QString m_description;

    bool m_canBeNull;

    DT_TYPE m_type;


};

#endif // USERDATATYPE_H

#ifndef CORE_STOREDMETHOD_H
#define CORE_STOREDMETHOD_H

#include "core_NamedItem.h"
#include "SqlSourceTreeItem.h"
#include "TreeItem.h"
#include "core_SerializableElement.h"
#include "core_CloneableElement.h"
#include "core_ObjectWithUid.h"
#include "core_SqlSource.h"
#include "core_ParameterAndDescription.h"
#include "core_VersionElement.h"

class StoredMethod :
        public NamedItem,
        public SerializableElement,
        public CloneableElement,
        public VersionElement,
        public SqlSource,
        public TreeItem,
        public SqlSourceTreeItem
{
public:

    /**
     * Returns the keyword which is used in the SQL generation of this stored method (ie: FUNCTION/PROCEDURE)
     * @return the keyword used in SQL generation
     */
    virtual QString keyword() = 0;

    /**
     * @brief serialize
     * @param doc
     * @param parent
     * @see SerializableElement::serialize
     */
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;

    /**
     * @brief clone
     * @param sourceVersion
     * @param targetVersion
     * @return
     */
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion) = 0;

public:

    StoredMethod(const QString& name, const QString& uid, Version *v, bool guided);

    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const Connection*);

    void setSql(const QString&);
    QString getSql() const;
    QVector<ParameterAndDescription*> getParametersWithDescription();
    QString getBriefDescription() const
    {
        return m_brief;
    }
    QString getNameFromSql(int stidx, int& nameidx);
    QString getDescription() const
    {
        return m_desc;
    }
    QString getReturnDesc() const
    {
        return m_returnDesc;
    }

    bool isGuided() const
    {
        return m_guidedCreation;
    }

    void setBriefDescr(const QString& a)
    {
        m_brief = a;
    }

    void setDescription(const QString& a)
    {
        m_desc= a;
    }

    void setReturn(const QString& a)
    {
        m_returns = a;
    }

    QString getReturnType() const
    {
        return m_returns;
    }

    bool hasParameter(const QString& name);
    ParameterAndDescription* getParameter(const QString& name);
    void addParameter(ParameterAndDescription* pd)
    {
        m_guidedParameters.append(pd);
    }

    void removeParameter(const QString& name);
    void moveUpParameter(int);
    void moveDownParameter(int);

    virtual QString getSqlHash() const;
    virtual void rename(const QString&);

    // will serialize the parameters if this is a guided method
    void serialize_parameters(QDomDocument& doc, QDomElement& parent) const;

    void setParameters(const QVector<ParameterAndDescription*>& pads)
    {
        m_guidedParameters = pads;
    }

protected:

    QString m_sql;

    // these are populated automatically by the documentation generator
    // these are serialized only if it's guided
    QString m_brief;
    QString m_desc;
    QString m_returns;
    QString m_returnDesc;
    bool m_guidedCreation;

    // this is populated only if the method was created with the guide
    QVector<ParameterAndDescription*> m_guidedParameters;
};

#endif // CORE_STOREDMETHOD_H

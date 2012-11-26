#ifndef TABLEINSTANCE_H
#define TABLEINSTANCE_H

#include "core_Table.h"
#include "TreeItem.h"
#include "NamedItem.h"
#include "SerializableElement.h"
#include "SqlSourceEntity.h"
#include "core_ObjectWithUid.h"
#include "core_CloneableElement.h"
#include "core_ColumnWithValue.h"

#include <QHash>
#include <QList>
#include <QString>

class Version;

class TableInstance : public TreeItem,
        public NamedItem, public SerializableElement,
        public SqlSourceEntity, public ObjectWithUid,
        public CloneableElement
{
public:

    TableInstance(Table* tab, bool ref, const QString& uid, Version *v);

    TableInstance(const QString &name, bool ref, const QString &uid, Version *v);

    QVector<QString> columns() const;

    Table* table() const
    {
        return m_table;
    }

    void setValues(QHash < QString, QVector<QString> > v)
    {
        m_values = v;
    }

    void serialize(QDomDocument &doc, QDomElement &parent) const;

    const QHash < QString, QVector<QString> >& values() const
    {
        return m_values;
    }

    bool instantiatedBecuaseOfRkReference() const
    {
        return m_becauseOfReference;
    }

    void addTableReferencingThis(Table* refTab);

    void addInstantiatedTableInstance(TableInstance* tabInst)
    {
        m_instantiatedTablesInstances.append(tabInst);
    }

    virtual QStringList generateSqlSource(AbstractSqlGenerator *generator,QHash<QString,QString>, const Connection*);

    QVector<TableInstance*>& getInstantiatedTableInstances()
    {
        return m_instantiatedTablesInstances;
    }

    void sentence()
    {
        qDebug() <<  "sentece " << getName();
        m_sentenced = true;
    }

    void unSentence()
    {
        qDebug() <<  "unsentece " << getName();
        m_sentenced = false;
    }

    bool sentenced() const
    {
        return m_sentenced;
    }

    QString getReferencingTables() const
    {
        QString result = "";
        for(int i=0; i<m_referencingTables.size(); i++)
        {
            result += m_referencingTables.at(i)->getName();
            result += " ";
        }
        return result;
    }

    void addColumn(const QString& colName);

    QVector<QString> removeColumn(const QString& colName);

    void renameColumn(const QString& oldName, const QString& newName);
    virtual QUuid getClassUid() const;
    virtual CloneableElement* clone(Version *sourceVersion, Version *targetVersion);
    virtual void updateGui();

    void finalizeCloning(TableInstance* src, Version *sourceVersion, Version *targetVersion);
    void finalizeAutoinstantiatedTinsts(TableInstance* src, Version *sourceVersion, Version *targetVersion);

    void forceSetTableName(const QString& t)
    {
        m_tabName = t;
    }

    QString getTableName() const
    {
        return m_tabName;
    }

    void forceSetTable(Table* t)
    {
        m_table = t;
    }

    QVector <QVector<ColumnWithValue*> > getValues(QVector<ColumnWithValue*> columns);
    QVector <QVector<ColumnWithValue*> > getFullValues();


    virtual QString getSqlHash() const { return "N/A"; }


private:

    // the table of the table instance
    Table* m_table;

    // a hash in which to the column name we have mapped a list of default values.
    // TODO: This might need to be changed to be a hash of <Column*,QVector<QString> > instead ... think about it
    // TODO: for later versions come up with a solution that the USerDataType has values that can be put here ...
    // TODO: This "split" design for holding default values is simply horrible. Create a new class which holds default/startup values for the table and for the table
    // instances
    QHash < QString, QVector<QString> > m_values;

    // true if this table instance was created because of a reference (ie. foreign key for ex.)
    bool m_becauseOfReference;

    // this will contain a list of tables that have a foreign key to the table of this table instance
    QVector<Table*> m_referencingTables;

    // contains a list of other table instances that were created because this table instance's table required it (foreign key)
    QVector<TableInstance*> m_instantiatedTablesInstances;

    bool m_sentenced;

    // used in the deserialization, nowhere else
    QString m_tabName;

};

#endif // TABLEINSTANCE_H

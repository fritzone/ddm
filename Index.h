#ifndef INDEX_H
#define INDEX_H

#include "TreeItem.h"
#include "SerializableElement.h"

#include <QVector>
#include <QString>

class Column;
class Table;

class Index : virtual public TreeItem, virtual public SerializableElement
{
public:
    Index(Table*);

    Index(const QString& name, const QString& type, Table* tab);

    void addColumn(const Column* column);

    bool hasColumn(const Column*) const;

    const QString& getName() const;

    const QString& getType() const;

    void setName(const QString& name);

    void setType(const QString& type);

    void resetColumns();

    const QVector<const Column*>& getColumns() const
    {
        return m_columns;
    }

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    Table* getOwner() const
    {
        return m_owner;
    }

private:

    Table* m_owner;

    QString m_name;

    QString m_type;

    QVector<const Column*> m_columns;
};

#endif // INDEX_H

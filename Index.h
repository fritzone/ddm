#ifndef INDEX_H
#define INDEX_H

#include "TreeItem.h"
#include "NamedItem.h"
#include "SerializableElement.h"

#include <QVector>
#include <QString>

class Column;
class Table;

class Index : public TreeItem, public SerializableElement, public NamedItem
{
public:
    Index(Table*);

    Index(const QString& name, const QString& type, Table* tab);

    void addColumn(const Column* column);

    bool hasColumn(const Column*) const;

    const QString& getType() const;

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

    QString m_type;

    QVector<const Column*> m_columns;
};

#endif // INDEX_H

#ifndef INDEX_H
#define INDEX_H

#include "TreeItem.h"

#include <QVector>
#include <QString>

class Column;

class Index : public TreeItem
{
public:
    Index();

    Index(const QString& name, const QString& type);

    void addColumn(const Column* column);

    bool hasColumn(const Column*) const;

    const QString& getName() const;

    const QString& getType() const;

    void setName(const QString& name);

    void setType(const QString& type);

    void resetColumns();


private:

    QString m_name;

    QString m_type;

    QVector<const Column*> m_columns;
};

#endif // INDEX_H

#ifndef FOREIGNKEY_H
#define FOREIGNKEY_H

#include "TreeItem.h"
#include "SerializableElement.h"
#include "Column.h"
#include "DraggableGraphicsItemForForeignKey.h"

class Table;

class ForeignKey : public TreeItem, public SerializableElement
{

public:

    /**
     * Association between a foreign table column and a local table column.
     */
    class ColumnAssociation : public TreeItem
    {
    public:

        ColumnAssociation(const Table* foreignTable, const Column* foreignColumn, const Table* localTable, const Column* localColumn):
                m_foreignTable(foreignTable), m_localTable(localTable), m_foreignColumn(foreignColumn), m_localColumn(localColumn),
                m_sforeignTable(""), m_slocalTable(""), m_sforeignColumn(""), m_slocalColumn("")
        {}

        ColumnAssociation(QString foreignTable, QString foreignColumn, QString localTable, QString localColumn):
                m_foreignTable(0), m_localTable(0), m_foreignColumn(0), m_localColumn(0),
                m_sforeignTable(foreignTable), m_slocalTable(localTable), m_sforeignColumn(foreignColumn), m_slocalColumn(localColumn)
        {}

        const Table* getForeignTable() const { return m_foreignTable; }
        const Table* getLocalTable() const { return m_localTable; }
        const Column* getForeignColumn() const { return m_foreignColumn; }
        const Column* getLocalColumn() const { return m_localColumn; }

        QString getSForeignTable() const { return m_sforeignTable; }
        QString getSLocalTable() const { return m_slocalTable; }
        QString getSForeignColumn() const { return m_sforeignColumn; }
        QString getSLocalColumn() const { return m_slocalColumn; }

        void setForeignTable(const Table* foreignTable) {m_foreignTable = foreignTable;}
        void setForeignColumn(const Column* foreignColumn) {m_foreignColumn = foreignColumn;}
        void setLocalColumn(const Column* localColumn) {m_localColumn = localColumn;}
        void setLocalTable(const Table* localTable) {m_localTable = localTable;}

    private:

        const Table* m_foreignTable;
        const Table* m_localTable;
        const Column* m_foreignColumn;
        const Column* m_localColumn;

        QString m_sforeignTable;
        QString m_slocalTable;
        QString m_sforeignColumn;
        QString m_slocalColumn;

    };

public:

    ForeignKey(){}

    void addAssociation(ColumnAssociation* assoc)
    {
        m_associations.append(assoc);
    }

    ColumnAssociation* getAssociation(int i)
    {
        return m_associations.at(i);
    }

    const QVector<ColumnAssociation*>& getAssociations() const
    {
        return m_associations;
    }

    const QString& getName() const
    {
        return m_name;
    }

    void setName(const QString& name)
    {
        m_name = name;
    }

    const QString& getOnUpdate() const
    {
        return m_onUpdate;
    }

    void setOnUpdate(const QString& onUpdate)
    {
        m_onUpdate = onUpdate;
    }

    const QString& getOnDelete() const
    {
        return m_onDelete;
    }

    void setOnDelete(const QString& onDelete)
    {
        m_onDelete = onDelete;
    }

    void removeAssociation(const QString& fcName, const QString& lcName)
    {
        for(int i=0; i<m_associations.size(); i++)
        {
            if(m_associations[i]->getForeignColumn()->getName() == fcName && m_associations[i]->getLocalColumn()->getName() == lcName)
            {
                m_associations.remove(i);
                return;
            }
        }
    }

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const
    {
        QDomElement fkElement = doc.createElement("ForeignKey");      // will hold the foreign keys' stuff

        fkElement.setAttribute("Name", m_name);
        fkElement.setAttribute("OnUpdate", m_onUpdate);
        fkElement.setAttribute("OnDelete", m_onDelete);

        // save the associations
        QDomElement associationsElement = doc.createElement("Associations");
        for(int i=0; i<m_associations.size(); i++)
        {
            QString foreignTabName = m_associations[i]->getForeignTable()->getName();
            QString localTabName = m_associations[i]->getLocalTable()->getName();
            QString foreignColumn = m_associations[i]->getForeignColumn()->getName();
            QString localColumn = m_associations[i]->getLocalColumn()->getName();
            QDomElement assocElement = doc.createElement("Association");      // will hold the data in this element
            assocElement.setAttribute("ForeignTable", foreignTabName);
            assocElement.setAttribute("LocalTable", localTabName);
            assocElement.setAttribute("ForeignColumn", foreignColumn);
            assocElement.setAttribute("LocalColumn", localColumn);
            associationsElement.appendChild(assocElement);
        }
        fkElement.appendChild(associationsElement);
        parent.appendChild(fkElement);
    }

    /**                                            /\
     *                                            /  \
     * This should prepare a graphics like: __ __/name\__ ____
     *                                           \    /
     *                                            \  /
     *                                             \/
     * and the two lines after and before the space should be movable so that they move when the user moves the tables
     */
    DraggableGraphicsViewItemForForeignKey* getItem()
    {
        DraggableGraphicsViewItemForForeignKey* grp = new DraggableGraphicsViewItemForForeignKey(this);
        QGraphicsTextItem* txtName = new QGraphicsTextItem(getName(), grp);
        QRectF boundingForName = txtName->boundingRect();
        int rombX1 = boundingForName.left() - 5;
        int rombY1 = (boundingForName.bottom() + boundingForName.top()) / 2;
        int rombX2 = (boundingForName.right() + boundingForName.left()) / 2;
        int rombY2 = boundingForName.top() - (boundingForName.right() + boundingForName.left()) / 2;
        int rombX3 = boundingForName.right() + 5;
        int rombY3 = (boundingForName.bottom() + boundingForName.top()) / 2;
        int rombX4 = (boundingForName.right() + boundingForName.left()) / 2;
        int rombY4 = boundingForName.bottom() + (boundingForName.right() + boundingForName.left()) / 2;
        QGraphicsLineItem* line1 = new QGraphicsLineItem(rombX1, rombY1, rombX2, rombY2, grp);
        QGraphicsLineItem* line2 = new QGraphicsLineItem(rombX2, rombY2, rombX3, rombY3, grp);
        QGraphicsLineItem* line3 = new QGraphicsLineItem(rombX3, rombY3, rombX4, rombY4, grp);
        QGraphicsLineItem* line4 = new QGraphicsLineItem(rombX4, rombY4, rombX1, rombY1, grp);


        grp->setLeftPoint(QPointF(rombX1, rombY1));
        grp->setTopPoint(QPointF(rombX2, rombY2));
        grp->setRightPoint(QPointF(rombX3, rombY3));
        grp->setBottomPoint(QPointF(rombX4, rombY4));

        return grp;
    }

private:

    // the associations between the columns
    QVector<ColumnAssociation*> m_associations;

    QString m_name;

    QString m_onUpdate;

    QString m_onDelete;
};

#endif // FOREIGNKEY_H

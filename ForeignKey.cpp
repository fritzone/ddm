#include "ForeignKey.h"

void ForeignKey::removeAssociation(const QString& fcName, const QString& lcName)
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

void ForeignKey::serialize(QDomDocument &doc, QDomElement &parent) const
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
DraggableGraphicsViewItemForForeignKey* ForeignKey::getItem() const
{
    DraggableGraphicsViewItemForForeignKey* grp = new DraggableGraphicsViewItemForForeignKey();
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

    grp->setToolTip(getDescriptiveText());
    txtName->setToolTip(getDescriptiveText());
    line1->setToolTip(getDescriptiveText());
    line2->setToolTip(getDescriptiveText());
    line3->setToolTip(getDescriptiveText());
    line4->setToolTip(getDescriptiveText());

    return grp;
}

QString ForeignKey::getDescriptiveText() const
{
    QString result = "";
    QString localTable = "";
    QString foreignTable = "";
    for(int i=0; i<m_associations.size(); i++)
    {
        localTable = m_associations[i]->getSLocalTable();
        foreignTable = m_associations[i]->getSForeignTable();
        break;
    }
    localTable += "(";
    foreignTable += "(";
    for(int i=0; i<m_associations.size(); i++)
    {
        localTable += m_associations[i]->getSLocalColumn();
        foreignTable += m_associations[i]->getSForeignColumn();

        if(i<m_associations.size() - 1)
        {
            localTable += ", ";
            foreignTable += ", ";
        }
    }
    localTable += ") -> ";
    foreignTable += ")";
    result = localTable + foreignTable;
    return result;
}

QString ForeignKey::getLocalDescriptiveText() const
{
    QString result = "";
    for(int i=0; i<m_associations.size(); i++)
    {
        result += m_associations[i]->getSLocalColumn();
        if(i<m_associations.size() - 1)
        {
            result += ", ";
        }
    }

    return result;
}

QString ForeignKey::getForeignDescriptiveText() const
{
    QString result = "";
    for(int i=0; i<m_associations.size(); i++)
    {
        result += m_associations[i]->getSForeignColumn();
        if(i<m_associations.size() - 1)
        {
            result += ", ";
        }
    }
    return result;
}

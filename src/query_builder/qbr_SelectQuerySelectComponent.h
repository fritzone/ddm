#ifndef SELECTQUERYSELECTCOMPONENT_H
#define SELECTQUERYSELECTCOMPONENT_H

#include "qbr_QueryComponents.h"

class Column;
struct ColumnOfTabWithTabInstance;

class SelectQuerySelectComponent : public QueryComponent
{
public:
    SelectQuerySelectComponent(Query *q, QueryComponent*, int, Version *v);
    virtual QString get() const;
    virtual QString getClass() const {return "SelectQuerySelectComponent";}
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose();
    virtual QueryComponent* duplicate();
    virtual QVector<CellTypeChooserType>  getTypeset() const
    {
        return QVector<CellTypeChooserType> ();
    }
    bool hasGroupByFunctions();
    bool hasAtLeastOneColumnSelected();
    QVector<const ColumnOfTabWithTabInstance *> getSelectedColumns();

    /**
     * Returns a list with the following format: "$TabName.ColName"
     * Then "~AsText" if there is a component with AS
     * Or if neither of these, then  "#number" (such as order by an expression)
     */
    QStringList getOrderByElements();

    /**
     * This should return a vector of the components that are in the "SELECT" branch of the query
     */
    QVector<const QueryComponent*> getSelectedComponents();
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual QUuid getClassUid() const;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);
};

#endif // SELECTQUERYSELECTCOMPONENT_H

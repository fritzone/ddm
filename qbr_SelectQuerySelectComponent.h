#ifndef SELECTQUERYSELECTCOMPONENT_H
#define SELECTQUERYSELECTCOMPONENT_H

#include "qbr_QueryComponents.h"

class Column;

class SelectQuerySelectComponent : public QueryComponent
{
public:
    SelectQuerySelectComponent(QueryComponent*,int);
    virtual QString get() const;
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
    QVector<const Column*> getSelectedColumns();
    QStringList getOrderByElements();

    /**
     * This should return a vector of the components that are in the "SELECT" branch of the query
     */
    QVector<const QueryComponent*> getSelectedComponents();
};

#endif // SELECTQUERYSELECTCOMPONENT_H

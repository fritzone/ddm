#ifndef SELECTQUERYSELECTCOMPONENT_H
#define SELECTQUERYSELECTCOMPONENT_H

#include "qbr_QueryComponents.h"

class Column;

class SelectQuerySelectComponent : public QueryComponent
{
public:
    SelectQuerySelectComponent(QueryComponent*,int);
    virtual QString get(){return "SELECT"; }
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
};

#endif // SELECTQUERYSELECTCOMPONENT_H

#ifndef SELECTQUERYGROUPBYCOMPONENT_H
#define SELECTQUERYGROUPBYCOMPONENT_H

#include "qbr_QueryComponents.h"

class SelectQueryGroupByComponent : public QueryComponent
{
public:
    SelectQueryGroupByComponent(QueryComponent*, int);
    virtual QString get() const;
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){}
    virtual QueryComponent* duplicate();
    virtual QVector<CellTypeChooserType>  getTypeset() const
    {
        return QVector<CellTypeChooserType> ();
    }

};

#endif // SELECTQUERYGROUPBYCOMPONENT_H

#ifndef SELECTQUERYORDERBYCOMPONENT_H
#define SELECTQUERYORDERBYCOMPONENT_H

#include "qbr_QueryComponents.h"

class SelectQueryOrderByComponent : public QueryComponent
{
public:
    SelectQueryOrderByComponent(QueryComponent*, int);
    virtual QString get() const;
    virtual QString getClass() const {return "SelectQueryOrderByComponent";}
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){}
    virtual QueryComponent* duplicate();
    virtual QVector<CellTypeChooserType>  getTypeset() const
    {
        return QVector<CellTypeChooserType> ();
    }
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
};

#endif // SELECTQUERYORDERBYCOMPONENT_H

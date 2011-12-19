#ifndef SELECTQUERYFROMCOMPONENT_H
#define SELECTQUERYFROMCOMPONENT_H

#include "qbr_QueryComponents.h"

class Table;

class SelectQueryFromComponent : public QueryComponent
{
public:
    SelectQueryFromComponent(QueryComponent*,int);
    virtual QString get() const ;
    virtual QString getClass() const {return "SelectQueryFromComponent";}
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose();
    virtual QueryComponent* duplicate();
    virtual QVector<CellTypeChooserType>  getTypeset() const
    {
        return QVector<CellTypeChooserType> ();
    }

    bool allowCloseButton();

    QVector<const Table*> getTables() const;
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
};

#endif // SELECTQUERYFROMCOMPONENT_H

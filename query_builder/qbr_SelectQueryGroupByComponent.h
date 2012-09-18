#ifndef SELECTQUERYGROUPBYCOMPONENT_H
#define SELECTQUERYGROUPBYCOMPONENT_H

#include "qbr_QueryComponents.h"

class SelectQueryGroupByComponent : public QueryComponent
{
public:
    SelectQueryGroupByComponent(QueryComponent*, int, Version *v);
    virtual QString get() const;
    virtual QString getClass() const {return "SelectQueryGroupByComponent";}
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
    virtual QUuid getClassUid() const;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);
};

#endif // SELECTQUERYGROUPBYCOMPONENT_H

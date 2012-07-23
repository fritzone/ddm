#ifndef SELECTQUERYWHERECOMPONENT_H
#define SELECTQUERYWHERECOMPONENT_H

#include "qbr_QueryComponents.h"

class SelectQueryWhereComponent : public QueryComponent
{
public:

    // TODO: This is a classical lesson for polymorfism, I'm just too lazy right now
    enum WhereType
    {
        WHERETYPE_WHERE = 0,
        WHERETYPE_HAVING = 1,
        WHERETYPE_ON = 2
    };

    SelectQueryWhereComponent(QueryComponent*, int, WhereType);
    virtual QString get() const;
    virtual QString getClass() const
    {
        switch(m_whereType)
        {
        case WHERETYPE_WHERE: return "SelectQueryWhereComponent";
        case WHERETYPE_HAVING: return "SelectQueryHavingComponent";
        case WHERETYPE_ON: return "SelectQueryOnComponent";
        default:return "ImpossibleComponent";
        }
    }
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

private:
    WhereType m_whereType;
};

#endif // SELECTQUERYWHERECOMPONENT_H

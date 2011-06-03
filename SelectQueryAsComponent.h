#ifndef SELECTQUERYASCOMPONENT_H
#define SELECTQUERYASCOMPONENT_H

#include "QueryComponents.h"

class SelectQueryAsComponent : public QueryComponent
{
public:
    SelectQueryAsComponent(QueryComponent*, int);
    virtual QString get(){return "AS"; }
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose();
    virtual QueryComponent* duplicate();
    bool setAs(const QString& as);
    QString getAs() const
    {
        return m_as;
    }
    virtual QVector<CellTypeChooserType>  getTypeset() const
    {
        return QVector<CellTypeChooserType> ();
    }

private:

    QString m_as;
};

#endif // SELECTQUERYASCOMPONENT_H

#ifndef SELECTQUERYASCOMPONENT_H
#define SELECTQUERYASCOMPONENT_H

#include "qbr_QueryComponents.h"

class SelectQueryAsComponent : public QueryComponent
{
public:
    SelectQueryAsComponent(QueryComponent*, int);
    virtual QString get() const {return getSpacesForLevel() + "AS " + m_as; }
    virtual QString getClass() const {return "SelectQueryAsComponent";}
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
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

private:

    QString m_as;
};

#endif // SELECTQUERYASCOMPONENT_H
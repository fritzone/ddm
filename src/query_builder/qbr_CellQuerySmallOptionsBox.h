#ifndef CELLQUERYSMALLOPTIONSBOX_H
#define CELLQUERYSMALLOPTIONSBOX_H

#include "qbr_QueryGraphicsItem.h"
#include "qbr_QueryGraphicsHelper.h"
#include "qbr_QueryComponents.h"
#include "qbr_OptionTypes.h"

#include <QSet>

class CellQuerySmallOptionsBox : public QueryGraphicsItem
{
public:

    /* a structure holding the options present in this box, together with their icon*/
    struct OptionsList
    {
        QStringList text;
        QList<QIcon> icons;
    };

    CellQuerySmallOptionsBox(QSet<OptionsType> types, QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h) = 0;
    virtual void highlight(const QPen&) = 0;
    virtual QAbstractGraphicsShapeItem* getGraphicsItem() = 0;

    virtual void updateWidth(int)
    {}
    virtual void mousePress(int x, int y);
    virtual void mouseMove(int x, int y);
    virtual void mouseLeft(int x, int y);
    virtual void onClose()
    {}

private:

    OptionsList prepareOptions();

private:
    QSet<OptionsType> m_types;
};

#endif // CELLQUERYSMALLOPTIONSBOX_H

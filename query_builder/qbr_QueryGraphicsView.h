#ifndef QUERYGRAPHICSVIEW_H
#define QUERYGRAPHICSVIEW_H

#include <QGraphicsView>

class QueryGraphicsView : public QGraphicsView
{
public:
    QueryGraphicsView(QWidget* parent);
    virtual ~QueryGraphicsView();
};

#endif // QUERYGRAPHICSVIEW_H

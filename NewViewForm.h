#ifndef NEWVIEWFORM_H
#define NEWVIEWFORM_H

#include <QWidget>
#include "QueryComponents.h"

class QueryGraphicsView;
class QueryGraphicsScene;
class QueryGraphicsItem;

namespace Ui {
    class NewViewForm;
}

class NewViewForm : public QWidget {
    Q_OBJECT
public:
    NewViewForm(QueryComponents*c, QWidget *parent = 0);
    ~NewViewForm();
    void setGraphicsItem(QueryGraphicsItem*);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::NewViewForm *ui;
    QueryGraphicsView* m_qgv;
    QueryGraphicsScene* m_qgs;
    QueryComponents* m_comps;
};

#endif // NEWVIEWFORM_H

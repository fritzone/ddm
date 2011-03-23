#ifndef NEWVIEWFORM_H
#define NEWVIEWFORM_H

#include <QWidget>
#include "QueryGraphicsHelper.h"

class QueryGraphicsView;
class QueryGraphicsScene;
class QueryGraphicsItem;

namespace Ui {
    class NewViewForm;
}

class NewViewForm : public QWidget {
    Q_OBJECT
public:
    NewViewForm(QueryGraphicsHelper*c, QWidget *parent = 0);
    ~NewViewForm();
    void setGraphicsItem(QueryGraphicsItem*);
    QueryGraphicsScene* getScene()
    {
        return m_qgs;
    }

    void scrollTo(int hor, int ver);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::NewViewForm *ui;
    QueryGraphicsView* m_qgv;
    QueryGraphicsScene* m_qgs;
    QueryGraphicsHelper* m_comps;
};

#endif // NEWVIEWFORM_H

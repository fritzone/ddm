#ifndef NEWVIEWFORM_H
#define NEWVIEWFORM_H

#include <QWidget>

class QueryGraphicsView;
class QueryGraphicsScene;
class QueryGraphicsItem;

namespace Ui {
    class NewViewForm;
}

class NewViewForm : public QWidget {
    Q_OBJECT
public:
    NewViewForm(QWidget *parent = 0);
    ~NewViewForm();
    void setGraphicsItem(QueryGraphicsItem*);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::NewViewForm *ui;
    QueryGraphicsView* m_qgv;
    QueryGraphicsScene* m_qgs;
};

#endif // NEWVIEWFORM_H

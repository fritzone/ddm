#ifndef NEWVIEWFORM_H
#define NEWVIEWFORM_H

#include "SourceCodePresenterWidget.h"

#include <QWidget>

class QueryGraphicsHelper;
class QueryGraphicsView;
class QueryGraphicsScene;
class QueryGraphicsItem;
class SqlHighlighter;
class View;

namespace Ui {
    class NewViewForm;
}

class NewViewForm : public SourceCodePresenterWidget
{
    Q_OBJECT
public:
    NewViewForm(bool queryBuilder, QueryGraphicsHelper*c, QWidget *parent = 0);
    ~NewViewForm();
    void setGraphicsItem(QueryGraphicsItem*);
    QueryGraphicsScene* getScene()
    {
        return m_qgs;
    }

    void scrollTo(int hor, int ver);
    void getCenter(int& x, int& y);

    virtual void presentSql(Project*, const QString& codepage);
    virtual void presentSql(Project*, SqlSourceEntity*, const QString& codepage);
    void setView(View* );

protected:
    void changeEvent(QEvent *e);

private:
    Ui::NewViewForm *ui;
    QueryGraphicsView* m_qgv;
    QueryGraphicsScene* m_qgs;
    QueryGraphicsHelper* m_comps;
    SqlHighlighter* m_highlighter;
    bool m_queryBuilder;
    View* m_view;
};

#endif // NEWVIEWFORM_H

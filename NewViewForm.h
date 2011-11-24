#ifndef NEWVIEWFORM_H
#define NEWVIEWFORM_H

#include "SourceCodePresenterWidget.h"

#include <QWidget>
#include <QTextEdit>

class QueryGraphicsHelper;
class QueryGraphicsView;
class QueryGraphicsScene;
class QueryGraphicsItem;
class SqlHighlighter;
class View;
class QTextEditWithCodeCompletion;

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

public slots:
    void onChkCanReplaceToggle(bool);
    void onNameChange(QString);
    void onSqlChange();
    void onHelp();
    void onSaveSql();
    void onInject();

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
    //QTextEditWithCodeCompletion *txtSql;
    QTextEdit*txtSql;


};

#endif // NEWVIEWFORM_H

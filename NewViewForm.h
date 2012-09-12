#ifndef NEWVIEWFORM_H
#define NEWVIEWFORM_H

#include "SourceCodePresenterWidget.h"

#include <QWidget>
#include "TextEditWithCodeCompletion.h"

class QueryGraphicsHelper;
class QueryGraphicsView;
class QueryGraphicsScene;
class QueryGraphicsItem;
class SqlHighlighter;
class View;
class TextEditWithCodeCompletion;

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

    virtual void presentSql(Project*);
    virtual void presentSql(Project*, SqlSourceEntity*, MainWindow::showSomething){};
    void setView(View* );

public slots:
    void onChkCanReplaceToggle(bool);
    void onNameChange(QString);
    void onSqlChange();
    void onHelp();
    void onSaveSql();
    void onInject();
    void onLockUnlock(bool checked);

protected:
    void changeEvent(QEvent *e);

private:

    QString getViewNameFromSql();

private:
    Ui::NewViewForm *ui;
    QueryGraphicsView* m_qgv;
    QueryGraphicsScene* m_qgs;
    QueryGraphicsHelper* m_comps;
    bool m_queryBuilder;
    View* m_view;
    TextEditWithCodeCompletion*txtSql;
    bool m_updateSqlAfterNameChange;
    bool m_autoChange;
};

#endif // NEWVIEWFORM_H

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
class Version;
class WidgetForSpecificProperties;

namespace Ui {
    class NewViewForm;
}

class NewViewForm : public SourceCodePresenterWidget
{
    Q_OBJECT
public:
    NewViewForm(Version *v, bool queryBuilder, QueryGraphicsHelper*c, QWidget *parent = 0);
    ~NewViewForm();
    void setGraphicsItem(QueryGraphicsItem*);
    QueryGraphicsScene* getScene()
    {
        return m_qgs;
    }

    void scrollTo(int hor, int ver);
    void getCenter(int& x, int& y);

    virtual void presentSql(Project*, Version*);
    virtual void presentSql(Project*, SqlSource*, MainWindow::showSomething){}
    void setView(View* );

public slots:
    void onNameChange(QString);
    void onSqlChange();
    void onSaveSql();
    void onInject();
    void onLockUnlock(bool checked);
    void onUndelete();

protected:
    void changeEvent(QEvent *e);

private:

    QString getViewNameFromSql();
    void disableEditingControls(bool dis);

private:
    Ui::NewViewForm *ui;
    QueryGraphicsView* m_qgv;
    QueryGraphicsScene* m_qgs;
    QueryGraphicsHelper* m_qgh;
    bool m_queryBuilder;
    View* m_view;
    TextEditWithCodeCompletion*txtSql;
    bool m_updateSqlAfterNameChange;
    bool m_autoChange;
    Version* m_version;
    WidgetForSpecificProperties* m_mainWsp;

};

#endif // NEWVIEWFORM_H

#ifndef SQLFORM_H
#define SQLFORM_H

#include <QWidget>

#include "core_SqlSource.h"
#include "SourceCodePresenterWidget.h"

class SqlHighlighter;
class DatabaseEngine;

namespace Ui
{
    class SqlForm;
}

class SqlForm : public SourceCodePresenterWidget
{
    Q_OBJECT
public:
    SqlForm(Version* v, DatabaseEngine* engine, QWidget *parent = 0);
    ~SqlForm();

    void setSource(const QString& src);

    void setSqlList(QStringList s)
    {
        sqlList = s;
    }

    virtual void presentSql(Project* p, Version *v);
    virtual void presentSql(Project*, SqlSource*, MainWindow::showSomething s);

public slots:

    void onInject();
    void onSave();
    void onHelp();
    void onGoToOriginator();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SqlForm *ui;

    SqlHighlighter* highlighter;
    DatabaseEngine* m_engine;
    QStringList sqlList;
    SqlSource* m_sourceEntity;
};

#endif // SQLFORM_H

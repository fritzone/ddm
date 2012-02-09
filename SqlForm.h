#ifndef SQLFORM_H
#define SQLFORM_H

#include <QWidget>

#include "SqlSourceEntity.h"
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
    SqlForm(DatabaseEngine* engine, QWidget *parent = 0);
    ~SqlForm();

    void setSource(const QString& src);

    void setSqlList(QStringList s)
    {
        sqlList = s;
    }

    virtual void presentSql(Project* p,const QString& codepage);
    virtual void presentSql(Project*, SqlSourceEntity*,const QString& codepage);

public slots:

    void onInject();
    void onSave();
    void onHelp();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SqlForm *ui;

    SqlHighlighter* highlighter;
    DatabaseEngine* m_engine;
    QStringList sqlList;
    QString m_codepage;
};

#endif // SQLFORM_H

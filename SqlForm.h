#ifndef SQLFORM_H
#define SQLFORM_H

#include <QWidget>

#include "SqlSourceEntity.h"

class SqlHighlighter;
class DatabaseEngine;

namespace Ui
{
    class SqlForm;
}

class SqlForm : public QWidget {
    Q_OBJECT
public:
    SqlForm(DatabaseEngine* engine, QWidget *parent = 0);
    ~SqlForm();

    void setSqlSource(SqlSourceEntity* ent)
    {
        entity = ent;
    }

    void setSource(const QString& src);

    void setSqlList(QStringList s)
    {
        sqlList = s;
    }

public slots:

    void onInject();
    void onSave();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SqlForm *ui;
    SqlSourceEntity* entity;
    SqlHighlighter* highlighter;
    DatabaseEngine* m_engine;
    QStringList sqlList;
};

#endif // SQLFORM_H

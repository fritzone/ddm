#ifndef SQLFORM_H
#define SQLFORM_H

#include <QWidget>

#include "SqlSourceEntity.h"

class SqlHighlighter;

namespace Ui {
    class SqlForm;
}

class SqlForm : public QWidget {
    Q_OBJECT
public:
    SqlForm(QWidget *parent = 0);
    ~SqlForm();

    void setSqlSource(SqlSourceEntity* ent)
    {
        entity = ent;
    }

    void setSource(const QString& src);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SqlForm *ui;
    SqlSourceEntity* entity;
    SqlHighlighter* highlighter;
};

#endif // SQLFORM_H

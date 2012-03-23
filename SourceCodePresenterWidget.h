#ifndef SOURCECODEPRESENTERWIDGET_H
#define SOURCECODEPRESENTERWIDGET_H

#include <QWidget>
#include "mw_helper.h"

class Project;
class SqlSourceEntity;

class SourceCodePresenterWidget : public QWidget
{
public:

    SourceCodePresenterWidget(QWidget *parent = 0) : QWidget(parent), entity(0)
    {}

    /**
     * Presents the SQL for the whole project
     */
    virtual void presentSql(Project*) = 0;

    /**
     * Presents the SQL for the selected entity
     */
    virtual void presentSql(Project*, SqlSourceEntity*, MainWindow::showSomething s) = 0;

    virtual void updateSql(Project* p)
    {
        if(entity)
        {
            presentSql(p, entity, (MainWindow::showSomething)&MainWindow::showNothing);
        }
        else
        {
            presentSql(p);
        }
    }

    void setSqlSource(SqlSourceEntity* ent)
    {
        entity = ent;
    }

    SqlSourceEntity* getSqlSourceEntity() const
    {
        return entity;
    }

protected:

    SqlSourceEntity* entity;

};

#endif // SOURCECODEPRESENTERWIDGET_H

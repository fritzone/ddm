#ifndef SOURCECODEPRESENTERWIDGET_H
#define SOURCECODEPRESENTERWIDGET_H

#include <QWidget>
#include "mw_helper.h"

class Project;
class SqlSourceEntity;

class SourceCodePresenterWidget : public QWidget
{
public:

    SourceCodePresenterWidget(Version *v, QWidget *parent) : QWidget(parent), entity(0), m_version(v)
    {}

    /**
     * Presents the SQL for the whole project
     */
    virtual void presentSql(Project*, Version*) = 0;

    /**
     * Presents the SQL for the selected entity
     */
    virtual void presentSql(Project*, SqlSourceEntity*, MainWindow::showSomething s) = 0;

    virtual void updateSql(Project* p, Version *v)
    {
        if(entity)
        {
            presentSql(p, entity, (MainWindow::showSomething)&MainWindow::showNothing);
        }
        else
        {
            presentSql(p, v);
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

    Version* version() const
    {
        return m_version;
    }

protected:

    SqlSourceEntity* entity;
    Version* m_version;

};

#endif // SOURCECODEPRESENTERWIDGET_H

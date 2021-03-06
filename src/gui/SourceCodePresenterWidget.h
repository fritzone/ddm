#ifndef SOURCECODEPRESENTERWIDGET_H
#define SOURCECODEPRESENTERWIDGET_H

#include <QWidget>

#include "MainWindow.h"

class Project;
class SqlSource;
class Version;

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
    virtual void presentSql(Project*, SqlSource*, MainWindow::showSomething s) = 0;

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

    void setSqlSource(SqlSource* ent)
    {
        entity = ent;
    }

    SqlSource* getSqlSourceEntity() const
    {
        return entity;
    }

    Version* version() const
    {
        return m_version;
    }

protected:

    SqlSource* entity;
    Version* m_version;

};

#endif // SOURCECODEPRESENTERWIDGET_H

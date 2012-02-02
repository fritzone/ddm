#ifndef SOURCECODEPRESENTERWIDGET_H
#define SOURCECODEPRESENTERWIDGET_H

#include <QWidget>

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
    virtual void presentSql(Project*, const QString& codepage) = 0;

    /**
     * Presents the SQL for the selected entity
     */
    virtual void presentSql(Project*, SqlSourceEntity*, const QString& codepage) = 0;

    virtual void updateSql(Project* p, const QString& codepage)
    {
        if(entity)
        {
            presentSql(p, entity, codepage);
        }
        else
        {
            presentSql(p, codepage);
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

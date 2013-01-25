#ifndef REPO_REPOSITORYELEMENTFORM_H
#define REPO_REPOSITORYELEMENTFORM_H

#include <QWidget>

namespace Ui {
class repo_RepositoryElementForm;
}

class Entity;

class RepositoryEntityForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit RepositoryEntityForm(QWidget *parent, Entity* ent);
    ~RepositoryEntityForm();
    
private:
    Ui::repo_RepositoryElementForm *ui;
    Entity* m_entity;
};

#endif // REPO_REPOSITORYELEMENTFORM_H

#ifndef REPO_REPOSITORYELEMENTFORM_H
#define REPO_REPOSITORYELEMENTFORM_H

#include <QWidget>

namespace Ui {
class repo_RepositoryElementForm;
}

class repo_RepositoryElementForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit repo_RepositoryElementForm(QWidget *parent = 0);
    ~repo_RepositoryElementForm();
    
private:
    Ui::repo_RepositoryElementForm *ui;
};

#endif // REPO_REPOSITORYELEMENTFORM_H

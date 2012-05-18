#ifndef PROJECTDETAILSFORM_H
#define PROJECTDETAILSFORM_H

#include <QWidget>

class Project;

namespace Ui
{
    class ProjectDetailsForm;
}

class ProjectDetailsForm : public QWidget
{
    Q_OBJECT
public:
    ProjectDetailsForm(QWidget *parent = 0);
    ~ProjectDetailsForm();
    void setProject(Project* prj, const QString&);

public slots:
    void onBtnUpdate();
    void onOopCheckChange(int);
    void onNameChanged(QString);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ProjectDetailsForm *ui;

    Project* m_project;
};

#endif // PROJECTDETAILSFORM_H

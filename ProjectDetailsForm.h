#ifndef PROJECTDETAILSFORM_H
#define PROJECTDETAILSFORM_H

#include <QWidget>

namespace Ui {
    class ProjectDetailsForm;
}

class ProjectDetailsForm : public QWidget {
    Q_OBJECT
public:
    ProjectDetailsForm(QWidget *parent = 0);
    ~ProjectDetailsForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ProjectDetailsForm *ui;
};

#endif // PROJECTDETAILSFORM_H

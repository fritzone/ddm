#ifndef REPO_ROLELISTINGFORM_H
#define REPO_ROLELISTINGFORM_H

#include <QWidget>

namespace Ui {
class RoleListingForm;
}

class RoleListingForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit RoleListingForm(QWidget *parent = 0);
    ~RoleListingForm();
    
private:
    Ui::RoleListingForm *ui;
};

#endif // REPO_ROLELISTINGFORM_H

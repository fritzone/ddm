#ifndef REPO_DATABASEDETAILSFORM_H
#define REPO_DATABASEDETAILSFORM_H

#include <QWidget>

namespace Ui {
class repo_DatabaseDetailsForm;
}

class repo_DatabaseDetailsForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit repo_DatabaseDetailsForm(QWidget *parent = 0);
    ~repo_DatabaseDetailsForm();
    
private:
    Ui::repo_DatabaseDetailsForm *ui;
};

#endif // REPO_DATABASEDETAILSFORM_H

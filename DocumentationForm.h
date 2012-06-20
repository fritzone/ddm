#ifndef DOCUMENTATIONFORM_H
#define DOCUMENTATIONFORM_H

#include <QWidget>

namespace Ui {
class DocumentationForm;
}

class DocumentationForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit DocumentationForm(QWidget *parent = 0);
    ~DocumentationForm();
    
private:
    Ui::DocumentationForm *ui;
};

#endif // DOCUMENTATIONFORM_H

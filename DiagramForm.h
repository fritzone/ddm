#ifndef DIAGRAMFORM_H
#define DIAGRAMFORM_H

#include <QWidget>
class Version;

namespace Ui
{
    class DiagramForm;
}

class DiagramForm : public QWidget
{
    Q_OBJECT
public:
    DiagramForm(Version* v, QWidget *parent = 0);
    ~DiagramForm();


protected:
    void changeEvent(QEvent *e);

private:
    Ui::DiagramForm *ui;
    Version* ver;

};

#endif // DIAGRAMFORM_H

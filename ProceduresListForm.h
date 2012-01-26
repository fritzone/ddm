#ifndef ProceduresLISTFORM_H
#define ProceduresLISTFORM_H

#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
    class ProceduresListForm;
}

class Procedure;

class ProceduresListForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProceduresListForm(QWidget *parent = 0);
    ~ProceduresListForm();
    void populateProcedures(const QVector<Procedure *> procedures);
public slots:
    void doubleClickTree(QTreeWidgetItem* item, int);
protected:
    void changeEvent(QEvent *e);

private:
    Ui::ProceduresListForm *ui;
};

#endif // ProceduresLISTFORM_H

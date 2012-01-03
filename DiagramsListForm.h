#ifndef DIAGRAMSLISTFORM_H
#define DIAGRAMSLISTFORM_H

#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
    class DiagramsListForm;
}

class Diagram;
class MainWindow;

class DiagramsListForm : public QWidget
{
    Q_OBJECT

public:
    explicit DiagramsListForm(QWidget *parent = 0);
    void populateDiagrams(const QVector<Diagram*>);
    ~DiagramsListForm();
public slots:

    void doubleClickTree(QTreeWidgetItem*, int);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DiagramsListForm *ui;
    MainWindow* mw;
};

#endif // DIAGRAMSLISTFORM_H

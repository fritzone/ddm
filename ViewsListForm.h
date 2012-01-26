#ifndef VIEWSLISTFORM_H
#define VIEWSLISTFORM_H

#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
    class ViewsListForm;
}

class View;

class ViewsListForm : public QWidget
{
    Q_OBJECT

public:
    explicit ViewsListForm(QWidget *parent = 0);
    ~ViewsListForm();
    void populateViews(const QVector<View *> views);
public slots:
    void doubleClickTree(QTreeWidgetItem* item, int);
protected:
    void changeEvent(QEvent *e);

private:
    Ui::ViewsListForm *ui;

};

#endif // VIEWSLISTFORM_H

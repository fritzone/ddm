#ifndef BROWSETABLEFORM_H
#define BROWSETABLEFORM_H

#include <QWidget>

namespace Ui {
    class BrowseTableForm;
}

class QTableView;

class BrowseTableForm : public QWidget
{
    Q_OBJECT

public:
    explicit BrowseTableForm(QWidget *parent = 0);
    ~BrowseTableForm();
    QTableView* getTable();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::BrowseTableForm *ui;
};

#endif // BROWSETABLEFORM_H

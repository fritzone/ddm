#ifndef NAMEDOBJECTLISTINGFORM_H
#define NAMEDOBJECTLISTINGFORM_H

#include <QWidget>
#include <QStringList>

#include "ui_NamedObjectListingForm.h"  // hmm... hacking the proper QT way... long live templates
#include "MainWindow.h" // hm... plain ugliness ...
#include "core_NamedItem.h" // if needed then needed

class NamedObjectListingForm : public QWidget
{
    Q_OBJECT

public:
    explicit NamedObjectListingForm(QWidget *parent, MainWindow::showSomething shower, const QIcon& icon, const QString& title);
    ~NamedObjectListingForm();

    template <class T>
    void populateObjects(const QVector<T*>& items)
    {
        ui->treeItems->clear();
        for(int i=0; i<items.size(); i++)
        {
            QStringList a(items.at(i)->getName());
            QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, a);
            item->setData(0, Qt::UserRole, QVariant(items.at(i)->getObjectUid()));
            item->setIcon(0, m_icon);
            ui->treeItems->addTopLevelItem(item);
        }
    }

private slots:
    void doubleClickTree(QTreeWidgetItem*,int);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::NamedObjectListingForm *ui;
    MainWindow::showSomething m_objectShower;
    QIcon m_icon;
    QString m_title;
};

#endif // NAMEDOBJECTLISTINGFORM_H

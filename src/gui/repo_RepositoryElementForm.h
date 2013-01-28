#ifndef REPO_REPOSITORYELEMENTFORM_H
#define REPO_REPOSITORYELEMENTFORM_H

#include <QWidget>
#include <QMap>
#include <QString>

namespace Ui {
class repo_RepositoryElementForm;
}

class Entity;
class QTableWidgetItem;
class QComboBox;
class QSignalMapper;

class RepositoryEntityForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit RepositoryEntityForm(QWidget *parent, Entity* ent);
    ~RepositoryEntityForm();

private slots:

    void newAttribute(QTableWidgetItem*);
    void onAttributeRolesSelected(const QString&);
    void onDeleteAttribute();

private:
    Ui::repo_RepositoryElementForm *ui;
    Entity* m_entity;
    QMap<QString, QComboBox*> m_attributeCombos;
    QSignalMapper* m_signalMapperForCombosInAttributes;
};

#endif // REPO_REPOSITORYELEMENTFORM_H

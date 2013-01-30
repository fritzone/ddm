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
    void newCollection(QTableWidgetItem*);
    void newReference(QTableWidgetItem*);

    void onAttributeRolesSelected(const QString&);
    void onConnectionsRolesSelected(const QString&);
    void onConnectionsEntitySelected(const QString&);
    void onReferencesRolesSelected(const QString&);
    void onReferencesEntitySelected(const QString&);
    void onDeleteAttribute();

private:

    void populateAttributes();
    void populateCollections();
    void populateReferences();

private:
    Ui::repo_RepositoryElementForm *ui;
    Entity* m_entity;
    QMap<QString, QComboBox*> m_attributeCombos;

    QMap<QString, QComboBox*> m_collectionsCombosForRoles;
    QMap<QString, QComboBox*> m_collectionsCombosForEntities;

    QMap<QString, QComboBox*> m_referenceCombosForEntities;
    QMap<QString, QComboBox*> m_referenceCombosForRole;

    QSignalMapper* m_signalMapperForCombosInAttributes;

    QSignalMapper* m_signalMapperForCombosRoleInCollections;
    QSignalMapper* m_signalMapperForCombosEntityInCollections;

    QSignalMapper* m_signalMapperForCombosRoleInReferences;
    QSignalMapper* m_signalMapperForCombosEntityInReferences;
};

#endif // REPO_REPOSITORYELEMENTFORM_H


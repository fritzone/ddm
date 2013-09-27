#ifndef GUI_DELETEHELPER_H
#define GUI_DELETEHELPER_H

#include "MainWindow.h"
#include "Version.h"
#include "uids.h"
#include "core_VersionElement.h"
#include "VersionGuiElements.h"
#include "core_Function.h"
#include "core_Procedure.h"
#include "core_Diagram.h"
#include "core_View.h"
#include "core_Trigger.h"

#include <QMessageBox>

class DeleteHelper : public QObject
{
    Q_OBJECT

public:

    struct DeleterEntry
    {
        DeleterEntry() : m_versionDeleter(0), m_showSomething(0)
        {}

        DeleterEntry(MainWindow::showSomething showSomething, itemDeleter versionDeleter):
            m_versionDeleter(versionDeleter), m_showSomething(showSomething)
        {}

        itemDeleter m_versionDeleter; // this does teh actual delete
        MainWindow::showSomething m_showSomething;
    };

    DeleteHelper()
    {
        m_deleters[uidFunction] = DeleterEntry(&MainWindow::showFunctionWithGuid, &Version::deleteFunction);


        m_deleters[uidProcedure] = DeleterEntry(&MainWindow::showProcedureWithGuid, &Version::deleteProcedure);
        m_deleters[uidTable] = DeleterEntry(&MainWindow::showTableWithGuid, &Version::deleteTableWithName);
        m_deleters[uidTableInstance] = DeleterEntry(&MainWindow::showTableInstanceWithGuid, &Version::deleteTableInstance);
        m_deleters[uidTrigger] = DeleterEntry(&MainWindow::showTriggerWithGuid, &Version::deleteTrigger);
        m_deleters[uidView] = DeleterEntry(&MainWindow::showViewWithGuid, &Version::deleteView);;


        m_deleters[uidDiagram] = DeleterEntry(&MainWindow::showDiagramWithGuid, &Version::deleteDiagram);

    }

    ~DeleteHelper(){}

    const QMap<QString, DeleterEntry>& getDeleters() const
    {
        return m_deleters;
    }

    template <class T> void genericDeleter()
    {
        ObjectWithUid* ouid = MainWindow::instance()->getRightClickedObject<ObjectWithUid>();
        if(!ouid)
        {
            return;
        }

        if(getDeleters().contains(ouid->getClassUid()))
        {
            NamedItem* ni = dynamic_cast<NamedItem*>(ouid);
            if(!ni)
            {
                return;
            }

            if(QMessageBox::question(MainWindow::instance(), tr("Are you sure?"), tr("Really delete ") + ni->getName()+ "?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
            {
                return;
            }

            VersionElement* ve = dynamic_cast<VersionElement*>(ouid);
            if(!ve)
            {
                return;
            }

            itemDeleter versionDeleter = getDeleters()[ouid->getClassUid()].m_versionDeleter;
            (ve->version()->*versionDeleter)(ni->getName());
            ve->version()->getGui()->updateForms();

            MainWindow::instance()->showNamedObjectList(getDeleters()[ouid->getClassUid()].m_showSomething,
                    ve->version()->getListOfObjectsForUid<T>( *(dynamic_cast<T*>(ouid)) ) ,
                    IconFactory::getIconForUid(ouid->getClassUid().toString()),
                    getGroupName<T>(ouid->getClassUid().toString()));
        }

    }


private:
    QMap<QString, DeleterEntry> m_deleters;
};

#endif // GUI_DELETEHELPER_H

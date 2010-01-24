#ifndef MAJORVERSION_H
#define MAJORVERSION_H

#include "Version.h"

#include <QString>
#include <QTreeWidgetItem>

class MajorVersion : public Version
{
public:
    /**
     * Constructor: Takes in the project Item and the current version
     */
    MajorVersion(QTreeWidget* tree, QTreeWidgetItem* projectItem, int ver);

    QTreeWidgetItem* getDtsItem() const
    {
        return dtsItem;
    }

    QTreeWidgetItem* getTablesItem() const
    {
        return tablesItem;
    }

    QTreeWidgetItem* getViewsItem() const
    {
        return viewsItem;
    }

    QTreeWidgetItem* getVersionItem() const
    {
        return versionItem;
    }


private:

    // the tree item containing the "Tables" collector.
    QTreeWidgetItem* tablesItem;

    // the tree item containing the "Views" collector.
    QTreeWidgetItem* viewsItem;

    // the tree item containing the "DataType" collector.
    QTreeWidgetItem* dtsItem;

    // the tree item holding the version
    QTreeWidgetItem* versionItem;

    // the version
    QString version;
};

#endif // MAJORVERSION_H

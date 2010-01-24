#include <QtGui>

#include "MajorVersion.h"

MajorVersion::MajorVersion(QTreeWidget* tree, QTreeWidgetItem* projectItem,
			   int ver)
{
    QIcon tablesIcon(":/images/actions/images/small/table.png");
    QIcon viewsIcon(":/images/actions/images/small/view_icon.png");
    QIcon dtsIcon(":/images/actions/images/small/datatypes.PNG");
    QIcon versionIcon(":/images/actions/images/small/version.PNG");

    // make the dts sub item coming from the project
    QString v = QString::number(ver);
    v = QString("Ver: ") + v;
    v = v + ".0";

    versionItem = new QTreeWidgetItem(projectItem, QStringList(v)) ;
    versionItem->setIcon(0, versionIcon);
    tree->addTopLevelItem(versionItem);

    // make the dts sub item coming from the project
    dtsItem = new QTreeWidgetItem(versionItem, QStringList(QObject::tr("Data Types"))) ;
    dtsItem->setIcon(0, dtsIcon);
    tree->addTopLevelItem(dtsItem);

    // make the tables sub item coming from the project
    tablesItem = new QTreeWidgetItem(versionItem, QStringList(QObject::tr("Tables"))) ;
    tablesItem->setIcon(0, tablesIcon);
    tree->addTopLevelItem(tablesItem);

    // make the views sub item coming from the project
    viewsItem = new QTreeWidgetItem(versionItem, QStringList(QObject::tr("Views"))) ;
    viewsItem->setIcon(0, viewsIcon);
    tree->addTopLevelItem(viewsItem);
}



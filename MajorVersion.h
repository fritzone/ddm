#ifndef MAJORVERSION_H
#define MAJORVERSION_H

#include "Version.h"

#include <QString>
#include <QTreeWidgetItem>

/**
 * Class representing a major version of the database. Major versions are always baselined versions, ie. they contain only
 * the elements that (technically) do not contain the changes vectors. Minor versions contain changes vectors, and a
 * reference to the Major version with the required data types. Changes vectors are three vectors for each type (UDT, Index, Table ,...)
 * one of them is holding the value that was deleted, one of them is holding the value that  was changed and one of them is
 * holding the value that was updated. When the minor version is baselined (for ex: 1.75 -> 2.0) into a major one a new MajorVersion object is created
 * and the final list of types is written into it. When a new (minor) version is released (for ex: 1.0 -> 1.1) a new MinorVersion object is
 * created with the changes that were performed on the major version since its loading ... This means that we need to track the changes of
 * the major version starting from the point it was "loaded".
 */
class MajorVersion : public Version
{
public:
    /**
     * Constructor: Takes in the project Item and the current version
     */
    MajorVersion(QTreeWidget* tree, QTreeWidgetItem* projectItem, int ver);

    virtual QTreeWidgetItem* getDtsItem() const
    {
        return dtsItem;
    }

    virtual QTreeWidgetItem* getTablesItem() const
    {
        return tablesItem;
    }

    virtual QTreeWidgetItem* getViewsItem() const
    {
        return viewsItem;
    }

    virtual QTreeWidgetItem* getVersionItem() const
    {
        return versionItem;
    }

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    virtual void addNewDataType(UserDataType*);

    virtual const QVector<UserDataType*>& getDataTypes() const;

    virtual bool hasDataType(const QString& name) const;

    virtual UserDataType* getDataType(const QString& name);

    virtual int getDataTypeIndex(const QString& name);

    virtual void addTable(Table*);

    virtual bool hasTable(Table*);

    virtual Table* getTable(const QString& name);

    virtual const QVector<Table*>& getTables() const;


private:

    // the tree item containing the "Tables" collector.
    QTreeWidgetItem* tablesItem;

    // the tree item containing the "Views" collector.
    QTreeWidgetItem* viewsItem;

    // the tree item containing the "DataType" collector.
    QTreeWidgetItem* dtsItem;

    // the tree item holding the version
    QTreeWidgetItem* versionItem;

    // the version as a string representation. Major versions are always of form X.0
    QString version;

    // The vector of data types. The order in it is the one the user creates the data types
    QVector<UserDataType*> m_dataTypes;

    // the tables in the system
    QVector<Table*> m_tables;
};

#endif // MAJORVERSION_H

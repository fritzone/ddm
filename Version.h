#ifndef VERSION_H
#define VERSION_H

#include <QTreeWidgetItem>

class UserDataType;

/**
 * Basic class holding data related to versions
 */
class Version
{
public:
    Version();
    virtual ~Version();

    // the methods below will return the various tree items
    virtual QTreeWidgetItem* getDtsItem() const = 0;
    virtual QTreeWidgetItem* getTablesItem() const = 0;
    virtual QTreeWidgetItem* getViewsItem() const = 0;
    virtual QTreeWidgetItem* getVersionItem() const = 0;

    void addNewDataType(UserDataType*);

    const QVector<UserDataType*>& getDataTypes() const
    {
        return dataTypes;
    }

private:

    QVector<UserDataType*> dataTypes;
};

#endif // VERSION_H

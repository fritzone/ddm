#ifndef MAJORVERSION_H
#define MAJORVERSION_H

#include "DefaultVersionImplementation.h"
#include "ContextMenuEnabledTreeWidget.h"

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
class MajorVersion : public DefaultVersionImplementation
{
public:
    /**
     * Constructor: Takes in the project Item and the current version
     */
    MajorVersion(QTreeWidget* tree, QTreeWidget* dttree, QTreeWidget* issueTree, ContextMenuEnabledTreeWidgetItem* projectItem, int ver, Project*);

    /**
     * This constructos is used when deserializing. In this case the tree and projectItem are set later
     */
    MajorVersion(QString verAsString, Project*);

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

};

#endif // MAJORVERSION_H

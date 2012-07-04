#ifndef VERSIONDATA_H
#define VERSIONDATA_H

#include <QVector>

class UserDataType;
class Table;
class ForeignKey;
class Diagram;
class TableInstance;
class Issue;
class View;
class Procedure;
class Trigger;
class Function;

/**
 * The role of this class is to group together the actual data belonging to a version (tables, instances, etc...)
 */
class VersionData
{
public:

    VersionData() : m_dataTypes(), m_tables(), m_diagrams(), m_tableInstances(), m_issues(), m_views(), m_procedures(), m_functions(), m_triggers()
    {}

private:

    VersionData(const VersionData&);
    VersionData& operator = (const VersionData&);

    friend class DefaultVersionImplementation;
    friend class MajorVersion;

    // The vector of data types. The order in it is the one the user creates the data types
    QVector<UserDataType*> m_dataTypes;

    // the tables in the system
    QVector<Table*> m_tables;

    QVector<Diagram*> m_diagrams;

    QVector<TableInstance*> m_tableInstances;

    QVector<Issue*> m_issues;

    QVector<View*> m_views;

    QVector<Procedure*> m_procedures;

    QVector<Function*> m_functions;

    QVector<Trigger*> m_triggers;
};

#endif // VERSIONDATA_H

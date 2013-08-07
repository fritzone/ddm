#ifndef ENUMS_H
#define ENUMS_H

enum ProcedureFormMode
{
    MODE_FUNCTION = 1,
    MODE_PROCEDURE = 2
};

enum DT_TYPE
{
    DT_STRING = 0,
    DT_NUMERIC = 1,
    DT_BOOLEAN = 2,
    DT_DATETIME = 3,
    DT_BLOB = 4,
    DT_MISC = 5,
    DT_SPATIAL = 6,
    DT_INVALID = 7,
    DT_GENERIC = 8,
    DT_VARIABLE = 9
} ;

enum ConnectionState
{
    UNDEFINED = 0,
    CONNECTED = 1,
    FAILED = 2,
    DID_NOT_TRY = 3,
    DROPPED = 4,
    DELETED = 5
};

enum BrowsedTableLayout
{
    BROWSE_TABLE = 0,
    CREATE_SCRIPT = 1,
    BROWSE_VIEW = 2,
    BROWSE_PROCEDURE = 3,
    BROWSE_FUNCTION = 4,
    BROWSE_TRIGGER = 5,
    NEW_TABLE_IN_DB = 6
};

#endif // ENUMS_H

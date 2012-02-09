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
    DT_NUMERIC,
    DT_BOOLEAN,
    DT_DATETIME,
    DT_BLOB,
    DT_MISC,
    DT_SPATIAL,
    DT_INVALID,
    DT_GENERIC,
    DT_VARIABLE
} ;

enum ConnectionState
{
    UNDEFINED=0,
    CONNECTED=1,
    FAILED=2,
    DID_NOT_TRY=3,
    DROPPED = 4,
    DELETED = 5
};

#endif // ENUMS_H

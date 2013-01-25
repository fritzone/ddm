#ifndef UIDS_H
#define UIDS_H

#include <QString>

static const QString nullUid            =                   "{00000000-0000-0000-0000-000000000000}";

static const QString uidNumericDT       =                   "{10000000-0000-0000-0000-000000000001}";
static const QString uidStringDT        =                   "{10000000-0000-0000-0000-000000000002}";
static const QString uidDateTimeDT      =                   "{10000000-0000-0000-0000-000000000003}";
static const QString uidBooleanDT       =                   "{10000000-0000-0000-0000-000000000004}";
static const QString uidBlobDT          =                   "{10000000-0000-0000-0000-000000000005}";
static const QString uidMiscDT          =                   "{10000000-0000-0000-0000-000000000006}";
static const QString uidSpatialDT       =                   "{10000000-0000-0000-0000-000000000007}";

static const QString uidTable           =                   "{10000001-0000-0000-0000-000000000001}";
static const QString uidTableInstance   =                   "{10000001-0000-0000-0000-000000000002}";
static const QString uidView            =                   "{10000001-0000-0000-0000-000000000003}";
static const QString uidProcedure       =                   "{10000001-0000-0000-0000-000000000004}";
static const QString uidFunction        =                   "{10000001-0000-0000-0000-000000000005}";
static const QString uidTrigger         =                   "{10000001-0000-0000-0000-000000000006}";
static const QString uidColumn          =                   "{10000001-0000-0000-0000-000000000007}";
static const QString uidIndex           =                   "{10000001-0000-0000-0000-000000000008}";
static const QString uidForeignKey      =                   "{10000001-0000-0000-0000-000000000009}";
static const QString uidDiagram         =                   "{10000001-0000-0000-0000-00000000000A}";
static const QString uidColumnOfIndex   =                   "{10000001-0000-0000-0000-00000000000B}";

static const QString uidTrueFalseSp     =                   "{10000002-0000-0000-0000-000000000001}";
static const QString uidValueListSp     =                   "{10000002-0000-0000-0000-000000000002}";
static const QString uidValueSp         =                   "{10000002-0000-0000-0000-000000000003}";

static const QString uidSqlTableProperty =                  "{10000003-0000-0000-0000-000000000001}";

// MySQL role UIDS
static const QString uidMysqlTemporaryTable =               "{10000004-0000-0000-0000-000000000001}";
static const QString uidMysqlIfNotExistsTable =             "{10000004-0000-0000-0000-000000000002}";
static const QString uidMysqlStorageEngineTable =           "{10000004-0000-0000-0000-000000000003}";
static const QString uidMysqlCodepageTable =                "{10000004-0000-0000-0000-000000000004}";

static const QString uidMysqlIndexType =                    "{10000005-0000-0000-0000-000000000001}";
static const QString uidMysqlIndexCategory =                "{10000005-0000-0000-0000-000000000002}";

static const QString uidMysqlColumnOfIndexLength =          "{10000006-0000-0000-0000-000000000001}";

static const QString uidMysqlColumnAutoIncrement =          "{10000007-0000-0000-0000-000000000001}";
static const QString uidMysqlColumnZeroFill =               "{10000007-0000-0000-0000-000000000002}";

static const QString uidSelectQueryAsComponent =            "{10000008-0000-0000-0000-000000000001}";
static const QString uidSelectQuerySelectComponent =        "{10000008-0000-0000-0000-000000000002}";
static const QString uidSelectQueryFromComponent =          "{10000008-0000-0000-0000-000000000003}";
static const QString uidSelectQueryWhereComponent =         "{10000008-0000-0000-0000-000000000004}";
static const QString uidSelectQueryGroupByComponent =       "{10000008-0000-0000-0000-000000000005}";
static const QString uidSelectQueryOrderByComponent =       "{10000008-0000-0000-0000-000000000006}";
static const QString uidSelectQueryJoinComponent    =       "{10000008-0000-0000-0000-000000000007}";
static const QString uidSingleExpressionComponent   =       "{10000008-0000-0000-0000-000000000008}";
static const QString uidTableQueryComponent         =       "{10000008-0000-0000-0000-000000000009}";
static const QString uidSelectQuery                 =       "{10000008-0000-0000-0000-00000000000A}";

static const QString uidDraggableGraphicsViewItem   =       "{10000009-0000-0000-0000-000000000001}";
static const QString uidDraggableGraphicsViewItemForText =  "{10000009-0000-0000-0000-000000000002}";
static const QString uidDraggableGraphicsViewItemForFK =    "{10000009-0000-0000-0000-000000000003}";
static const QString uidFKRelationdescriptor        =       "{10000009-0000-0000-0000-000000000004}";
static const QString uidDiagramTableDescriptor      =       "{10000009-0000-0000-0000-000000000005}";
static const QString uidDiagramNoteDescriptor       =       "{10000009-0000-0000-0000-000000000006}";
static const QString uidDiagramFkDescriptor         =       "{10000009-0000-0000-0000-000000000007}";

static const QString uidVersion                     =       "{1000000A-0000-0000-0000-000000000001}";

static const QString uidPatch                       =       "{1000000B-0000-0000-0000-000000000001}";

static const QString uidRepoEntity                  =       "{1000000C-0000-0000-0000-000000000001}";
static const QString uidMysqlDb = "{100000DB-0000-0000-0000-000000000001}";

#endif // UIDS_H

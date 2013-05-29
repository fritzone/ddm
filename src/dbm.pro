# -------------------------------------------------
# Project created by QtCreator 2009-11-25T12:54:30
# -------------------------------------------------
QT += network \
    sql \
    xml \
    webkit \
    svg

TARGET = ddm
TEMPLATE = app
unix:QMAKE_CXXFLAGS_WARN_ON += -Wall -Wextra -ansi -pedantic -std=c++0x

SOURCES += main.cpp \
    Configuration.cpp \
    DeserializationFactory.cpp \
    FkRelationDescriptor.cpp \
    StartupValuesHelper.cpp \
    SqlNamesValidator.cpp \
    NameGenerator.cpp \
    Workspace.cpp \
    ClipboardFactory.cpp \
    IssueManager.cpp \
    helper_MostRecentlyUsedFiles.cpp \
    UidWarehouse.cpp \
    SpsTooltipProviderForUid.cpp \
    DocumentationGenerator.cpp \
    TableUpdateGenerator.cpp \
    VersionUpdateGenerator.cpp \
    helper_utils.cpp \
    db_DatabaseEngineManager.cpp

HEADERS += \
    strings.h \
    Configuration.h \
    DeserializationFactory.h \
    FkRelationDescriptor.h \
    StartupValuesHelper.h \
    SqlNamesValidator.h \
    NameGenerator.h \
    Workspace.h \
    ClipboardFactory.h \
    utils.h \
    gui_colors.h \
    helper_MostRecentlyUsedFiles.h \
    commons.h \
    uids.h \
    UidWarehouse.h \
    SpsTooltipProviderForUid.h \
    DocumentationGenerator.h \
    TableUpdateGenerator.h \
    VersionUpdateGenerator.h \
    helper_utils.h \
    core_enums.h \
    core/DefaultVersionImplementation.h \
    db_DatabaseEngineManager.h

include (db/db.pri)
INCLUDEPATH += db

include (query_builder/query_builder.pri)
INCLUDEPATH += query_builder

include (qhtml/qhtml.pri)
INCLUDEPATH += qhtml

include (dbmysql/dbmysql.pri)
INCLUDEPATH += dbmysql

include (dbsqlite/dbsqlite.pri)
INCLUDEPATH += dbsqlite

include (connections/connections.pri)
INCLUDEPATH += connections

include (diagram/diagram.pri)
INCLUDEPATH += diagram

include (core/core.pri)
INCLUDEPATH += core

include (reverse_eng/reverse_eng.pri)
INCLUDEPATH += reverse_eng

include (issues/issues.pri)
INCLUDEPATH += issues

include (gui/gui.pri)
INCLUDEPATH += gui

RESOURCES += dbm.qrc \
    help_resources.qrc \
    docstyles.qrc
RC_FILE = dbm.rc

OTHER_FILES += \
    classic.css

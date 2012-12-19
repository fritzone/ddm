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
    DataTypesListForm.cpp \
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
    helper_utils.cpp

HEADERS += \
    strings.h \
    Configuration.h \
    DeserializationFactory.h \
    FkRelationDescriptor.h \
    StartupValuesHelper.h \
    SqlNamesValidator.h \
    NameGenerator.h \
    Workspace.h \
    DefaultVersionImplementation.h \
    ClipboardFactory.h \
    utils.h \
    gui_colors.h \
    helper_MostRecentlyUsedFiles.h \
    commons.h \
    uids.h \
    UidWarehouse.h \
    SpsTooltipProviderForUid.h \
    DeploymentInitiator.h \
    DocumentationGenerator.h \
    TableUpdateGenerator.h \
    VersionUpdateGenerator.h \
    helper_utils.h \
    core_enums.h \
    core/ForeignKey.h

include (db/db.pri)
INCLUDEPATH += db

include (query_builder/query_builder.pri)
INCLUDEPATH += query_builder

include (qhtml/qhtml.pri)
INCLUDEPATH += qhtml

include (dbmysql/dbmysql.pri)
INCLUDEPATH += dbmysql

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

installfiles_base.files += ddm codepages/codepages.mysql.dat
installfiles_base.path =  /home/ferenc/rpmbuild/BUILDROOT/usr/local/ddm
INSTALLS += installfiles_base
installfiles_doc.files += doc/*
installfiles_doc.path =  /home/ferenc/rpmbuild/BUILDROOT/usr/local/ddm/doc
INSTALLS += installfiles_doc
installfiles_rs.files += rsrc/mysql.defaults
installfiles_rs.path =  /home/ferenc/rpmbuild/BUILDROOT/usr/local/ddm/rsrc
INSTALLS += installfiles_rs

OTHER_FILES += \
    classic.css
























































































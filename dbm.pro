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
    MainWindowButtonDialog.cpp \
    DataTypesListForm.cpp \
    Project.cpp \
    MajorVersion.cpp \
    Codepage.cpp \
    Configuration.cpp \
    NewTableForm.cpp \
    Index.cpp \
    DeserializationFactory.cpp \
    ProjectDetailsForm.cpp \
    Solution.cpp \
    AboutBoxDialog.cpp \
    DiagramForm.cpp \
    FkRelationDescriptor.cpp \
    EnterNoteTextDialog.cpp \
    ForeignKey.cpp \
    ContextMenuDelegate.cpp \
    CreateTableInstancesDialog.cpp \
    TableInstanceForm.cpp \
    StartupValuesHelper.cpp \
    DynamicActionHandlerforMainWindow.cpp \
    SqlHighlighter.cpp \
    SqlForm.cpp \
    InjectSqlDialog.cpp \
    PreferencesDialog.cpp \
    SqlNamesValidator.cpp \
    NameGenerator.cpp \
    SimpleTextInputDialog.cpp \
    ContextMenuCollection.cpp \
    Workspace.cpp \
    VersionGuiElements.cpp \
    DefaultVersionImplementation.cpp \
    ClipboardFactory.cpp \
    IssueManager.cpp \
    IssueOriginator.cpp \
    ForeignKeyReccomendationIssue.cpp \
    NewViewForm.cpp \
    MainWindow.cpp \
    NewDataTypeForm.cpp \
    NewProjectDialog.cpp \
    gui_HelpWindow.cpp \
    core_Connection.cpp \
    core_ConnectionManager.cpp \
    core_DeployerThread.cpp \
    core_Deployer.cpp \
    core_ConnectionIssue.cpp \
    core_InjectSqlGenerator.cpp \
    core_InjectSqlGeneratorThread.cpp \
    helper_MostRecentlyUsedFiles.cpp \
    BrowseTableForm.cpp \
    ProcedureForm.cpp \
    FrameForLineNumbers.cpp \
    TextEditWithCodeCompletion.cpp \
    ListWidgetForCodeCompletion.cpp \
    GuiElements.cpp \
    ConnectionGuiElements.cpp \
    TriggerForm.cpp \
    NamedObjectListingForm.cpp \
    core_StoredMethod.cpp \
    UidWarehouse.cpp \
    WidgetForSpecificProperties.cpp \
    TrueFalseSp.cpp \
    ObjectWithSpInstances.cpp \
    TrueFalseSpInstance.cpp \
    SpsTooltipProviderForUid.cpp \
    ValueListSp.cpp \
    ValueListSpInstance.cpp \
    ValueSp.cpp \
    ValueSpInstance.cpp \
    DeploymentInitiator.cpp \
    DocumentationForm.cpp \
    DocumentationGenerator.cpp \
    SpInstance.cpp \
    TreeItem.cpp \
    TableComparisonForm.cpp \
    TableUpdateGenerator.cpp \
    VersionUpdateGenerator.cpp

HEADERS += MainWindow.h \
    MainWindowButtonDialog.h \
    NewProjectDialog.h \
    NewDataTypeForm.h \
    DataTypesListForm.h \
    strings.h \
    Project.h \
    MajorVersion.h \
    Version.h \
    Configuration.h \
    NewTableForm.h \
    IconFactory.h \
    TreeItem.h \
    Index.h \
    SerializableElement.h \
    NamedItem.h \
    DeserializationFactory.h \
    ProjectDetailsForm.h \
    ForeignKey.h \
    Solution.h \
    AboutBoxDialog.h \
    DiagramForm.h \
    TableListWidget.h \
    FkRelationDescriptor.h \
    EnterNoteTextDialog.h \
    ContextMenuEnabledTreeWidget.h \
    CreateTableInstancesDialog.h \
    TableInstanceForm.h \
    StartupValuesHelper.h \
    DynamicActionHandlerForMainWindow.h \
    SqlHighlighter.h \
    SqlForm.h \
    SqlSourceEntity.h \
    InjectSqlDialog.h \
    PreferencesDialog.h \
    SqlNamesValidator.h \
    NameGenerator.h \
    SourceCodePresenterWidget.h \
    SimpleTextInputDialog.h \
    ContextMenuCollection.h \
    Workspace.h \
    VersionGuiElements.h \
    DefaultVersionImplementation.h \
    CopyableElement.h \
    ClipboardFactory.h \
    VersionData.h \
    IssueManager.h \
    Issue.h \
    IssueOriginator.h \
    ForeignKeyReccomendationIssue.h \
    NewViewForm.h \
    utils.h \
    ColumnProviderForQuery.h \
    gui_HelpWindow.h \
    core_Connection.h \
    core_ConnectionManager.h \
    core_DeployerThread.h \
    core_Deployer.h \
    core_ConnectionIssue.h \
    core_InjectSqlGenerator.h \
    core_InjectSqlGeneratorThread.h \
    gui_colors.h \
    helper_MostRecentlyUsedFiles.h \
    BrowseTableForm.h \
    ProcedureForm.h \
    FrameForLineNumbers.h \
    TextEditWithCodeCompletion.h \
    ListWidgetForCodeCompletion.h \
    commons.h \
    GuiElements.h \
    ConnectionGuiElements.h \
    TriggerForm.h \
    NamedObjectListingForm.h \
    enums.h \
    core_StoredMethod.h \
    mw_helper.h \
    uids.h \
    UidWarehouse.h \
    WidgetForSpecificProperties.h \
    TrueFalseSp.h \
    ObjectWithSpInstances.h \
    SpInstance.h \
    TrueFalseSpInstance.h \
    SpsTooltipProviderForUid.h \
    ValueListSp.h \
    ValueListSpInstance.h \
    ValueSp.h \
    ValueSpInstance.h \
    DeploymentInitiator.h \
    DocumentationForm.h \
    DocumentationGenerator.h \
    TableComparisonForm.h \
    TableUpdateGenerator.h \
    VersionUpdateGenerator.h

FORMS += MainWindow.ui \
    MainWindowButtonDialog.ui \
    NewProjectDialog.ui \
    NewDataTypeForm.ui \
    DataTypesListForm.ui \
    NewTableForm.ui \
    ProjectDetailsForm.ui \
    AboutBoxDialog.ui \
    DiagramForm.ui \
    EnterNoteTextDialog.ui \
    CreateTableInstancesDialog.ui \
    TableInstanceForm.ui \
    SqlForm.ui \
    InjectSqlDialog.ui \
    PreferencesDialog.ui \
    SimpleTextInputDialog.ui \
    NewViewForm.ui \
    QueryItemListDialog.ui \
    HelpWindow.ui \
    BrowseTableForm.ui \
    ProcedureForm.ui \
    TriggerForm.ui \
    NamedObjectListingForm.ui \
    WidgetForSpecificProperties.ui \
    DocumentationForm.ui \
    TableComparisonForm.ui

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
























































































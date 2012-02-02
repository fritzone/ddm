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
QMAKE_CXXFLAGS_WARN_ON += -Wall -Wextra

SOURCES += main.cpp \
    MainWindowButtonDialog.cpp \
    DataTypesListForm.cpp \
    DataType.cpp \
    UserDataType.cpp \
    Project.cpp \
    MajorVersion.cpp \
    Codepage.cpp \
    Configuration.cpp \
    Table.cpp \
    NewTableForm.cpp \
    Column.cpp \
    Index.cpp \
    DeserializationFactory.cpp \
    ProjectDetailsForm.cpp \
    Solution.cpp \
    AboutBoxDialog.cpp \
    DiagramForm.cpp \
    DraggableGraphicsItem.cpp \
    ERGraphicsView.cpp \
    ERGraphicsScene.cpp \
    DraggableGraphicsViewItemForForeignKey.cpp \
    FkRelationDescriptor.cpp \
    Diagram.cpp \
    DraggableGraphicsViewItemForText.cpp \
    EnterNoteTextDialog.cpp \
    ForeignKey.cpp \
    DiagramTableDescriptor.cpp \
    DiagramNoteDescriptor.cpp \
    ContextMenuDelegate.cpp \
    CreateTableInstancesDialog.cpp \
    TableInstance.cpp \
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
    ReverseEngineerWizardWelcomeForm.cpp \
    ReverseEngineerWizard.cpp \
    ReverseEngineerWizardDatabasesForm.cpp \
    ReverseEngineerWizardTablesForm.cpp \
    ReverseEngineerWizardOptionsForm.cpp \
    IssueManager.cpp \
    IssueOriginator.cpp \
    ForeignKeyReccomendationIssue.cpp \
    NewViewForm.cpp \
    MainWindow.cpp \
    NewDataTypeForm.cpp \
    NewProjectDialog.cpp \
    db_DatabaseEngine.cpp \
    db_DatabaseNormalizationIssue.cpp \
    core_View.cpp \
    gui_HelpWindow.cpp \
    core_Connection.cpp \
    core_ConnectionManager.cpp \
    core_DeployerThread.cpp \
    core_Deployer.cpp \
    core_ConnectionIssue.cpp \
    core_InjectSqlGenerator.cpp \
    core_InjectSqlGeneratorThread.cpp \
    core_ReverseEngineerer.cpp \
    core_ReverseEngineererThread.cpp \
    helper_MostRecentlyUsedFiles.cpp \
    BrowseTableForm.cpp \
    ProcedureForm.cpp \
    FrameForLineNumbers.cpp \
    core_Procedure.cpp \
    TextEditWithCodeCompletion.cpp \
    ListWidgetForCodeCompletion.cpp \
    GuiElements.cpp \
    ConnectionGuiElements.cpp \
    TriggerForm.cpp \
    core_Trigger.cpp \
    NamedObjectListingForm.cpp \
    core_Function.cpp \
    core_StoredMethod.cpp
HEADERS += MainWindow.h \
    MainWindowButtonDialog.h \
    NewProjectDialog.h \
    NewDataTypeForm.h \
    DataTypesListForm.h \
    DataType.h \
    UserDataType.h \
    strings.h \
    Project.h \
    MajorVersion.h \
    Version.h \
    Codepage.h \
    Configuration.h \
    Table.h \
    NewTableForm.h \
    IconFactory.h \
    TreeItem.h \
    Column.h \
    Index.h \
    SerializableElement.h \
    NamedItem.h \
    DeserializationFactory.h \
    ProjectDetailsForm.h \
    ForeignKey.h \
    Solution.h \
    AboutBoxDialog.h \
    DiagramForm.h \
    DraggableGraphicsItem.h \
    ERGraphicsScene.h \
    ERGraphicsView.h \
    TableListWidget.h \
    DraggableGraphicsItemForForeignKey.h \
    FkRelationDescriptor.h \
    Diagram.h \
    DraggableGraphicsItemForText.h \
    EnterNoteTextDialog.h \
    DiagramNoteDescriptor.h \
    DiagramObjectDescriptor.h \
    DiagramFKDescriptor.h \
    DiagramTableDescriptor.h \
    ContextMenuEnabledTreeWidget.h \
    CreateTableInstancesDialog.h \
    TableInstance.h \
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
    ReverseEngineerWizardWelcomeForm.h \
    ReverseEngineerWizard.h \
    ReverseEngineerWizardDatabasesForm.h \
    ReverseEngineerWizardOptionsForm.h \
    IssueManager.h \
    Issue.h \
    IssueOriginator.h \
    ForeignKeyReccomendationIssue.h \
    NewViewForm.h \
    utils.h \
    ColumnProviderForQuery.h \
    db_AbstractDTSupplier.h \
    db_AbstractSQLGenerator.h \
    db_AbstractStorageEngine.h \
    db_AbstractStorageEngineListProvider.h \
    db_DatabaseBuiltinFunction.h \
    db_DatabaseBuiltinFunctionsParameter.h \
    db_DatabaseEngine.h \
    db_DatabaseNormalizationIssue.h \
    core_View.h \
    gui_HelpWindow.h \
    core_Connection.h \
    core_ConnectionManager.h \
    core_DeployerThread.h \
    core_Deployer.h \
    core_ConnectionIssue.h \
    core_InjectSqlGenerator.h \
    core_InjectSqlGeneratorThread.h \
    core_ReverseEngineerer.h \
    core_ReverseEngineererThread.h \
    gui_colors.h \
    helper_MostRecentlyUsedFiles.h \
    BrowseTableForm.h \
    ProcedureForm.h \
    FrameForLineNumbers.h \
    core_Procedure.h \
    TextEditWithCodeCompletion.h \
    ListWidgetForCodeCompletion.h \
    commons.h \
    GuiElements.h \
    ConnectionGuiElements.h \
    TriggerForm.h \
    core_Trigger.h \
    NamedObjectListingForm.h \
    core_Function.h \
    enums.h \
    core_StoredMethod.h \
    ReverseEngineerWizardObjectListForm.h
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
    ReverseEngineerWizardWelcomeForm.ui \
    ReverseEngineerWizardDatabasesForm.ui \
    ReverseEngineerWizardOptionsForm.ui \
    NewViewForm.ui \
    QueryItemListDialog.ui \
    HelpWindow.ui \
    BrowseTableForm.ui \
    ProcedureForm.ui \
    TriggerForm.ui \
    NamedObjectListingForm.ui \
    ReverseEngineerWizardObjectListForm.ui
include (query_builder/query_builder.pri)
INCLUDEPATH += query_builder
include (dbmysql/dbmysql.pri)
INCLUDEPATH += dbmysql
RESOURCES += dbm.qrc
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






















































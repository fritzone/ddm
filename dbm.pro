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
    TablesListForm.cpp \
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
    TableInstancesListForm.cpp \
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
    qbr_SingleExpressionQueryComponent.cpp \
    qbr_DatabaseFunctionInstantiationComponent.cpp \
    qbr_CellForSingleExpression.cpp \
    qbr_CellAsCommand.cpp \
    qbr_CellClose.cpp \
    qbr_CellCommand.cpp \
    qbr_CellFromCommand.cpp \
    qbr_CellGroupByCommand.cpp \
    qbr_CellJoinCommand.cpp \
    qbr_CellOrderByCommand.cpp \
    qbr_CellQuerySmallOptionsBox.cpp \
    qbr_CellSelectCommand.cpp \
    qbr_CellTable.cpp \
    qbr_CellTableChooser.cpp \
    qbr_CellTypeChooser.cpp \
    qbr_CellWhereCommand.cpp \
    qbr_SelectQuery.cpp \
    qbr_SelectQueryAsComponent.cpp \
    qbr_SelectQueryFromComponent.cpp \
    qbr_SelectQueryGraphicsItem.cpp \
    qbr_SelectQueryGroupByComponent.cpp \
    qbr_SelectQueryJoinComponent.cpp \
    qbr_SelectQueryOrderByComponent.cpp \
    qbr_SelectQuerySelectComponent.cpp \
    qbr_SelectQueryWhereComponent.cpp \
    qbr_QueryAsGenerator.cpp \
    qbr_QueryComponents.cpp \
    qbr_QueryGraphicsHelper.cpp \
    qbr_QueryGraphicsItem.cpp \
    qbr_QueryGraphicsScene.cpp \
    qbr_QueryGraphicsView.cpp \
    qbr_QueryItemListDialog.cpp \
    qbr_QueryTextInputItem.cpp \
    qbr_TableQueryComponent.cpp \
    qbr_TableGraphicsItem.cpp \
    dbmysql_MySQLCodepageSupplier.cpp \
    dbmysql_MySQLDatabaseEngine.cpp \
    dbmysql_MySQLDTSupplier.cpp \
    dbmysql_MySQLSQLGenerator.cpp \
    dbmysql_MySQLStorageEngineListProvider.cpp \
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
    DiagramsListForm.cpp \
    ViewsListForm.cpp \
    ProcedureForm.cpp \
    FrameForLineNumbers.cpp \
    core_Procedure.cpp \
    ProceduresListForm.cpp \
    TextEditWithCodeCompletion.cpp \
    ListWidgetForCodeCompletion.cpp
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
    TablesListForm.h \
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
    TableInstancesListForm.h \
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
    ReverseEngineerWizardTablesForm.h \
    ReverseEngineerWizardOptionsForm.h \
    IssueManager.h \
    Issue.h \
    IssueOriginator.h \
    ForeignKeyReccomendationIssue.h \
    NewViewForm.h \
    utils.h \
    ColumnProviderForQuery.h \
    qbr_SingleExpressionQueryComponent.h \
    qbr_DatabaseFunctionInstantiationComponent.h \
    qbr_CellForSingleExpression.h \
    qbr_CellAsCommand.h \
    qbr_CellClose.h \
    qbr_CellCommand.h \
    qbr_CellFromCommand.h \
    qbr_CellGroupByCommand.h \
    qbr_CellJoinCommand.h \
    qbr_CellOrderByCommand.h \
    qbr_CellQuerySmallOptionsBox.h \
    qbr_CellSelectCommand.h \
    qbr_CellTable.h \
    qbr_CellTableChooser.h \
    qbr_CellTypeChooser.h \
    qbr_CellWhereCommand.h \
    qbr_SelectQuery.h \
    qbr_SelectQueryAsComponent.h \
    qbr_SelectQueryFromComponent.h \
    qbr_SelectQueryGraphicsItem.h \
    qbr_SelectQueryGroupByComponent.h \
    qbr_SelectQueryJoinComponent.h \
    qbr_SelectQueryOrderByComponent.h \
    qbr_SelectQuerySelectComponent.h \
    qbr_SelectQueryWhereComponent.h \
    qbr_OptionTypes.h \
    qbr_Query.h \
    qbr_QueryAsGenerator.h \
    qbr_QueryComponents.h \
    qbr_QueryGraphicsHelper.h \
    qbr_QueryGraphicsItem.h \
    qbr_QueryGraphicsScene.h \
    qbr_QueryGraphicsView.h \
    qbr_QueryItemListDialog.h \
    qbr_QueryTextInputItem.h \
    qbr_TableQueryComponent.h \
    qbr_TableGraphicsItem.h \
    dbmysql_MySQLArchiveStorageEngine.h \
    dbmysql_MySQLCodepageSupplier.h \
    dbmysql_MySQLDTSupplier.h \
    dbmysql_MySQLInnoDBStorageEngine.h \
    dbmysql_MySQLMemoryStorageEngine.h \
    dbmysql_MySQLMyISAMStorageEngine.h \
    dbmysql_MySQLSQLGenerator.h \
    dbmysql_MySQLStorageEngineListProvider.h \
    dbmysql_MySQLDatabaseEngine.h \
    db_AbstractCodepageSupplier.h \
    db_AbstractDTSupplier.h \
    db_AbstractIndextypeProvider.h \
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
    DiagramsListForm.h \
    ViewsListForm.h \
    ProcedureForm.h \
    FrameForLineNumbers.h \
    core_Procedure.h \
    ProceduresListForm.h \
    TextEditWithCodeCompletion.h \
    ListWidgetForCodeCompletion.h \
    commons.h
FORMS += MainWindow.ui \
    MainWindowButtonDialog.ui \
    NewProjectDialog.ui \
    NewDataTypeForm.ui \
    DataTypesListForm.ui \
    TablesListForm.ui \
    NewTableForm.ui \
    ProjectDetailsForm.ui \
    AboutBoxDialog.ui \
    DiagramForm.ui \
    EnterNoteTextDialog.ui \
    CreateTableInstancesDialog.ui \
    TableInstanceForm.ui \
    SqlForm.ui \
    InjectSqlDialog.ui \
    TableInstancesListForm.ui \
    PreferencesDialog.ui \
    SimpleTextInputDialog.ui \
    ReverseEngineerWizardWelcomeForm.ui \
    ReverseEngineerWizardDatabasesForm.ui \
    ReverseEngineerWizardTablesForm.ui \
    ReverseEngineerWizardOptionsForm.ui \
    NewViewForm.ui \
    QueryItemListDialog.ui \
    HelpWindow.ui \
    BrowseTableForm.ui \
    DiagramsListForm.ui \
    ViewsListForm.ui \
    ProcedureForm.ui \
    ProceduresListForm.ui
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

















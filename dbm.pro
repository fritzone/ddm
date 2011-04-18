# -------------------------------------------------
# Project created by QtCreator 2009-11-25T12:54:30
# -------------------------------------------------
QT += network \
    sql \
    xml \
    webkit \
    svg
TARGET = dbm
TEMPLATE = app
SOURCES += main.cpp \
    MainWindowButtonDialog.cpp \
    DataTypesListForm.cpp \
    AbstractDTSupplier.cpp \
    DataType.cpp \
    MySQLDTSupplier.cpp \
    UserDataType.cpp \
    Project.cpp \
    MajorVersion.cpp \
    AbstractCodepageSupplier.cpp \
    MySQLCodepageSupplier.cpp \
    Codepage.cpp \
    DatabaseEngine.cpp \
    Configuration.cpp \
    Table.cpp \
    TablesListForm.cpp \
    NewTableForm.cpp \
    Column.cpp \
    AbstractIndextypeProvider.cpp \
    Index.cpp \
    DeserializationFactory.cpp \
    ProjectDetailsForm.cpp \
    Solution.cpp \
    MySQLDatabaseEngine.cpp \
    AbstractStorageEngine.cpp \
    AbstractStorageEngineListProvider.cpp \
    MySQLStorageEngineListProvider.cpp \
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
    MySQLSQLGenerator.cpp \
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
    DatabaseNormalizationIssue.cpp \
    ForeignKeyReccomendationIssue.cpp \
    NewViewForm.cpp \
    QueryGraphicsView.cpp \
    QueryGraphicsScene.cpp \
    Query.cpp \
    QueryGraphicsItem.cpp \
    SelectQuery.cpp \
    SelectQueryGraphicsItem.cpp \
    TableGraphicsItem.cpp \
    CellCommand.cpp \
    CellTable.cpp \
    CellTypeChooser.cpp \
    CellClose.cpp \
    CellTableChooser.cpp \
    QueryComponents.cpp \
    QueryItemListDialog.cpp \
    QueryGraphicsHelper.cpp \
    TableQueryComponent.cpp \
    SelectQuerySelectComponent.cpp \
    SelectQueryFromComponent.cpp \
    CellQuerySmallOptionsBox.cpp \
    SelectQueryWhereComponent.cpp \
    CellSelectCommand.cpp \
    CellFromCommand.cpp \
    CellWhereCommand.cpp \
    CellGroupByCommand.cpp \
    SelectQueryGroupByComponent.cpp \
    SelectQueryAsComponent.cpp \
    CellAsCommand.cpp \
    QueryTextInputItem.cpp \
    SelectQueryOrderByComponent.cpp \
    CellOrderByCommand.cpp \
    QueryAsGenerator.cpp \
    MainWindow.cpp \
    NewDataTypeForm.cpp \
    NewProjectDialog.cpp \
    SelectQueryJoinComponent.cpp \
    CellJoinCommand.cpp
HEADERS += MainWindow.h \
    MainWindowButtonDialog.h \
    NewProjectDialog.h \
    NewDataTypeForm.h \
    DataTypesListForm.h \
    AbstractDTSupplier.h \
    DataType.h \
    MySQLDTSupplier.h \
    UserDataType.h \
    strings.h \
    Project.h \
    MajorVersion.h \
    Version.h \
    AbstractCodepageSupplier.h \
    MySQLCodepageSupplier.h \
    Codepage.h \
    DatabaseEngine.h \
    Configuration.h \
    Table.h \
    TablesListForm.h \
    NewTableForm.h \
    IconFactory.h \
    TreeItem.h \
    Column.h \
    AbstractIndextypeProvider.h \
    MySQLIndextypeProvider.h \
    Index.h \
    SerializableElement.h \
    NamedItem.h \
    DeserializationFactory.h \
    ProjectDetailsForm.h \
    ForeignKey.h \
    Solution.h \
    MySQLDatabaseEngine.h \
    AbstractStorageEngine.h \
    AbstractStorageEngineListProvider.h \
    MySQLStorageEngineListProvider.h \
    MySQLMyISAMStorageEngine.h \
    MySQLInnoDBStorageEngine.h \
    MySQLMemoryStorageEngine.h \
    MySQLArchiveStorageEngine.h \
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
    AbstractSQLGenerator.h \
    MySQLSQLGenerator.h \
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
    DatabaseNormalizationIssue.h \
    ForeignKeyReccomendationIssue.h \
    NewViewForm.h \
    QueryGraphicsView.h \
    QueryGraphicsScene.h \
    Query.h \
    QueryGraphicsItem.h \
    SelectQuery.h \
    SelectQueryGraphicsItem.h \
    TableGraphicsItem.h \
    CellCommand.h \
    CellTable.h \
    CellTypeChooser.h \
    CellClose.h \
    CellTableChooser.h \
    QueryComponents.h \
    QueryItemListDialog.h \
    QueryGraphicsHelper.h \
    TableQueryComponent.h \
    SelectQuerySelectComponent.h \
    SelectQueryFromComponent.h \
    CellQuerySmallOptionsBox.h \
    SelectQueryWhereComponent.h \
    CellSelectCommand.h \
    CellFromCommand.h \
    CellWhereCommand.h \
    OptionsType.h \
    CellGroupByCommand.h \
    SelectQueryGroupByComponent.h \
    SelectQueryAsComponent.h \
    CellAsCommand.h \
    QueryTextInputItem.h \
    SelectQueryOrderByComponent.h \
    CellOrderByCommand.h \
    utils.h \
    QueryAsGenerator.h \
    SelectQueryJoinComponent.h \
    CellJoinCommand.h \
    ColumnProviderForQuery.h
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
    QueryItemListDialog.ui
RESOURCES += dbm.qrc

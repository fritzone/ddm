# -------------------------------------------------
# Project created by QtCreator 2009-11-25T12:54:30
# -------------------------------------------------
QT += network \
    sql \
    xml
TARGET = dbm
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    MainWindowButtonDialog.cpp \
    NewProjectDialog.cpp \
    newdatatypeform.cpp \
    datatypeslistform.cpp \
    AbstractDTSupplier.cpp \
    DataType.cpp \
    MySQLDTSupplier.cpp \
    UserDataType.cpp \
    Project.cpp \
    MajorVersion.cpp \
    Version.cpp \
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
    FkRelationDescriptor.cpp
HEADERS += MainWindow.h \
    MainWindowButtonDialog.h \
    NewProjectDialog.h \
    newdatatypeform.h \
    datatypeslistform.h \
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
    FkRelationDescriptor.h
FORMS += mainwindow.ui \
    mainwindowbuttondialog.ui \
    newprojectdialog.ui \
    newdatatypeform.ui \
    datatypeslistform.ui \
    TablesListForm.ui \
    NewTableForm.ui \
    ProjectDetailsForm.ui \
    AboutBoxDialog.ui \
    DiagramForm.ui
RESOURCES += dbm.qrc

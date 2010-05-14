#############################################################################
# Makefile for building: dbm
# Generated by qmake (2.01a) (Qt 4.6.2) on: Fri Apr 16 09:21:17 2010
# Project:  dbm.pro
# Template: app
# Command: /home/ferenc/qtsdk-2010.02/qt/bin/qmake -spec ../../qtsdk-2010.02/qt/mkspecs/linux-g++-64 -unix CONFIG+=debug -o Makefile dbm.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_SQL_LIB -DQT_XML_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -m64 -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -m64 -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I../../qtsdk-2010.02/qt/mkspecs/linux-g++-64 -I. -I../../qtsdk-2010.02/qt/include/QtCore -I../../qtsdk-2010.02/qt/include/QtNetwork -I../../qtsdk-2010.02/qt/include/QtGui -I../../qtsdk-2010.02/qt/include/QtXml -I../../qtsdk-2010.02/qt/include/QtSql -I../../qtsdk-2010.02/qt/include -I. -I.
LINK          = g++
LFLAGS        = -m64 -Wl,-rpath,/home/ferenc/qtsdk-2010.02/qt/lib
LIBS          = $(SUBLIBS)  -L/home/ferenc/qtsdk-2010.02/qt/lib -lQtSql -L/home/ferenc/qtsdk-2010.02/qt/lib -lQtXml -lQtGui -L/usr/X11R6/lib64 -lQtNetwork -lQtCore -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /home/ferenc/qtsdk-2010.02/qt/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
		mainwindow.cpp \
		mainwindowbuttondialog.cpp \
		newprojectdialog.cpp \
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
		MysqlCodepageSupplier.cpp \
		Codepage.cpp \
		DatabaseEngine.cpp \
		Configuration.cpp \
		Table.cpp \
		TablesListForm.cpp \
		NewTableForm.cpp \
		Column.cpp \
		AbstractIndextypeProvider.cpp \
		Index.cpp moc_mainwindow.cpp \
		moc_mainwindowbuttondialog.cpp \
		moc_newprojectdialog.cpp \
		moc_newdatatypeform.cpp \
		moc_datatypeslistform.cpp \
		moc_TablesListForm.cpp \
		moc_NewTableForm.cpp \
		qrc_dbm.cpp
OBJECTS       = main.o \
		mainwindow.o \
		mainwindowbuttondialog.o \
		newprojectdialog.o \
		newdatatypeform.o \
		datatypeslistform.o \
		AbstractDTSupplier.o \
		DataType.o \
		MySQLDTSupplier.o \
		UserDataType.o \
		Project.o \
		MajorVersion.o \
		Version.o \
		AbstractCodepageSupplier.o \
		MysqlCodepageSupplier.o \
		Codepage.o \
		DatabaseEngine.o \
		Configuration.o \
		Table.o \
		TablesListForm.o \
		NewTableForm.o \
		Column.o \
		AbstractIndextypeProvider.o \
		Index.o \
		moc_mainwindow.o \
		moc_mainwindowbuttondialog.o \
		moc_newprojectdialog.o \
		moc_newdatatypeform.o \
		moc_datatypeslistform.o \
		moc_TablesListForm.o \
		moc_NewTableForm.o \
		qrc_dbm.o
DIST          = ../../qtsdk-2010.02/qt/mkspecs/common/g++.conf \
		../../qtsdk-2010.02/qt/mkspecs/common/unix.conf \
		../../qtsdk-2010.02/qt/mkspecs/common/linux.conf \
		../../qtsdk-2010.02/qt/mkspecs/qconfig.pri \
		../../qtsdk-2010.02/qt/mkspecs/features/qt_functions.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/qt_config.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/exclusive_builds.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/default_pre.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/debug.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/default_post.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/warn_on.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/qt.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/unix/thread.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/moc.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/resources.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/uic.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/yacc.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/lex.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/include_source_dir.prf \
		dbm.pro
QMAKE_TARGET  = dbm
DESTDIR       = 
TARGET        = dbm

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): ui_mainwindow.h ui_mainwindowbuttondialog.h ui_newprojectdialog.h ui_newdatatypeform.h ui_datatypeslistform.h ui_TablesListForm.h ui_NewTableForm.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: dbm.pro  ../../qtsdk-2010.02/qt/mkspecs/linux-g++-64/qmake.conf ../../qtsdk-2010.02/qt/mkspecs/common/g++.conf \
		../../qtsdk-2010.02/qt/mkspecs/common/unix.conf \
		../../qtsdk-2010.02/qt/mkspecs/common/linux.conf \
		../../qtsdk-2010.02/qt/mkspecs/qconfig.pri \
		../../qtsdk-2010.02/qt/mkspecs/features/qt_functions.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/qt_config.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/exclusive_builds.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/default_pre.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/debug.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/default_post.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/warn_on.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/qt.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/unix/thread.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/moc.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/resources.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/uic.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/yacc.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/lex.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/include_source_dir.prf \
		/home/ferenc/qtsdk-2010.02/qt/lib/libQtSql.prl \
		/home/ferenc/qtsdk-2010.02/qt/lib/libQtCore.prl \
		/home/ferenc/qtsdk-2010.02/qt/lib/libQtXml.prl \
		/home/ferenc/qtsdk-2010.02/qt/lib/libQtGui.prl \
		/home/ferenc/qtsdk-2010.02/qt/lib/libQtNetwork.prl
	$(QMAKE) -spec ../../qtsdk-2010.02/qt/mkspecs/linux-g++-64 -unix CONFIG+=debug -o Makefile dbm.pro
../../qtsdk-2010.02/qt/mkspecs/common/g++.conf:
../../qtsdk-2010.02/qt/mkspecs/common/unix.conf:
../../qtsdk-2010.02/qt/mkspecs/common/linux.conf:
../../qtsdk-2010.02/qt/mkspecs/qconfig.pri:
../../qtsdk-2010.02/qt/mkspecs/features/qt_functions.prf:
../../qtsdk-2010.02/qt/mkspecs/features/qt_config.prf:
../../qtsdk-2010.02/qt/mkspecs/features/exclusive_builds.prf:
../../qtsdk-2010.02/qt/mkspecs/features/default_pre.prf:
../../qtsdk-2010.02/qt/mkspecs/features/debug.prf:
../../qtsdk-2010.02/qt/mkspecs/features/default_post.prf:
../../qtsdk-2010.02/qt/mkspecs/features/warn_on.prf:
../../qtsdk-2010.02/qt/mkspecs/features/qt.prf:
../../qtsdk-2010.02/qt/mkspecs/features/unix/thread.prf:
../../qtsdk-2010.02/qt/mkspecs/features/moc.prf:
../../qtsdk-2010.02/qt/mkspecs/features/resources.prf:
../../qtsdk-2010.02/qt/mkspecs/features/uic.prf:
../../qtsdk-2010.02/qt/mkspecs/features/yacc.prf:
../../qtsdk-2010.02/qt/mkspecs/features/lex.prf:
../../qtsdk-2010.02/qt/mkspecs/features/include_source_dir.prf:
/home/ferenc/qtsdk-2010.02/qt/lib/libQtSql.prl:
/home/ferenc/qtsdk-2010.02/qt/lib/libQtCore.prl:
/home/ferenc/qtsdk-2010.02/qt/lib/libQtXml.prl:
/home/ferenc/qtsdk-2010.02/qt/lib/libQtGui.prl:
/home/ferenc/qtsdk-2010.02/qt/lib/libQtNetwork.prl:
qmake:  FORCE
	@$(QMAKE) -spec ../../qtsdk-2010.02/qt/mkspecs/linux-g++-64 -unix CONFIG+=debug -o Makefile dbm.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/dbm1.0.0 || $(MKDIR) .tmp/dbm1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/dbm1.0.0/ && $(COPY_FILE) --parents mainwindow.h mainwindowbuttondialog.h newprojectdialog.h newdatatypeform.h datatypeslistform.h AbstractDTSupplier.h DataType.h MySQLDTSupplier.h UserDataType.h strings.h Project.h MajorVersion.h Version.h AbstractCodepageSupplier.h MysqlCodepageSupplier.h Codepage.h DatabaseEngine.h Configuration.h Table.h TablesListForm.h NewTableForm.h IconFactory.h TreeItem.h Column.h AbstractIndextypeProvider.h MySQLIndextypeProvider.h Index.h .tmp/dbm1.0.0/ && $(COPY_FILE) --parents dbm.qrc .tmp/dbm1.0.0/ && $(COPY_FILE) --parents main.cpp mainwindow.cpp mainwindowbuttondialog.cpp newprojectdialog.cpp newdatatypeform.cpp datatypeslistform.cpp AbstractDTSupplier.cpp DataType.cpp MySQLDTSupplier.cpp UserDataType.cpp Project.cpp MajorVersion.cpp Version.cpp AbstractCodepageSupplier.cpp MysqlCodepageSupplier.cpp Codepage.cpp DatabaseEngine.cpp Configuration.cpp Table.cpp TablesListForm.cpp NewTableForm.cpp Column.cpp AbstractIndextypeProvider.cpp Index.cpp .tmp/dbm1.0.0/ && $(COPY_FILE) --parents mainwindow.ui mainwindowbuttondialog.ui newprojectdialog.ui newdatatypeform.ui datatypeslistform.ui TablesListForm.ui NewTableForm.ui .tmp/dbm1.0.0/ && (cd `dirname .tmp/dbm1.0.0` && $(TAR) dbm1.0.0.tar dbm1.0.0 && $(COMPRESS) dbm1.0.0.tar) && $(MOVE) `dirname .tmp/dbm1.0.0`/dbm1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/dbm1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_mainwindow.cpp moc_mainwindowbuttondialog.cpp moc_newprojectdialog.cpp moc_newdatatypeform.cpp moc_datatypeslistform.cpp moc_TablesListForm.cpp moc_NewTableForm.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_mainwindow.cpp moc_mainwindowbuttondialog.cpp moc_newprojectdialog.cpp moc_newdatatypeform.cpp moc_datatypeslistform.cpp moc_TablesListForm.cpp moc_NewTableForm.cpp
moc_mainwindow.cpp: mainwindow.h
	/home/ferenc/qtsdk-2010.02/qt/bin/moc $(DEFINES) $(INCPATH) mainwindow.h -o moc_mainwindow.cpp

moc_mainwindowbuttondialog.cpp: mainwindowbuttondialog.h
	/home/ferenc/qtsdk-2010.02/qt/bin/moc $(DEFINES) $(INCPATH) mainwindowbuttondialog.h -o moc_mainwindowbuttondialog.cpp

moc_newprojectdialog.cpp: newprojectdialog.h
	/home/ferenc/qtsdk-2010.02/qt/bin/moc $(DEFINES) $(INCPATH) newprojectdialog.h -o moc_newprojectdialog.cpp

moc_newdatatypeform.cpp: newdatatypeform.h
	/home/ferenc/qtsdk-2010.02/qt/bin/moc $(DEFINES) $(INCPATH) newdatatypeform.h -o moc_newdatatypeform.cpp

moc_datatypeslistform.cpp: datatypeslistform.h
	/home/ferenc/qtsdk-2010.02/qt/bin/moc $(DEFINES) $(INCPATH) datatypeslistform.h -o moc_datatypeslistform.cpp

moc_TablesListForm.cpp: TablesListForm.h
	/home/ferenc/qtsdk-2010.02/qt/bin/moc $(DEFINES) $(INCPATH) TablesListForm.h -o moc_TablesListForm.cpp

moc_NewTableForm.cpp: NewTableForm.h
	/home/ferenc/qtsdk-2010.02/qt/bin/moc $(DEFINES) $(INCPATH) NewTableForm.h -o moc_NewTableForm.cpp

compiler_rcc_make_all: qrc_dbm.cpp
compiler_rcc_clean:
	-$(DEL_FILE) qrc_dbm.cpp
qrc_dbm.cpp: dbm.qrc \
		images/big/bigfileimport.png \
		images/big/bigadd.png \
		images/big/bigfilenew.png \
		images/big/bigfileexport.png \
		images/big/bigfileopen.png \
		images/big/bigdatatypes.PNG \
		images/actions/filesave.png \
		images/actions/add.png \
		images/actions/table.png \
		images/actions/empty_icon.png \
		images/actions/filenew.png \
		images/small/flag_korean.PNG \
		images/small/flag_armenia.PNG \
		images/small/version.PNG \
		images/small/up.png \
		images/small/flag_bin.PNG \
		images/small/flag_chinese.PNG \
		images/small/flag_greek.PNG \
		images/small/datatypes_datetime.PNG \
		images/small/table.png \
		images/small/flag_latvian.PNG \
		images/small/mysql.png \
		images/small/datatypes.PNG \
		images/small/flag_bulgarian.PNG \
		images/small/recycled.png \
		images/small/flag_ukrainian.PNG \
		images/small/flag_roman.PNG \
		images/small/widget_doc.png \
		images/small/flag_hungarian.PNG \
		images/small/down.png \
		images/small/oracle.PNG \
		images/small/flag_slovenian.PNG \
		images/small/flag_israel.PNG \
		images/small/right.png \
		images/small/remove.png \
		images/small/document-convert.png \
		images/small/flag_spanish2.PNG \
		images/small/flag_english.PNG \
		images/small/flag_turkish.PNG \
		images/small/datatypes_blob.PNG \
		images/small/flag_spanish.PNG \
		images/small/flag_lithuanian.PNG \
		images/small/flag_unicode.PNG \
		images/small/flag_danish.PNG \
		images/small/left.png \
		images/small/key.png \
		images/small/view_icon.png \
		images/small/flag_thai.PNG \
		images/small/datatypes_bool.PNG \
		images/small/datatypes_spatial.PNG \
		images/small/face-devilish.png \
		images/small/flag_estonia.PNG \
		images/small/flag_german1.PNG \
		images/small/datatypes_numeric.PNG \
		images/small/datatypes_text.PNG \
		images/small/flag_croatian.PNG \
		images/small/apply.png \
		images/small/flag_esperanto.PNG \
		images/small/datatypes_misc.PNG \
		images/small/flag_persian.PNG \
		images/small/flag_icelandic.PNG \
		images/small/flag_general.PNG \
		images/small/flag_german2.PNG \
		images/small/add.png \
		images/small/flag_slovak.PNG \
		images/small/flag_japanese.PNG \
		images/small/flag_romanian.PNG \
		images/small/flag_swedish.PNG \
		images/small/flag_czech.PNG \
		images/small/project_open.png \
		images/small/flag_polish.PNG \
		images/small/network_server_database.png \
		images/small/cancel.png
	/home/ferenc/qtsdk-2010.02/qt/bin/rcc -name dbm dbm.qrc -o qrc_dbm.cpp

compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_mainwindow.h ui_mainwindowbuttondialog.h ui_newprojectdialog.h ui_newdatatypeform.h ui_datatypeslistform.h ui_TablesListForm.h ui_NewTableForm.h
compiler_uic_clean:
	-$(DEL_FILE) ui_mainwindow.h ui_mainwindowbuttondialog.h ui_newprojectdialog.h ui_newdatatypeform.h ui_datatypeslistform.h ui_TablesListForm.h ui_NewTableForm.h
ui_mainwindow.h: mainwindow.ui
	/home/ferenc/qtsdk-2010.02/qt/bin/uic mainwindow.ui -o ui_mainwindow.h

ui_mainwindowbuttondialog.h: mainwindowbuttondialog.ui
	/home/ferenc/qtsdk-2010.02/qt/bin/uic mainwindowbuttondialog.ui -o ui_mainwindowbuttondialog.h

ui_newprojectdialog.h: newprojectdialog.ui
	/home/ferenc/qtsdk-2010.02/qt/bin/uic newprojectdialog.ui -o ui_newprojectdialog.h

ui_newdatatypeform.h: newdatatypeform.ui
	/home/ferenc/qtsdk-2010.02/qt/bin/uic newdatatypeform.ui -o ui_newdatatypeform.h

ui_datatypeslistform.h: datatypeslistform.ui
	/home/ferenc/qtsdk-2010.02/qt/bin/uic datatypeslistform.ui -o ui_datatypeslistform.h

ui_TablesListForm.h: TablesListForm.ui
	/home/ferenc/qtsdk-2010.02/qt/bin/uic TablesListForm.ui -o ui_TablesListForm.h

ui_NewTableForm.h: NewTableForm.ui
	/home/ferenc/qtsdk-2010.02/qt/bin/uic NewTableForm.ui -o ui_NewTableForm.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_rcc_clean compiler_uic_clean 

####### Compile

main.o: main.cpp mainwindow.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

mainwindow.o: mainwindow.cpp mainwindow.h \
		ui_mainwindow.h \
		mainwindowbuttondialog.h \
		newprojectdialog.h \
		datatypeslistform.h \
		newdatatypeform.h \
		UserDataType.h \
		DataType.h \
		strings.h \
		TreeItem.h \
		Project.h \
		DatabaseEngine.h \
		Version.h \
		AbstractDTSupplier.h \
		NewTableForm.h \
		Table.h \
		IconFactory.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mainwindow.o mainwindow.cpp

mainwindowbuttondialog.o: mainwindowbuttondialog.cpp mainwindowbuttondialog.h \
		ui_mainwindowbuttondialog.h \
		mainwindow.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mainwindowbuttondialog.o mainwindowbuttondialog.cpp

newprojectdialog.o: newprojectdialog.cpp newprojectdialog.h \
		ui_newprojectdialog.h \
		DatabaseEngine.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o newprojectdialog.o newprojectdialog.cpp

newdatatypeform.o: newdatatypeform.cpp newdatatypeform.h \
		ui_newdatatypeform.h \
		mainwindow.h \
		DatabaseEngine.h \
		DataType.h \
		strings.h \
		AbstractDTSupplier.h \
		AbstractCodepageSupplier.h \
		Codepage.h \
		UserDataType.h \
		TreeItem.h \
		Configuration.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o newdatatypeform.o newdatatypeform.cpp

datatypeslistform.o: datatypeslistform.cpp datatypeslistform.h \
		UserDataType.h \
		DataType.h \
		strings.h \
		TreeItem.h \
		ui_datatypeslistform.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o datatypeslistform.o datatypeslistform.cpp

AbstractDTSupplier.o: AbstractDTSupplier.cpp AbstractDTSupplier.h \
		DataType.h \
		strings.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o AbstractDTSupplier.o AbstractDTSupplier.cpp

DataType.o: DataType.cpp DataType.h \
		strings.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DataType.o DataType.cpp

MySQLDTSupplier.o: MySQLDTSupplier.cpp MySQLDTSupplier.h \
		AbstractDTSupplier.h \
		DataType.h \
		strings.h \
		UserDataType.h \
		TreeItem.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o MySQLDTSupplier.o MySQLDTSupplier.cpp

UserDataType.o: UserDataType.cpp UserDataType.h \
		DataType.h \
		strings.h \
		TreeItem.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o UserDataType.o UserDataType.cpp

Project.o: Project.cpp Project.h \
		AbstractDTSupplier.h \
		DataType.h \
		strings.h \
		MajorVersion.h \
		Version.h \
		DatabaseEngine.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Project.o Project.cpp

MajorVersion.o: MajorVersion.cpp MajorVersion.h \
		Version.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o MajorVersion.o MajorVersion.cpp

Version.o: Version.cpp Version.h \
		UserDataType.h \
		DataType.h \
		strings.h \
		TreeItem.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Version.o Version.cpp

AbstractCodepageSupplier.o: AbstractCodepageSupplier.cpp AbstractCodepageSupplier.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o AbstractCodepageSupplier.o AbstractCodepageSupplier.cpp

MysqlCodepageSupplier.o: MysqlCodepageSupplier.cpp MysqlCodepageSupplier.h \
		AbstractCodepageSupplier.h \
		Codepage.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o MysqlCodepageSupplier.o MysqlCodepageSupplier.cpp

Codepage.o: Codepage.cpp Codepage.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Codepage.o Codepage.cpp

DatabaseEngine.o: DatabaseEngine.cpp DatabaseEngine.h \
		AbstractCodepageSupplier.h \
		MySQLDTSupplier.h \
		AbstractDTSupplier.h \
		DataType.h \
		strings.h \
		MysqlCodepageSupplier.h \
		MySQLIndextypeProvider.h \
		AbstractIndextypeProvider.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DatabaseEngine.o DatabaseEngine.cpp

Configuration.o: Configuration.cpp Configuration.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Configuration.o Configuration.cpp

Table.o: Table.cpp Table.h \
		TreeItem.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Table.o Table.cpp

TablesListForm.o: TablesListForm.cpp TablesListForm.h \
		ui_TablesListForm.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o TablesListForm.o TablesListForm.cpp

NewTableForm.o: NewTableForm.cpp NewTableForm.h \
		ui_NewTableForm.h \
		IconFactory.h \
		strings.h \
		DatabaseEngine.h \
		Project.h \
		Version.h \
		UserDataType.h \
		DataType.h \
		TreeItem.h \
		Column.h \
		Table.h \
		mainwindow.h \
		AbstractIndextypeProvider.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o NewTableForm.o NewTableForm.cpp

Column.o: Column.cpp Column.h \
		TreeItem.h \
		UserDataType.h \
		DataType.h \
		strings.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Column.o Column.cpp

AbstractIndextypeProvider.o: AbstractIndextypeProvider.cpp AbstractIndextypeProvider.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o AbstractIndextypeProvider.o AbstractIndextypeProvider.cpp

Index.o: Index.cpp Index.h \
		TreeItem.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Index.o Index.cpp

moc_mainwindow.o: moc_mainwindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_mainwindow.o moc_mainwindow.cpp

moc_mainwindowbuttondialog.o: moc_mainwindowbuttondialog.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_mainwindowbuttondialog.o moc_mainwindowbuttondialog.cpp

moc_newprojectdialog.o: moc_newprojectdialog.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_newprojectdialog.o moc_newprojectdialog.cpp

moc_newdatatypeform.o: moc_newdatatypeform.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_newdatatypeform.o moc_newdatatypeform.cpp

moc_datatypeslistform.o: moc_datatypeslistform.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_datatypeslistform.o moc_datatypeslistform.cpp

moc_TablesListForm.o: moc_TablesListForm.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_TablesListForm.o moc_TablesListForm.cpp

moc_NewTableForm.o: moc_NewTableForm.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_NewTableForm.o moc_NewTableForm.cpp

qrc_dbm.o: qrc_dbm.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o qrc_dbm.o qrc_dbm.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:


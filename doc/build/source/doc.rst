************
Introduction
************

The scope of the document is to provide the necessary steps for you to be able to build DDM from the source code package, and to get it up and running on your Linux systems. 
The steps more or less should be the same on a Windows system too, but due to that Windows has a more approachable system for installing third party software we recommend for you to use
the already pre-packaged application. 

********************************
System requirements for building
********************************

For building DDM out of the source you will need to have installed on your system standard Linux development tools and the QT development libraries. Since this varies from 
Linux distro to Linux distro, we will focus our efforts on the two mainstream distros (which are using two different packagind system) so our instructions are here for:

    * Fedora 19
    * (K)Ubuntu 13.04
    
We did not had too much time while deploying this release, so on a standard Ubuntu, using Unity there is no Icon for the application. If you can live with this, feel free to use it.

This document assumes that you are using a freshly installed OS, if you already installed some of the packages below feel free to skip them. You will need to install soem development packages, the Qt development libraries, and also source control applications if you want to use the CUBRID database engine (or want to use the source code from the online repository instead of the source package).

Fedora (19)
===========

.. code-block:: bash

    sudo yum groupinstall 'Development Tools'
    sudo yum install gcc-c++ qt-devel qtwebkit-devel qt-config cmake mercurial git wget

(K)Ubuntu (13.04)
=================

.. code-block:: bash

    sudo apt-get install g++ libqt4-dev cmake libqt4-webkit ubuntu-dev-tools mercurial git

********
Building
********

Either you can use the source package for the latest version (available from https://sourceforge.net/projects/dbm-project/files/) or you can clone the mercurial repository to get the latest features too. However, when you clone the repository, bear in mind, that you are working on a product which is frequently developed changed, so it will be unstable. 

Cloning the repository
======================

.. code-block:: bash

    hg clone http://hg.code.sf.net/p/dbm-project/code dbm-project-code
    cd dbm-project-code/
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make
    sudo make install

Now your application should be installed into **/opt/ddm/**

Building from the source package
================================

.. code-block:: bash

    tar xjf dbm-project-0.1.k-src.tar.gz
    cd dbm-project-0.1.k-src
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make
    sudo make install

Now your application should be installed into **/opt/ddm/**

***********************
Running the application
***********************

In order to be able to effectively use the application in order to deploy databases you will need to install your platform specific MySql and CUBRID libraries. Usually newer Qt versions have built in Sqlite support, so there's nothing needed for this.

Fedora
==========

Install the qt-mysql package. Yes, we know this installs the mariadb packages, but (at this stage) mysql and mariadb are more or less compatible so these should function without any problems.

.. code-block:: bash

    sudo yum install qt-mysql

Ubuntu
=========

The following packages are required

.. code-block:: bash

    sudo apt-get install libqt4-sql-mysql libmysqlclient18

******************************
Supporting the CUBRID Database
******************************

In order for your system to support the CUBRID database engine you will need either to take our pre-compiled CUBRID package and place them in the correct location or compile the QCubrid package by yourself.

Using the pre-compiled packages
===============================

Our pre compiled CUBRID packages are built for Fedora (32 bit) and Ubuntu (32 bit, 64 bit) . The Fedora driver is linked against Qt 4.8.5 and the Ubuntu driver is linked against Qt 4.8.4 (32 bit) and 4.8.1 (the 64 bit). Fetch the one you require from https://sourceforge.net/projects/dbm-project/files/ and install it like:

.. code-block:: bash

    sudo tar xjf <YOUR_FILE>.tar.bz2 -C /

This will instruct tar to extract the file into the root directory since the package of the drivers contain all the required path information. In case your system config does not match exactly the above one, we highly reccomend manually building the driver in order to have it functioning.

Compiling QCubrid from source
=============================

Compiling the QCubrid driver requires you to have the Qt sources available, and also the sources for the QCubrid driver itself. We present here how to obtain them, and finally how to make the compilation itself.

Obtaining the Qt Source code
----------------------------

As a first step you will need to get the qt version you are using and download the proper package. This is done by using qmake.

.. rubric:: Fedora

.. code-block:: bash

    qmake-qt4 -query | grep QT_VERSION
    
This will give you a one line reply, such as:

.. code-block:: bash

    QT_VERSION:4.8.5

so download:

.. code-block:: bash

    wget http://download.qt-project.org/official_releases/qt/4.8/4.8.5/qt-everywhere-opensource-src-4.8.5.tar.gz
    
and extract it:

.. code-block:: bash

    tar xzf qt-everywhere-opensource-src-4.8.5.tar.gz

.. rubric:: Ubuntu

.. code-block:: bash

    qmake -query | grep QT_VERSION

This will give you a one line reply, such as:

.. code-block:: bash

    QT_VERSION:4.8.4

so download:

.. code-block:: bash

    wget http://download.qt-project.org/archive/qt/4.8/4.8.4/qt-everywhere-opensource-src-4.8.4.tar.gz
    
and extract it.

.. code-block:: bash

    tar xzf qt-everywhere-opensource-src-4.8.4.tar.gz
    
Obtaining the CUBRID CCI driver
-------------------------------

DDM supports CUBRID 9.1 so go to http://www.cubrid.org/?mid=downloads&item=cci_driver&os=detect&cubrid=9.1.0 and fetch the package which is for your operating system. I use the 32 bit OS so, the download link would be:

.. code-block:: bash

    wget http://ftp.cubrid.org/CUBRID_Drivers/CCI_Driver/9.1.0/CUBRID-CCI-9.1.0.0212-i386.tar.gz

and extract it:

.. code-block:: bash

    tar xzf CUBRID-CCI-9.1.0.0212-i386.tar.gz
    
This will give you a CUBRID directory with the following layout:

.. code-block:: none

    CUBRID
    ├── include
    │   ├── cas_cci.h
    │   └── cas_error.h
    └── lib
        ├── libcascci.a
        ├── libcascci.so -> libcascci.so.9.1.0
        ├── libcascci.so.9 -> libcascci.so.9.1.0
        └── libcascci.so.9.1.0

You will need to place the direvers to the proper location.

.. rubric:: Fedora

.. code-block:: bash

    sudo cp ./CUBRID/lib/libcascci.* /usr/lib
    
.. rubric:: Ubuntu

.. code-block:: bash

    sudo cp ./CUBRID/lib/* /usr/lib/i386-linux-gnu/

Obtaining the QCubrid driver
----------------------------

The QCubrid driver can be fetched from:

.. code-block:: bash

    git clone http://git.gitorious.org/qcubrid/qcubrid-fixes.git
    
Right now we are experiencing some problems with gitorious hosting (cannot merge), so till the gitorious support team struggles with this issue please use the fixes repository, we will update the documentation once the issues are sorted out.

This will give you the following driectory:

.. code-block:: none

    qcubrid-fixes
    ├── plugins
    │   └── sqldrivers
    │       └── cubrid
    │           ├── cubrid.pro
    │           └── main.cpp
    └── sql
        └── drivers
            └── cubrid
                ├── cubrid_qmake.txt
                ├── qsql_cubrid.cpp
                ├── qsql_cubrid.h
                └── qsql_cubrid.pri

Compiling the QCubrid driver
----------------------------

.. rubric:: Fedora

Firstly you will need to place the directories from the QCubrid into their proper location.

The **sql/drivers/cubrid** directory should go to **qt-everywhere-opensource-src-4.8.5/src/sql/drivers** and the **plugins/sqldrivers/cubrid** directory should go to **qt-everywhere-opensource-src-4.8.5/src/plugins/sqldrivers**.

so the commands are:

.. code-block:: bash

    cp -R qcubrid-fixes/sql/drivers/cubrid ./qt-everywhere-opensource-src-4.8.5/src/sql/drivers
    cp -R qcubrid-fixes/plugins/sqldrivers/cubrid ./qt-everywhere-opensource-src-4.8.5/src/plugins/sqldrivers
    
And now go to the **qt-everywhere-opensource-src-4.8.5/src/plugins/sqldrivers/cubrid** directory 

.. code-block:: bash

    cd qt-everywhere-opensource-src-4.8.5/src/plugins/sqldrivers/cubrid

and execute:

.. code-block:: bash

    qmake-qt4 "INCLUDEPATH+=/home/`whoami`/CUBRID/include" "LIBS+=-L/home/`whoami`/CUBRID/lib" cubrid.pro
    make
    sudo make install

The funny line is there because make needs a full path to the location of the directories. You should finally see a line at the end:

.. code-block:: bash

    install -m 755 -p "libqsqlcubrid.so" "/usr/lib/qt4/plugins/sqldrivers/libqsqlcubrid.so"

This means your library was succesfully installed.

.. rubric:: Ubuntu

The **sql/drivers/cubrid** directory should go to **qt-everywhere-opensource-src-4.8.4/src/sql/drivers** and the **plugins/sqldrivers/cubrid** directory should go to **qt-everywhere-opensource-src-4.8.4/src/plugins/sqldrivers**.

so the commands are:

.. code-block:: bash

    cp -R qcubrid-fixes/sql/drivers/cubrid ./qt-everywhere-opensource-src-4.8.4/src/sql/drivers
    cp -R qcubrid-fixes/plugins/sqldrivers/cubrid ./qt-everywhere-opensource-src-4.8.4/src/plugins/sqldrivers
    
And now go to the **qt-everywhere-opensource-src-4.8.4/src/plugins/sqldrivers/cubrid** directory 

.. code-block:: bash

    cd qt-everywhere-opensource-src-4.8.4/src/plugins/sqldrivers/cubrid

and execute:

.. code-block:: bash

    qmake "INCLUDEPATH+=/home/`whoami`/CUBRID/include" "LIBS+=-L/home/`whoami`/CUBRID/lib" cubrid.pro
    make
    sudo make install
    
When done, you should see:

.. code-block:: bash

    install -m 755 -p "libqsqlcubrid.so" "/usr/lib/i386-linux-gnu/qt4/plugins/sqldrivers/libqsqlcubrid.so"
    strip --strip-unneeded "/usr/lib/i386-linux-gnu/qt4/plugins/sqldrivers/libqsqlcubrid.so"
    
This means, you library was succesfully installed.


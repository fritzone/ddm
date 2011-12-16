; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=Database Deployment Manager
AppVersion=0.1f
AppVerName=Database Deployment Manager Version 0.1f
AppPublisher=The Unauthorized Frog Project
DefaultDirName={pf}\Database Deployment Manager
DefaultGroupName=Database Deployment Manager
UninstallDisplayIcon={app}\ddm.exe
Compression=lzma2
SolidCompression=yes
OutputDir=c:\ddm-dist
SourceDir=c:\ddm-dist
WizardImageFile=unaf.bmp
WizardImageStretch=no
WizardSmallImageFile=icon.bmp
WizardImageBackColor=clWhite
VersionInfoVersion=0.1.0.0
VersionInfoTextVersion=0.1f
ShowLanguageDialog=no
LicenseFile=lgpl.txt
FlatComponentsList=no  

[Files]
Source: "codepages.mysql.dat"; DestDir: "{app}"
Source: "ddm.cfx"; DestDir: "{app}"
Source: "ddm.exe"; DestDir: "{app}"; DestName: "ddm.exe"
Source: "ddm.iss"; DestDir: "{app}"
Source: "ddm_connections.cfx"; DestDir: "{app}"
Source: "libgcc_s_dw2-1.dll"; DestDir: "{app}"
Source: "libmysql.dll"; DestDir: "{app}"
Source: "mingwm10.dll"; DestDir: "{app}"
Source: "phonon4.dll"; DestDir: "{app}"
Source: "qt.conf"; DestDir: "{app}"
Source: "QtCore4.dll"; DestDir: "{app}"
Source: "QtGui4.dll"; DestDir: "{app}"
Source: "QtNetwork4.dll"; DestDir: "{app}"
Source: "QtSql4.dll"; DestDir: "{app}"
Source: "QtSvg4.dll"; DestDir: "{app}"
Source: "QtWebKit4.dll"; DestDir: "{app}"
Source: "QtXml4.dll"; DestDir: "{app}"
Source: "sqldrivers\qsqlmysql4.dll"; DestDir: "{app}\sqldrivers"
Source: "rsrc\mysql.defaults"; DestDir: "{app}\rsrc"
Source: "doc\*"; DestDir: "{app}\doc"

[Icons]
Name: "{group}\Database Deployment Manager"; Filename: "{app}\ddm.exe"
Name: "{group}\Uninstall Database Deployment Manager"; Filename: "{uninstallexe}"
Name: "{commondesktop}\Database Deployment Manager"; Filename: "{app}\ddm.exe"

[Messages]
BeveledLabel=(c) 2011 The Unauthorized Frog Project

[CustomMessages]
CreateDesktopIcon=Create a &desktop icon

[Tasks]
Name: desktopicon; Description: "{cm:CreateDesktopIcon}"

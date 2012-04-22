; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMFCKit2004View
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "MFCKit2004.h"
LastPage=0

ClassCount=7
Class1=CMFCKit2004App
Class2=CMFCKit2004Doc
Class3=CMFCKit2004View
Class4=CMainFrame

ResourceCount=3
Resource1=IDR_POPUPMENU
Class5=CAboutDlg
Class6=CPopup
Resource2=IDR_MAINFRAME
Class7=CNothing
Resource3=IDD_ABOUTBOX

[CLS:CMFCKit2004App]
Type=0
HeaderFile=MFCKit2004.h
ImplementationFile=MFCKit2004.cpp
Filter=N

[CLS:CMFCKit2004Doc]
Type=0
HeaderFile=MFCKit2004Doc.h
ImplementationFile=MFCKit2004Doc.cpp
Filter=N
LastObject=CMFCKit2004Doc

[CLS:CMFCKit2004View]
Type=0
HeaderFile=MFCKit2004View.h
ImplementationFile=MFCKit2004View.cpp
Filter=C
LastObject=ID_NEWSTUFF_J
BaseClass=CView
VirtualFilter=VWC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T




[CLS:CAboutDlg]
Type=0
HeaderFile=MFCKit2004.cpp
ImplementationFile=MFCKit2004.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_VIEW_TOOLBAR
Command12=ID_VIEW_STATUS_BAR
Command13=ID_MODE_VIEW
Command14=ID_MODE_CONTROL
Command15=ID_DEMO_POLYLINE
Command16=ID_DEMO_ANIMATION
Command17=ID_DEMO_NONE
Command18=ID_APP_ABOUT
Command19=ID_NEWSOMETHING_VVV
CommandCount=19

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[CLS:CPopup]
Type=0
HeaderFile=Popup.h
ImplementationFile=Popup.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_POPUP_CLOSE

[MNU:IDR_POPUPMENU]
Type=1
Class=CMFCKit2004View
Command1=ID_POPUP_CLOSE
Command2=ID_STUFF_2
Command3=ID_STUFF_3
Command4=ID_STUFF_4
Command5=ID_STUFF_5
CommandCount=5

[CLS:CNothing]
Type=0
HeaderFile=Nothing.h
ImplementationFile=Nothing.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CNothing


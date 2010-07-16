
#CONFIG		+= qt debug_and_release warn_on build_all
#CONFIG		+= qt release warn_on build_all

CONFIG += static 
#CONFIG += staticlib

QT += core gui xml
TARGET 	= gpx2map


# copy from qtdemo
RESOURCES = gpx2map.qrc
win32:RC_FILE = gpx2map.rc
mac {
ICON = qtdemo.icns
QMAKE_INFO_PLIST = Info_mac.plist
}

HEADERS = main_window.h
SOURCES	= main_window.cpp \
          main.cpp



	

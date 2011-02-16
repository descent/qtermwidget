
#CONFIG		+= qt debug_and_release warn_on build_all
#CONFIG		+= qt release warn_on build_all

#CONFIG += static 
#CONFIG += staticlib
CONFIG      += console debug
QT += core gui xml network webkit
TARGET 	= gpx2map


# copy from qtdemo
RESOURCES = gpx2map.qrc
win32:RC_FILE = gpx2map.rc
mac {
ICON = qtdemo.icns
QMAKE_INFO_PLIST = Info_mac.plist
}

HEADERS = main_window.h \
          route_item.h \
          browserwindow.hpp \
	  kuhn_munkres.hpp \
	  alt_key.hpp

SOURCES	= main_window.cpp \
          browserwindow.cpp \
	  kuhn_munkres.cpp \
	  alt_key.cpp \
          main.cpp




	

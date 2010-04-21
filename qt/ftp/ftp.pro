#CONFIG += qt debug
TARGET = aftp
CONFIG += qt 
HEADERS       = ftpwindow.h\
		file_list_widget.h\
		ftp_server_list.h\
                tree_view_popup_menu.h\
		ftp_transfer.h \
		ftp_bookmark.h \
		rename_dialog.h
# ref QString document
# do not do any encoding conversion in QString
#DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
SOURCES       =  main.cpp \
		 ftpwindow.cpp \
		 file_list_widget.cpp\
		 ftp_server_list.cpp\
		 tree_view_popup_menu.cpp\
		 ftp_transfer.cpp\
		ftp_bookmark.cpp \
		rename_dialog.cpp

RESOURCES    += ftp.qrc
QT           += network


unix:INCLUDEPATH += /usr/local/include

#unix:LIBS += -L/usr/local/lib -lmath
#win32:LIBS += c:/mylibs/math.lib

unix:LIBS += -L/usr/local/lib -liconv
unix:DEFINES += UNIX_PATH
win32:LIBS += -liconv
win32:DEFINES += ICONV_CONST=const
win32:DEFINES += WIN_PATH
macx:DEFINES += ICONV_CONST=const
macx:DEFINES += MAC_ENC
# for windows debug (qDebug)

#win32:debug {
#  CONFIG += console
#}


# install
#target.path = $$[QT_INSTALL_EXAMPLES]/network/ftp
#sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS ftp.pro images
#sources.path = $$[QT_INSTALL_EXAMPLES]/network/ftp
#INSTALLS += target sources

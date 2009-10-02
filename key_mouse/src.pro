TEMPLATE	= app
DESTDIR 	= . 

CONFIG		+= qt debug_and_release warn_on build_all

QT += core gui

MOC_DIR 	= .

CONFIG(debug, debug|release) {
    OBJECTS_DIR = .
    TARGET 	= key_mouse_d
} else {
    OBJECTS_DIR = .
    TARGET 	= key_mouse
}

HEADERS       = config_widget.h
SOURCES 	= main.cpp config_widget.cpp
#SOURCES 	= main.cpp 

INCLUDEPATH 	= 

#LIBS 		+= -L.. -lqtermwidget


	

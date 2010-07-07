
#CONFIG		+= qt debug_and_release warn_on build_all
#CONFIG		+= qt release warn_on build_all

CONFIG += static 
#CONFIG += staticlib

QT += core gui xml
TARGET 	= qt_tes

# Flex/bison specifics
 
#LEX = flex
#QMAKE_LEX = flex
#QMAKE_YACC = bison

#LEXSOURCES += tes.l
FLEXSOURCES += tes.l


# ref: http://www.qtcentre.org/threads/3557-Flex-Bison-and-qmake
#flex.commands = flex --header-file=tes.h -o tes.cpp ${QMAKE_FILE_IN}
flex.commands = flex --header-file=tes.h -o lex.yy.cpp ${QMAKE_FILE_IN}
flex.input = FLEXSOURCES
flex.output = lex.yy.cpp
flex.variable_out = SOURCES
flex.name = flex
QMAKE_EXTRA_COMPILERS += flex



#YACCSOURCES += 

#MOC_DIR 	= ./.moc

#CONFIG(debug, debug|release) {
#    OBJECTS_DIR = ./.objs_d
#    TARGET 	= rich8_edit_save_d
##    LIBS 	+= -L.. ../libqtermwidget_d.a
#} else {
#    OBJECTS_DIR = ./.objs
#    TARGET 	= rich8_edit_save
##    LIBS 	+= -L.. ../libqtermwidget.a
#}
#TEMPLATE	= app
#DESTDIR 	= .

# flex library
LIBS += -ll

HEADERS = main_window.h
SOURCES	= main_window.cpp \
          main.cpp



	

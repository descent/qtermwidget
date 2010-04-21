singleprocess:singleprocess=true
TEMPLATE     = lib
CONFIG      += qtopia warn_on release
win32:CONFIG += dll
win32:DEFINES += QTOPIA_PLUGIN_MAKEDLL QTOPIA_DLL
HEADERS      =  qpe_input_impl.h\
		qpe_input_w.h \
		input_handler.h \
		etphonetic_input.h \
		big5.h \
		et_phonetic.h \
		etchangjei_input.h \
		et_changjei.h

SOURCES      =  qpe_input_impl.cpp\
		qpe_input_w.cpp \
		input_handler.cpp \
		etphonetic_input.cpp \
		big5.cpp \
		et_phonetic.cpp \
		etchangjei_input.cpp \
		et_changjei.cpp


TARGET       = qpeinput
DESTDIR      = $(QPEDIR)/plugins/inputmethods
VERSION      = 1.0.0



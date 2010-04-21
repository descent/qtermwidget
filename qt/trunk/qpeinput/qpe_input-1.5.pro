singleprocess:singleprocess=true
TEMPLATE     = lib
CONFIG      += qt warn_on release
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



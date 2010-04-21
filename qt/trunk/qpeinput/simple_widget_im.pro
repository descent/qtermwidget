TEMPLATE     = lib
CONFIG      += qt warn_on release
HEADERS      =  simple_widget_im.h\
		qpe_input_w.h \

SOURCES      =  simple_widget_im.cpp\
		qpe_input_w.cpp \


TARGET       = qpeinput
DESTDIR      = $(QPEDIR)/plugins/inputmethods
INCLUDEPATH += $(QPEDIR)/include
LIBS        += -lqpe
VERSION      = 1.0.0



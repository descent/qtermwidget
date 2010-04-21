# make ARCH=arm-linux-
# make APP_TYPE=qtopia to build qtopia app
# else build qt/x11 app
ARCH=
CXX=$(ARCH)g++
MOC=$(QTDIR)/bin/moc
OBJS= qt_key.o\
     moc_qt_key.o\
     main.lo

LOBJS=qt_key.lo\
     moc_qt_key.lo\
     main.lo

#STRIP=$(ARCH)strip
STRIP=
LIBTOOL=libtool
LIBTOOL_COMPILE=--tag=CXX --mode=compile
LIBTOOL_LINK=--tag=CXX --mode=link

APP_TYPE=

ifeq ($(APP_TYPE), qtopia)

LDFLAGS=-L$(QTDIR)/lib -L$(QPEDIR)/lib 
QTE_CXXFLAGS=-DQWS -fno-exceptions -fno-rtti
LIBS=-lopie -lqte -lqpe 
QPE_DEF=-DQTOPIA_APP

else # qt x11

LDFLAGS=-L$(QTDIR)/lib -L$(QPEDIR)/lib -L/usr/X11R6/lib
QTE_CXXFLAGS=
LIBS=-lqt -lXrender -lXrandr -lXcursor -lXinerama -lXft -lfreetype -lfontconfig -lXext -lX11 -lm -lSM -lICE -ldl 

endif

TARGET=qtkey

CXXFLAGS=-I$(QTDIR)/include -I$(QPEDIR)/include $(QTE_CXXFLAGS) -g -Wall $(QPE_DEF) -static
#CXXFLAGS=-I$(QTDIR)/include -I$(QPEDIR)/include -DQWS -g -Wall

$(TARGET) : $(LOBJS)
	$(LIBTOOL) $(LIBTOOL_LINK) $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS) && $(STRIP) $@


main.lo:main.cpp qt_key.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

qt_key.lo:qt_key.cpp qt_key.h 
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_qt_key.cpp:qt_key.h
	$(MOC) -o $@ $<

moc_qt_key.lo:moc_qt_key.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.lo *.lo moc_* $(TARGET)

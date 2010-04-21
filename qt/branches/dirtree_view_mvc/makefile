# make ARCH=arm-linux-
# make APP_TYPE=qtopia to build qtopia app
# else build qt/x11 app
ARCH=
CXX=$(ARCH)g++
MOC=$(QTDIR)/bin/moc
OBJS=dirtree_view.o \
     moc_dirtree_view.o \
     font_selector.o \
     moc_font_selector.o \
     rename_line_edit.o \
     moc_rename_line_edit.o \
     image_widget.o\
     moc_image_widget.o\
     convertencode.o\
     main.o

STRIP=$(ARCH)strip
LIBTOOL=libtool
LIBTOOL_COMPILE=--mode=compile
LIBTOOL_LINK=--mode=link
#STRIP=

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

TARGET=filemanager

CXXFLAGS=-I$(QTDIR)/include -I$(QPEDIR)/include $(QTE_CXXFLAGS) -g -Wall $(QPE_DEF)
#CXXFLAGS=-I$(QTDIR)/include -I$(QPEDIR)/include -DQWS -g -Wall

$(TARGET) : $(OBJS)
	$(LIBTOOL) $(LIBTOOL_LINK) $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS) && $(STRIP) $@


main.o:main.cpp 
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

dirtree_view.o:dirtree_view.cpp dirtree_view.h big5str_to_qstr.h font_selector.h rename_line_edit.h big5str_to_qstr.h image_widget.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_dirtree_view.cpp:dirtree_view.h
	$(MOC) -o $@ $<
moc_dirtree_view.o:moc_dirtree_view.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

font_selector.o:font_selector.cpp font_selector.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_font_selector.cpp:font_selector.h
	$(MOC) -o $@ $<

moc_font_selector.o: moc_font_selector.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_rename_line_edit.cpp:rename_line_edit.h
	$(MOC) -o $@ $<

moc_rename_line_edit.o:moc_rename_line_edit.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

rename_line_edit.o:rename_line_edit.cpp rename_line_edit.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

convertencode.o:convertencode.cpp convertencode.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

image_widget.o:image_widget.cpp image_widget.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<
file_list.o:file_list.cpp file_list.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<
observer.o:observer.cpp observer.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<
subject.o:subject.cpp subject.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_image_widget.cpp:image_widget.h
	$(MOC) -o $@ $<
moc_image_widget.o:moc_image_widget.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o moc_* $(TARGET)

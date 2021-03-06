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
     confirm_dialog.o\
     moc_confirm_dialog.o\
     convertencode.o\
     file_monitor.o\
     moc_file_monitor.o\
     main.o

LOBJS=dirtree_view.lo \
     moc_dirtree_view.lo \
     fo_listview_item.lo \
     moc_fo_listview_item.lo \
     font_selector.lo \
     moc_font_selector.lo \
     rename_line_edit.lo \
     moc_rename_line_edit.lo \
     image_widget.lo\
     moc_image_widget.lo\
     confirm_dialog.lo\
     moc_confirm_dialog.lo\
     convertencode.lo\
     file_monitor.lo\
     moc_file_monitor.lo\
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
LIBS=-lopie -lqte -lqpe -lfam
QPE_DEF=-DQTOPIA_APP

else # qt x11

LDFLAGS=-L$(QTDIR)/lib -L$(QPEDIR)/lib -L/usr/X11R6/lib
QTE_CXXFLAGS=
LIBS=-lqt -lXrender -lXrandr -lXcursor -lXinerama -lXft -lfreetype -lfontconfig -lXext -lX11 -lm -lSM -lICE -ldl -lfam

endif

TARGET=filemanager

CXXFLAGS=-I$(QTDIR)/include -I$(QPEDIR)/include $(QTE_CXXFLAGS) -g -Wall $(QPE_DEF) -static
#CXXFLAGS=-I$(QTDIR)/include -I$(QPEDIR)/include -DQWS -g -Wall

$(TARGET) : $(LOBJS)
	$(LIBTOOL) $(LIBTOOL_LINK) $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS) && $(STRIP) $@


main.lo:main.cpp 
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

dirtree_view.lo:dirtree_view.cpp dirtree_view.h big5str_to_qstr.h font_selector.h rename_line_edit.h big5str_to_qstr.h image_widget.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_dirtree_view.cpp:dirtree_view.h
	$(MOC) -o $@ $<

moc_dirtree_view.lo:moc_dirtree_view.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

fo_listview_item.lo:fo_listview_item.cpp fo_listview_item.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_fo_listview_item.cpp: fo_listview_item.h
	$(MOC) -o $@ $<

moc_fo_listview_item.lo: moc_fo_listview_item.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<


font_selector.lo:font_selector.cpp font_selector.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_font_selector.cpp:font_selector.h
	$(MOC) -o $@ $<

moc_font_selector.lo: moc_font_selector.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_rename_line_edit.cpp:rename_line_edit.h
	$(MOC) -o $@ $<

moc_rename_line_edit.lo:moc_rename_line_edit.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

rename_line_edit.lo:rename_line_edit.cpp rename_line_edit.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

convertencode.lo:convertencode.cpp convertencode.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

image_widget.lo:image_widget.cpp image_widget.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<
file_list.lo:file_list.cpp file_list.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<
observer.lo:observer.cpp observer.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<
subject.lo:subject.cpp subject.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_image_widget.cpp:image_widget.h
	$(MOC) -o $@ $<
moc_image_widget.lo:moc_image_widget.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

confirm_dialog.lo:confirm_dialog.cpp confirm_dialog.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

file_monitor.lo: file_monitor.cpp file_monitor.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_file_monitor.cpp: file_monitor.h
	$(MOC) -o $@ $<
moc_file_monitor.lo: moc_file_monitor.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_confirm_dialog.cpp:confirm_dialog.h
	$(MOC) -o $@ $<
moc_confirm_dialog.lo:moc_confirm_dialog.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.lo *.lo moc_* $(TARGET)

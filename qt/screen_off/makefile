# make ARCH=arm-linux-
# make APP_TYPE=qtopia to build qtopia app
# else build qt/x11 app
ARCH=
#CXX=$(ARCH)g++-4.0
CXX=$(ARCH)g++
MOC=$(QTDIR)/bin/moc
OBJS=dirtree_view.o \
     moc_dirtree_view.o \
     font_selector.o \
     moc_font_selector.o \
     rename_line_edit.o \
     moc_rename_line_edit.o \
     image_widget.o\
     grid_thumb.o\
     moc_grid_thumb.o\
     moc_image_widget.o\
     convertencode.o\
     file_monitor.o\
     moc_file_monitor.o\
     main.o

LOBJS=dirtree_view.lo \
     moc_dirtree_view.lo \
     encode_dialog.lo \
     moc_encode_dialog.lo \
     fo_listview_item.lo \
     moc_fo_listview_item.lo \
     font_selector.lo \
     moc_font_selector.lo \
     rename_line_edit.lo \
     moc_rename_line_edit.lo \
     image_widget.lo\
     grid_thumb.lo\
     moc_grid_thumb.lo\
     moc_image_widget.lo\
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


LDFLAGS=-L$(QTDIR)/lib -L$(QPEDIR)/lib 
QTE_CXXFLAGS=-DQWS -fno-exceptions -fno-rtti

TARGET=filemanager

CXXFLAGS=-I$(QTDIR)/include -I$(QPEDIR)/include $(QTE_CXXFLAGS) -g -Wall $(QPE_DEF) 
#CXXFLAGS=-I$(QTDIR)/include -I$(QPEDIR)/include -DQWS -g -Wall

$(TARGET) : $(LOBJS)
	$(LIBTOOL) $(LIBTOOL_LINK) $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS) && $(STRIP) $@


main.lo:main.cpp 
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

dirtree_view.lo:dirtree_view.cpp dirtree_view.h big5str_to_qstr.h font_selector.h rename_line_edit.h encode_dialog.h image_widget.h 
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

encode_dialog.lo:encode_dialog.cpp encode_dialog.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_encode_dialog.cpp: encode_dialog.h
	$(MOC) -o $@ $<

moc_encode_dialog.lo: moc_encode_dialog.cpp
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

grid_thumb.lo:grid_thumb.cpp grid_thumb.h
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<
moc_grid_thumb.cpp:grid_thumb.h
	$(MOC) -o $@ $<

moc_grid_thumb.lo:moc_grid_thumb.cpp
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

screen: screen.o moc_screen_widget.o screen_widget.o
	$(CXX) $(LDFLAGS) -o $@ $^ -lqte
screen.o: screen.cpp
	$(CXX) $(CXXFLAGS) -c $<

screen_widget.o: screen_widget.cpp screen_widget.h
	$(CXX) $(CXXFLAGS) -c $<

moc_screen_widget.cpp:screen_widget.h
	$(MOC) -o $@ $<

moc_screen_widget.o:moc_screen_widget.cpp
	$(CXX) $(CXXFLAGS) -c $<

moc_file_monitor.lo: moc_file_monitor.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

moc_confirm_dialog.cpp:confirm_dialog.h
	$(MOC) -o $@ $<
moc_confirm_dialog.lo:moc_confirm_dialog.cpp
	$(LIBTOOL) $(LIBTOOL_COMPILE) $(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o *.lo *.lo moc_* $(TARGET)

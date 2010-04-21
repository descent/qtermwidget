CXX=g++

INC=-I/usr/include/qt3

CXXFLAGS=$(INC)

qcdsee:qcdsee.o main_win.o 
	libtool --mode=link $(CXX) $(CXXFLAGS) -o $@ $^ -lqt-mt

qcdsee.o:qcdsee.cpp main_win.h
	libtool --mode=compile $(CXX) $(CXXFLAGS) -c $<

main_win.o: main_win.cpp main_win.h
	libtool --mode=compile $(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o



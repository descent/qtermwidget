CONFIG		+= qtopiaapp

HEADERS		= TEWidget.h \
                  input_method_w.h \
		  screen.h\
		  #subject.h\
		  #observer.h\
		  temulation_parser.h\
		  MyPty.h

SOURCES		= TEWidget.cpp \
                  input_method_w.cpp \
		  temulation_parser.cpp\
		  #subject.cpp\
		  screen.cpp\
		  main.cpp

TRANSLATABLES += $${HEADERS} \
                    $${SOURCES} \
                    MyPty.cpp

unix:SOURCES  += MyPty.cpp 


TARGET		= embeddedkonsole

pics.files=$${QTOPIA_DEPOT_PATH}/pics/embeddedkonsole/*
pics.path=/pics/embeddedkonsole
desktop.files=$${QTOPIA_DEPOT_PATH}/apps/Applications/embeddedkonsole.desktop
desktop.path=/apps/Applications
keytabs.file=$${QTOPIA_DEPOT_PATH}/etc/keytabs
keytabs.path=/etc
INSTALLS+=desktop keytabs
PICS_INSTALLS+=pics

contains(QMAKE_ARCH,x86)|contains(QMAKE_ARCH,generic)|contains(QMAKE_ARCH,ipaq)|contains(QMAKE_ARCH,sharp) {
    LIBS+=-lutil
    DEFINES+=HAVE_OPENPTY
}

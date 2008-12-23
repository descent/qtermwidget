#!/bin/sh

QT=""

if [ "$1" = "" ] ; then
  echo "need 1 argument "
  exit 1
fi

if [ "$2" = "" ] ; then
  QT=Qt-4.4.3
fi
echo /usr/local/Trolltech/$QT/bin/qmake $1
/usr/local/Trolltech/$QT/bin/qmake $1

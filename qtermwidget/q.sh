#!/bin/sh

QT=""

if [ "$1" = "" ] ; then
  echo "usage:"
  echo "$0 src.pro [QTVERSION]"
  echo "$0 src.pro QT=Qt-4.4.3"
  exit 1
fi

if [ "$2" = "" ] ; then
  QT=Qt-4.4.3
else
  QT=$2
fi
echo /usr/local/Trolltech/$QT/bin/qmake $1
/usr/local/Trolltech/$QT/bin/qmake $1

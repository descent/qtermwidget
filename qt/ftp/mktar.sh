#!/bin/sh
cd ftp
make distclean
cd ..
#tar jcvf ftp.`date +%F`.tar.bz2 ftp
tar jcvf ftp.`date +%G-%m-%d_%H-%M-%S`.tar.bz2 ftp

#!/bin/sh
make maintainer-clean
rm -rf aclocal.m4  compile  config.h.in \
    configure   depcomp  install-sh \
     Makefile.in  missing
find -name "Makefile.in" -exec rm {} \;


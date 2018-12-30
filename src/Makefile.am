AM_CPPFLAGS = -I$(srcdir)/../lib  -I$(srcdir)

lib_LTLIBRARIES = libtunet.la
libtunet_la_SOURCES = tunet.c tunet.h
libtunet_la_LIBADD = ../lib/libportal.la $(CRYPTOLIB) $(CURLLIB)

bin_PROGRAMS = tunet
tunet_SOURCES = cli.c
tunet_LDADD = ./libtunet.la

include_HEADERS = tunet.h

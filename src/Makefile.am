lib_LTLIBRARIES = libtunet.la
libtunet_la_CPPFLAGS = -I$(srcdir) -I$(srcdir)/../lib \
                       -I$(srcdir)/../utf8proc -I$(srcdir)/../sds -I$(srcdir)/../cJSON \
                       -I$(srcdir)/../tidy-html5/include
libtunet_la_SOURCES = tunet.c tunet.h
libtunet_la_LIBADD = ../lib/libportal.la $(CRYPTOLIB) $(CURLLIB)

bin_PROGRAMS = tunet
tunet_SOURCES = cli.c
tunet_LDADD = ./libtunet.la
tunet_LDFLAGS = -L$(prefix)/lib -Wl,-rpath -Wl,$(prefix)/lib

include_HEADERS = tunet.h

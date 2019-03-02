lib_LTLIBRARIES = libtunet.la
libtunet_la_CPPFLAGS = -I$(srcdir)/../lib  -I$(srcdir)
libtunet_la_SOURCES = tunet.c tunet.h
libtunet_la_LIBADD = ../lib/libportal.la $(CRYPTOLIB) $(CURLLIB)
if WINDOWS
libtunet_la_LDFLAGS = -no-undefined
libtunet_la_CPPFLAGS += -DTUNET_EXPORTS
endif

if WINDOWS
else
bin_PROGRAMS = tunet
tunet_SOURCES = cli.c
tunet_LDADD = ./libtunet.la
tunet_LDFLAGS = -L$(prefix)/lib -Wl,-rpath -Wl,$(prefix)/lib
endif

include_HEADERS = tunet.h

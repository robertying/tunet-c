libportal_la_CPPFLAGS = -I$(srcdir)/../openssl/include -I$(srcdir)/../openssl/include/openssl -I$(srcdir)/../tidy-html5/include
noinst_LTLIBRARIES = libportal.la
libportal_la_SOURCES = portal.c portal.h list.c list.h utf.c utf.h parser.c parser.h utf8proc.c utf8proc.h sds.c sds.h cJSON.c cJSON.h
libportal_la_LIBADD = ../tidy-html5/install/lib/libtidys.a
if WINDOWS
libportal_la_CPPFLAGS += -DUTF8PROC_EXPORTS
endif

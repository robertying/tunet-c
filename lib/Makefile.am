noinst_LTLIBRARIES = libportal.la
libportal_la_SOURCES = portal.c portal.h list.c list.h utf.c utf.h parser.c parser.h \
                       $(srcdir)/../utf8proc/utf8proc.c $(srcdir)/../utf8proc/utf8proc.h \
                       $(srcdir)/../sds/sds.c $(srcdir)/../sds/sds.h \
                       $(srcdir)/../cJSON/cJSON.c $(srcdir)/../cJSON/cJSON.h
libportal_la_LIBADD = -ltidys
libportal_la_CPPFLAGS = -I$(srcdir)/../utf8proc -I$(srcdir)/../sds -I$(srcdir)/../cJSON

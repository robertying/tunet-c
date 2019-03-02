libportal_la_CPPFLAGS = -I$(srcdir)/../openssl/include -I$(srcdir)/../openssl/include/openssl
noinst_LTLIBRARIES = libportal.la
libportal_la_SOURCES = portal.c portal.h list.c list.h utf.c utf.h utf8proc.c utf8proc.h sds.c sds.h cJSON.c cJSON.h
if WINDOWS
libportal_la_CPPFLAGS += -DUTF8PROC_EXPORTS
endif

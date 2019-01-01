AC_INIT([tunet], [1.1.0], [github.com/robertying/tunet-c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_CHECK_LIB([crypto], [SHA1], [CRYPTOLIB=-lcrypto])
AC_CHECK_LIB([curl], [curl_easy_setopt], [CURLLIB=-lcurl])
AC_SUBST([CRYPTOLIB])
AC_SUBST([CURLLIB])

AM_PROG_AR
LT_INIT
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile lib/Makefile src/Makefile])
AC_OUTPUT

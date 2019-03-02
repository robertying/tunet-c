AC_INIT([tunet], [2.0.0], [github.com/robertying/tunet-c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_CHECK_LIB([crypto], [SHA1], [CRYPTOLIB=-lcrypto], [AC_MSG_ERROR([Cannot find libopenssl])])
AC_CHECK_LIB([curl], [curl_easy_setopt], [CURLLIB=-lcurl], [AC_MSG_ERROR([Cannot find libcurl])])
AC_SUBST([CRYPTOLIB])
AC_SUBST([CURLLIB])

AC_CANONICAL_HOST
build_linux=no
build_windows=no
case "${host_os}" in
    darwin*)
        build_linux=yes
        ;;
    linux*)
        build_linux=yes
        ;;
    cygwin*|mingw*)
        build_windows=yes
        ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
        ;;
esac
AM_CONDITIONAL([LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([WINDOWS], [test "$build_windows" = "yes"])

AM_PROG_AR
LT_INIT([win32-dll])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile lib/Makefile src/Makefile])
AC_OUTPUT

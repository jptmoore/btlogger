AC_INIT(btlogger, 0.3.4, jmoore@zedstar.org)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/])
AM_CONFIG_HEADER([src/config.h])

AC_DEFINE([PROGNAME],["btlogger"],["simple bluetooth to sqlite logger"])

AC_DEFINE([DB],["/var/lib/misc/btlog.db"],["default location for database"])

AC_DEFINE([CONSUMER_KEY],["edit me"],["Consumer key"])

AC_DEFINE([CONSUMER_SECRET],["edit me"],["Consumer secret"])

AC_DEFINE([SCAN_FREQ],[20],["seconds to wait between scans"])

AC_DEFINE([API_URL],["https://twitter.com/"],["RESTful API"])

# --enable-debug flag
AC_ARG_ENABLE(debug,
    [  --enable-debug     Turn on debugging [[default=no]]],
    enable_debug="$enableval",
    enable_debug=no)

if test x$enable_debug = xyes ; then
    AC_DEFINE([DEBUG_MODE], [], [true])
fi


# Checks for programs.
AC_PROG_CC

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

# check for sqlite3
PKG_CHECK_MODULES(SQLITE3, [sqlite3])

# check for gthread
PKG_CHECK_MODULES(GTHREAD, gthread-2.0)

# check for librest
PKG_CHECK_MODULES(LIBREST, rest-0.7)

# check for glib
PKG_CHECK_MODULES(GLIB, [glib-2.0])

# check for dbus
PKG_CHECK_MODULES(DBUS, [dbus-1 dbus-glib-1])

# check for libsoup
PKG_CHECK_MODULES(LIBSOUP, [libsoup-2.4])

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT

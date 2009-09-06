INCLUDES = -Wall $(GLIB_CFLAGS) $(DBUS_CFLAGS) $(SQLITE3_CFLAGS) $(LIBSOUP_CFLAGS)

bin_PROGRAMS = btlogger

btlogger_SOURCES = scan.c scan.h \
											log.c log.h \
											marshal.c marshal.h \
											tweet.c tweet.h \
											config.h \
											btlog.db \
											main.c
											
btlogger_LDADD = $(GLIB_LIBS) $(DBUS_LIBS) $(SQLITE3_LIBS) $(LIBSOUP_LIBS)

btlogger_DATA = btlog.db

btloggerdir = /var/lib/misc
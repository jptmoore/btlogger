// License: GPLv3
// Copyright 2009 John P. T. Moore
// jmoore@zedstar.org

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sqlite3.h>
#include <dbus/dbus-glib.h>
#include <glib.h>
#include <libsoup/soup.h>
#include "scan.h"
#include "log.h"
#include "marshal.h"
#include "tweet.h"
#include "config.h"

#define log_output(fmtstr, args...) \
  (g_print(PROGNAME ":%s: " fmtstr "\n", __func__, ##args))

int main(int argc, char** argv)
{
  GMainLoop *mainloop = NULL;
  GError *error = NULL;
  DBusGConnection *bus = NULL;
  btloggerObject *bobj = NULL;
  gchar *id = NULL;
  gchar *name = NULL;
  gchar *user = NULL;
  gchar *pass = NULL;
  GOptionContext *context;

  gboolean verbose = FALSE;
  gint scan_freq = SCAN_FREQ;
  gchar *database = DB;
  sqlite3 *db;  
  
  
  GOptionEntry entries[] = 
  {
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Verbose output", NULL },
    { "scan frequency", 's', 0, G_OPTION_ARG_INT, &scan_freq, "Scan every N seconds", "N" },
    { "database", 'd', 0, G_OPTION_ARG_FILENAME, &database, "sqlite database", NULL },
    { "user", 'u', 0, G_OPTION_ARG_STRING, &user, "Twitter username", NULL },
    { "pass", 'p', 0, G_OPTION_ARG_STRING, &pass, "Twitter password", NULL },
    { "id", 'i', G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING, &id, "unique id", NULL },
    { "name", 'n', G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING, &name, "name", NULL },
    { NULL }
  };


  context = g_option_context_new ("- log bluetooth devices");
  g_option_context_add_main_entries (context, entries, PACKAGE_NAME);
  
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr("option parsing failed: %s\n", error->message);
    exit (EXIT_FAILURE);
  }  

  if (verbose) { 
    log_output("opening database: %s", database);
  }
  db = openLog(database);

  // allow manual additions
  if ( (id) && (name) ) {
    logDevice(db, user, pass, id, name);
    log_output("added id:%s name:%s", id, name);
    exit (EXIT_SUCCESS);    
  }

  g_log_set_always_fatal (G_LOG_LEVEL_WARNING);

  g_thread_init (NULL);
  g_type_init();
  mainloop = g_main_loop_new(NULL, FALSE);  
  if (mainloop == NULL) {
    g_printerr("Couldn't create GMainLoop\n");
    exit(EXIT_FAILURE);
  }

  bus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
  if (error != NULL)
  {
    g_printerr("Connecting to system bus failed: %s\n", error->message);
    g_error_free(error);
    exit(EXIT_FAILURE);
  }

  // create btloggerObject
  bobj = setupService(bus, db, user, pass, verbose);
  if (bobj == NULL) {
    exit(EXIT_FAILURE);  
  }

  // wait scan_freq seconds then call findDevices
  g_timeout_add((scan_freq * 1000), (GSourceFunc)findDevices, (gpointer)bobj->dbusObject);

  g_main_loop_run(mainloop);
  
  return EXIT_FAILURE;  
  
}

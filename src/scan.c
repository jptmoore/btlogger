// License: GPLv3
// Copyright 2009 John P. T. Moore
// jmoore@zedstar.org

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sqlite3.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <glib.h>
#include <libsoup/soup.h>
#include "scan.h"
#include "log.h"
#include "marshal.h"
#include "tweet.h"
#include "config.h"


#ifndef DBUS_TYPE_G_DICTIONARY
#define DBUS_TYPE_G_DICTIONARY \
  (dbus_g_type_get_map("GHashTable", G_TYPE_STRING, G_TYPE_VALUE))
#endif

#define log_output(fmtstr, args...) \
  (g_print(PROGNAME ":%s: " fmtstr "\n", __func__, ##args))

// code from test/agent.c from bluez source
static char *get_default_adapter_path(DBusConnection *conn)
{
  DBusMessage *msg, *reply;
  DBusError err;
  const char *reply_path;
  char *path;

  msg = dbus_message_new_method_call("org.bluez", "/",
          "org.bluez.Manager", "DefaultAdapter");

  if (!msg) {
    fprintf(stderr, "Can't allocate new method call\n");
    return NULL;
  }

  dbus_error_init(&err);

  reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);

  dbus_message_unref(msg);

  if (!reply) {
    fprintf(stderr,
      "Can't get default adapter\n");
    if (dbus_error_is_set(&err)) {
      fprintf(stderr, "%s\n", err.message);
      dbus_error_free(&err);
    }
    return NULL;
  }

  if (!dbus_message_get_args(reply, &err,
          DBUS_TYPE_OBJECT_PATH, &reply_path,
          DBUS_TYPE_INVALID)) {
    fprintf(stderr,
      "Can't get reply arguments\n");
    if (dbus_error_is_set(&err)) {
      fprintf(stderr, "%s\n", err.message);
      dbus_error_free(&err);
    }
    return NULL;
  }

  path = strdup(reply_path);

  dbus_message_unref(reply);

  dbus_connection_flush(conn);

  return path;
}

static void device_found(DBusGProxy *pobject, const char *address,
          GHashTable *hash, gpointer user_data)
{
  GValue *value;
  const gchar *name;
  btloggerObject *bobj = (btloggerObject *)user_data;

  if (hash != NULL) {
    value = g_hash_table_lookup(hash, "Name");
    name = value ? g_value_get_string(value) : NULL;
    if (bobj->verbose) { 
      log_output("address: %s name: %s", address, name);
    }
    if (name != NULL) {
      logDevice(bobj->dbHandle, 
		bobj->twitter, 
		(char *)address, 
		(char *)name);
    }
  }

}
 

btloggerObject *setupService( DBusGConnection *connection, 
                              sqlite3 *db, 
			      RestProxy *twitter,
                              gboolean verbose )
{

  btloggerObject *bobj;
  DBusGProxy *proxy;
  gchar *adapter;
    
  // call some code I found in test/agent.c of bluez source
  adapter = get_default_adapter_path(dbus_g_connection_get_connection(connection));

  proxy = dbus_g_proxy_new_for_name(connection, "org.bluez",
                                          adapter, "org.bluez.Adapter");
  if (proxy == NULL) {
    g_free(adapter);
    dbus_g_connection_unref(connection);
    return NULL;
  }

  g_free(adapter);
   
  if ((bobj = (btloggerObject *)g_malloc(sizeof(btloggerObject))) == NULL) {
    g_printerr("failed to malloc btloggerObject!");
    exit(EXIT_FAILURE);
  }

  bobj->dbHandle = db;
  bobj->verbose = verbose;
  bobj->dbusObject = proxy;
  bobj->twitter = twitter;

  // need to setup marshaller for devices found
  dbus_g_object_register_marshaller(marshal_VOID__STRING_BOXED,
            G_TYPE_NONE, G_TYPE_STRING,
            G_TYPE_VALUE, G_TYPE_INVALID);

  dbus_g_proxy_add_signal(proxy, "DeviceFound",
      G_TYPE_STRING, DBUS_TYPE_G_DICTIONARY, G_TYPE_INVALID);
  dbus_g_proxy_connect_signal(proxy, "DeviceFound",
        G_CALLBACK(device_found), bobj, NULL);
  
  // btloggerObject
  return bobj;  
}

void cleanupService(btloggerObject *bobj)
{
  closeLog(bobj->dbHandle);
  g_free(bobj);    
}  

gboolean findDevices(DBusGProxy *obj)
{
  GError *error = NULL;
     
  dbus_g_proxy_call(obj, "StartDiscovery", &error, G_TYPE_INVALID, G_TYPE_INVALID);
  if (error != NULL)
  {
    g_printerr("Failed to discover devices: %s\n", error->message);
    g_error_free(error);
  }
      
  return TRUE;
}

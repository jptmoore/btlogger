#ifndef SCAN_H_
#define SCAN_H_

#include <rest/oauth-proxy.h>

typedef struct {
  sqlite3 *dbHandle;
  RestProxy *twitter;
  gint verbose;
  DBusGProxy *dbusObject;
} btloggerObject;

btloggerObject *setupService( DBusGConnection *connection, 
                              sqlite3 *db, 
			      RestProxy *twitter,
                              gboolean verbose );
                              
void cleanupService(btloggerObject *bobj);

gboolean findDevices(DBusGProxy *obj);

#endif

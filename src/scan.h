#ifndef SCAN_H_
#define SCAN_H_

typedef struct {
  sqlite3 *dbHandle;
  gchar *user;
  gchar *pass;
  gint verbose;
  DBusGProxy *dbusObject;
} btloggerObject;

btloggerObject *setupService( DBusGConnection *connection, 
                              sqlite3 *db, 
                              gchar *user, 
                              gchar *pass, 
                              gboolean verbose );
                              
void cleanupService(btloggerObject *bobj);

gboolean findDevices(DBusGProxy *obj);

#endif

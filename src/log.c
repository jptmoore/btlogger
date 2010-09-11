// License: GPLv3
// Copyright 2010 John P. T. Moore
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

#ifdef DEBUG_MODE
#define dbg(fmtstr, args...) \
  (g_print(PROGNAME ":%s: " fmtstr "\n", __func__, ##args))
#else
#define dbg(dummy...)
#endif



sqlite3 *openLog(char *file)
{
  int rc;
  sqlite3 *db;

  rc = sqlite3_open(file, &db);
  if( rc ) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(1);
  }

  return db;
}


void closeLog(sqlite3 *db)
{
  sqlite3_close(db);
}


int logDevice(sqlite3 *db, RestProxy *twitter, char *mac, char *name)
{
  int rc; // result code
  int id; // primary key
  char *sql;
  sqlite3_stmt *stmt;
  struct timeval t_now;
  time_t seen;
  char *zErrMsg = 0;

  gettimeofday(&t_now, NULL);
  seen = t_now.tv_sec;

  sql = sqlite3_mprintf("select id from log where mac = '%s'", mac);
  rc = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
  sqlite3_free(sql);
  if(rc!=SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return EXIT_FAILURE;
  }
  
  rc = sqlite3_step(stmt);
  switch(rc) {
    case SQLITE_ROW:
    case SQLITE_DONE: 
    case SQLITE_OK:
      break;
    default:
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return EXIT_FAILURE;
  }
     
  id = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  // if no record exists
  if (id == 0) {     
    sql = sqlite3_mprintf("insert into log (mac, name, seen) values ('%s', \"%s\", %d)", mac, name, seen);
    // post to Twitter if proxy set
    if (twitter) {
      gchar *message;

      message = g_strdup_printf ("saw %s with id %s #%s", 
				 (name) ? name : "someone who didn't set their device name", mac, PROGNAME);
      tweet(twitter, message);
      g_free(message);
    }
  }
  else {
    sql = sqlite3_mprintf("update log set name = \"%s\", seen = %d where id = %d", name, seen, id);
  }
  dbg("%s", sql);
  rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
  sqlite3_free(sql);
  if( rc!=SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}

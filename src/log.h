#ifndef LOG_H_
#define LOG_H_

sqlite3 *openLog(char *file);
void closeLog(sqlite3 *db);
int logDevice(sqlite3 *db, RestProxy *twitter, char *mac, char *name);

#endif

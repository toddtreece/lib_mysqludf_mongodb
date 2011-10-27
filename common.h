#ifndef MYSQLUDF_COMMON_H
#define MYSQLUDF_COMMON_H

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include <assert.h>

typedef unsigned long long ulonglong;
typedef long long longlong;

#include <mysql.h>

#include "mongo.h"
#include "utf8.h"

#define VERSION_STRING "1.0"
#define VERSION_STRING_LENGTH 3
#define ERRMSG_SIZE 1000 

extern pthread_mutex_t mongodb_mutex;
extern mongo mongodb_connection[1];
extern int init;

#endif /* MYSQLUDF_COMMON_H */

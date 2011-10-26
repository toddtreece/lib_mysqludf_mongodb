#ifndef MYSQLUDF_COMMON_H
#define MYSQLUDF_COMMON_H
#define DLLEXP

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include <assert.h>

typedef unsigned long long ulonglong;
typedef long long longlong;

#include <mysql.h>
//#include <m_ctype.h>
//#include <m_string.h>

#include "mongo.h"
#include "utf8.h"

#define VERSION_STRING "1.0"
#define VERSION_STRING_LENGTH 3
#define ERRMSG_SIZE 1000 

extern mongo mongodb_connection[1];
#endif /* MYSQLUDF_COMMON_H */

#ifndef MYSQLUDF_COMMON_H
#define MYSQLUDF_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>

typedef unsigned long long ulonglong;
typedef long long longlong;

#include <mysql.h>

#include "mongo.h"
#include "encodings.h"
#include "server.h"
#include "io.h"

#define VERSION_STRING "1.0"
#define VERSION_STRING_LENGTH 3
#define ERRMSG_SIZE 1000

#define NUMBER_OF_THREADS 20
#define LOG_PATH "/home/todd/lib_mysqludf_mongodb.log"

typedef struct mongodb_connection_type {
  mongo connection[1];
  int init;
} mongodb_connection_type;

extern FILE *log_file;
extern mongodb_connection_type *mdb;
extern pool_type *connection_pool;

#endif /* MYSQLUDF_COMMON_H */

#ifndef MYSQLUDF_COMMON_H
#define MYSQLUDF_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

typedef unsigned long long ulonglong;
typedef long long longlong;

#include <mysql.h>

#include "mongo.h"
#include "encodings.h"

#define VERSION_STRING "1.0"
#define VERSION_STRING_LENGTH 3
#define ERRMSG_SIZE 1000

#define NUMBER_OF_THREADS 50
#define LOG_PATH "/home/todd/lib_mysqludf_mongodb.log"

// TYPE DEFINITIONS
typedef struct job_type {
  UDF_ARGS          *args;
  struct job_type   *next;
  struct job_type   *prev;
} job_type;

typedef struct queue_type {
  int       count;
  job_type  *start;
  job_type  *end;
  sem_t     *semaphore;
} queue_type;

typedef struct pool_type {
  int         count;
  pthread_t   *threads;
  queue_type  *queue;
} pool_type;

typedef struct thread_type {                            
  pthread_mutex_t   *connection_mutex;
  pool_type         *pool;
} thread_type;

typedef struct mongodb_connection_type {
  mongo connection[1];
  int init;
} mongodb_connection_type;

extern FILE *log_file;
extern mongodb_connection_type *mdb;
extern pool_type *connection_pool;

#endif /* MYSQLUDF_COMMON_H */

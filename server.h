#ifndef MYSQLUDF_SERVER_H

#define MYSQLUDF_SERVER_H

#include <pthread.h>
#include <semaphore.h>
#include "common.h"

FILE *log_file;

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



// INIT FUNCTIONS
pool_type* pool_init(int thread_count);

int queue_init(pool_type* pool);

my_bool mongodb_connect_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

my_bool mongodb_disconnect_init(UDF_INIT *initid, UDF_ARGS *args, char *message);


// CONNECTION FUNCTIONS
void connect(UDF_ARGS *args);

void insert(UDF_ARGS *args);

void disconnect();


// UDF FUNCTIONS
long long mongodb_connect(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

long long mongodb_disconnect(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);


// QUEUE FUNCTIONS
void queue_push(pool_type* pool, job_type* job);

int queue_pop(pool_type* pool);

job_type* queue_last(pool_type* pool);

void queue_empty(pool_type* pool);


// POOL FUNCTIONS
int pool_push(pool_type *pool, UDF_ARGS* args);

void *pool_run(void *p);

void pool_destroy(pool_type* pool);


#endif /* MYSQLUDF_SERVER_H */

#ifndef MYSQLUDF_SERVER_H

#define MYSQLUDF_SERVER_H

#include "common.h"

// INIT FUNCTIONS
pool_type* pool_init(int thread_count);

int queue_init(pool_type* pool);

my_bool mongodb_connect_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

my_bool mongodb_disconnect_init(UDF_INIT *initid, UDF_ARGS *args, char *message);


// CONNECTION FUNCTIONS
void mdb_connect(UDF_ARGS *args);

void mdb_insert(UDF_ARGS *args);

void mdb_disconnect();


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

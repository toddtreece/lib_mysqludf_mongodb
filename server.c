#include "server.h"

FILE *log_file;
static int pool_keepalive = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t iconv_mutex = PTHREAD_MUTEX_INITIALIZER;
mongodb_connection_type *mdb;
pool_type *connection_pool;

// INIT FUNCTIONS
pool_type* pool_init(int thread_count){
  
  int t;
  pool_type* pool;

  if(!thread_count || thread_count < 1) thread_count = 1;

  pool = (pool_type*)malloc(sizeof(pool_type));
  
  if(pool == NULL) {
    fprintf(stderr, "Could not allocate memory for connection pool\n");
    return NULL;
  }

  pool->threads = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
  
  if(pool->threads == NULL){
    fprintf(stderr, "Could not allocate memory for thread IDs\n");
    return NULL;
  }

  pool->count = thread_count;

  if (queue_init(pool) == -1){
    fprintf(stderr, "Could not allocate memory for queue\n");
    return NULL;
  }

  pool->queue->semaphore = (sem_t*)malloc(sizeof(sem_t));

  sem_init(pool->queue->semaphore, 0, 0);
  
  for (t = 0; t < thread_count; t++){
    fprintf(stderr, "Created thread %d in pool \n", t);
    pthread_create(&(pool->threads[t]), NULL, pool_run, (void *)pool);
  }

  return pool;

}

int queue_init(pool_type *pool){

  pool->queue = (queue_type*)malloc(sizeof(queue_type));
  
  if (pool->queue == NULL) return -1;

  pool->queue->end = NULL;
  pool->queue->start = NULL;
  pool->queue->count = 0;

  return 0;

}

my_bool mongodb_connect_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  
  if(args->arg_count != 2) {

    strncpy(message, "Requires server IP and port.", MYSQL_ERRMSG_SIZE);

    return 1;

  }

  args->arg_type[0]= STRING_RESULT;
  args->arg_type[1]= INT_RESULT;

  initid->ptr= NULL;

  return 0;

}

my_bool mongodb_disconnect_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
   
  if(args->arg_count > 0) {
    strncpy(message, "This function requires no arguments: mongodb_disconnect()", MYSQL_ERRMSG_SIZE);
    return 1;
  }

  initid->ptr = NULL;

  return 0;

}



// CONNECTION FUNCTIONS
void mdb_connect(UDF_ARGS *args) {

  int i,status;

  connection_pool = pool_init(NUMBER_OF_THREADS);

  mdb = malloc(sizeof(mongodb_connection_type) * NUMBER_OF_THREADS);

  for(i=0; i < NUMBER_OF_THREADS; i++) {

    mdb[i].init = 1;
    
    mongo_init(mdb[i].connection);
    
    status = mongo_connect(mdb[i].connection, args->args[0], *((long long*)args->args[1]));
    
    if(status != MONGO_OK) {

      switch(mdb[i].connection->err) {
        
        MONGO_CONN_NO_SOCKET:
          fprintf(stderr, "Could not create a mongodb socket.\n");
          break;

        MONGO_CONN_FAIL:
          fprintf(stderr, "An error occured while attempting to connect to mongodb.\n");
          break;
        
        MONGO_CONN_ADDR_FAIL:
          fprintf(stderr, "Could not get address info from mongodb.\n");
          break;
        
        MONGO_CONN_NOT_MASTER:
          fprintf(stderr, "Warning: connected to a non-master node (read-only).\n");
          break;

      }
    
    } else {
      fprintf(stderr, "Created mongodb connection %d.\n", i);
    }

  }

}

void mdb_insert(UDF_ARGS *args) {

  int i, thread = 0;

  for(i = 0; i < NUMBER_OF_THREADS; i++) {
    
    if(pthread_equal(connection_pool->threads[i],pthread_self()))
      thread = i;
    
  }

  if(mdb[thread].init == 0)
    fprintf(stderr, "mongodb connection was not initialized.\n");

  if(!mdb[thread].connection->connected)
    mongo_reconnect(mdb[thread].connection);

  bson b[1];

  // the first argument is reserved
  // for the collection name, so start
  // the loop at 1.
  i = 1;

  bson_init(b);

  bson_append_new_oid(b, "_id");

  while (i < args->arg_count) {

    if (args->args[i]) {

      if (args->arg_type[i] == STRING_RESULT || args->arg_type[i] == DECIMAL_RESULT) {

        if(args->lengths[i] < strlen(args->args[i])) {

          args->args[i][args->lengths[i]] = '\0';

        }

        if(args->lengths[i] == 0 || strlen(args->args[i]) == 0 || args->args[i] == NULL) {

          bson_append_null(b, args->attributes[i]);

        } else {
          
          char *item = NULL;

          item = utf8_encode(args->args[i]);
          
          bson_append_string(b, args->attributes[i], item);

          free(item);

        }

      } else if (args->arg_type[i] == INT_RESULT) {

        if(args->args[i] == NULL) {

          bson_append_null(b, args->attributes[i]);

        } else {

          bson_append_int(b, args->attributes[i],*((long long*) args->args[i]));

        }
      
      } else if (args->arg_type[i] == REAL_RESULT) {

        if(args->args[i] == NULL) {

          bson_append_null(b, args->attributes[i]);

        } else {

          bson_append_double(b, args->attributes[i],*((double*) args->args[i]));

        }
      
      }

    }
    
    i++;

  }

  bson_finish(b);

  if(mongo_insert(mdb[thread].connection, args->args[0], b) == MONGO_ERROR && mdb[thread].connection->err == MONGO_IO_ERROR) {
    
    fprintf(stderr, "saving data to mongodb failed.\n");

  }

  bson_destroy(b);

}

void mdb_disconnect() {

  int i;

  for(i=0; i < NUMBER_OF_THREADS; i++) {

    if(mdb[i].connection->connected) {

      mongo_destroy(mdb[i].connection);

      fprintf(stderr, "Disconnected connection %d from mongodb.\n", i);

    }

  }

}



// UDF FUNCTIONS
long long mongodb_connect(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error) {
  
  mdb_connect(args);

  *is_null = 1;
  return 0;

}

long long mongodb_disconnect(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error) {

  mdb_disconnect();

  *is_null = 1;
  return 0;

}



// QUEUE FUNCTIONS
void queue_push(pool_type *pool, job_type *job) { 

  job->next = NULL;
  job->prev = NULL;

  job_type *old_job;
  old_job = pool->queue->start;

  switch(pool->queue->count) {

    case 0:
      pool->queue->end = job;
      pool->queue->start = job;
      break;

    default:
      old_job->prev = job;
      job->next = old_job;
      pool->queue->start = job;

  }

  (pool->queue->count)++;
  sem_post(pool->queue->semaphore);

  int sval;
  sem_getvalue(pool->queue->semaphore, &sval);

}

int queue_pop(pool_type *pool) {
  job_type *last_job;
  last_job = pool->queue->end;

  switch(pool->queue->count) {

    case 0:
      return -1;
      break;

    case 1:
      pool->queue->end = NULL;
      pool->queue->start = NULL;
      break;

    default:
      last_job->prev->next = NULL;
      pool->queue->end = last_job->prev;

  }

  pool->queue->count--;

  int sval;
  sem_getvalue(pool->queue->semaphore, &sval);

  return 0;
}

job_type* queue_last(pool_type *pool) {
  return pool->queue->end;
}

void queue_empty(pool_type *pool) {

  job_type *job;
  job = pool->queue->end;

  while(pool->queue->count) {

    pool->queue->end = job->prev;
    free(job);

    job = pool->queue->end;
    pool->queue->count--;

  }

  pool->queue->end = NULL;
  pool->queue->start = NULL;

}



// POOL FUNCTIONS
void *pool_run(void *p) {

  pool_type *pool = (pool_type*)p;

  while(pool_keepalive) {

    if(sem_wait(pool->queue->semaphore)) {
      fprintf(stderr,"Waiting for semaphore. \n");
      exit(1);
    }

    if(pool_keepalive) {

      UDF_ARGS *arg_buff;
      job_type *job;

      pthread_mutex_lock(&mutex);

        job = queue_last(pool);

        arg_buff = job->args;

        queue_pop(pool);

      pthread_mutex_unlock(&mutex);
        
      mdb_insert(arg_buff);      

      free(job);

    } else {

      pthread_exit(NULL);

    }

  }

  pthread_exit(NULL);
  
}

int pool_push(pool_type *pool, UDF_ARGS *args){

  job_type *job;

  job = (job_type*)malloc(sizeof(job_type));
  
  if(job == NULL) {
    fprintf(stderr, "Could not allocate memory for new job.\n");
    exit(1);
  }

  job->args = args;

  pthread_mutex_lock(&mutex);
  
    queue_push(pool, job);
  
  pthread_mutex_unlock(&mutex);
  
  return 0;

}

void pool_destroy(pool_type *pool){
  
  int t;

  pool_keepalive = 0; 

  for(t=0; t < (pool->count); t++) {

    if(sem_post(pool->queue->semaphore))
      fprintf(stderr, "Could not bypass sem_wait().\n");

  }

  if(sem_destroy(pool->queue->semaphore) != 0)

    fprintf(stderr, "Could not destroy semaphore.\n");

  for(t=0; t < (pool->count); t++)
    pthread_join(pool->threads[t], NULL);

  queue_empty(pool);

  free(pool->threads);
  free(pool->queue->semaphore);
  free(pool->queue);
  free(pool);

}

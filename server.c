#include "common.h"

pthread_mutex_t mongodb_mutex;

mongo mongodb_connection[1];

int init = 0;

my_bool mongodb_connect_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long mongodb_connect(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

my_bool mongodb_disconnect_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long mongodb_disconnect(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

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

long long mongodb_connect(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error) {
  
  int rc;
  
  mongo_init(mongodb_connection);
  
  pthread_mutex_init(&mongodb_mutex, NULL);

  init = 1;

  pthread_mutex_lock(&mongodb_mutex);
  
  rc = mongo_connect(mongodb_connection, args->args[0], *((long long*)args->args[1]));
  
  if(rc != MONGO_OK) {

    *error = 1;

    switch(mongodb_connection->err) {
      
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
  
  }

  pthread_mutex_unlock(&mongodb_mutex);

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

long long mongodb_disconnect(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error) {

  if(init == 0) {
    fprintf(stderr, "mongodb connection was not initialized.\n");
    *error = 1;
    return 0;
  }
  
  pthread_mutex_lock(&mongodb_mutex);
  
  if(mongodb_connection->connected) {

    mongo_destroy(mongodb_connection);

    fprintf(stderr, "Disconnected from mongodb.\n");

  }

  pthread_mutex_unlock(&mongodb_mutex);

  pthread_mutex_destroy(&mongodb_mutex);

  *is_null = 1;
  return 0;

}


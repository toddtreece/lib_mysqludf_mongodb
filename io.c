#include "common.h"

my_bool mongodb_save_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long mongodb_save(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

my_bool mongodb_save_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  
  if(args->arg_count < 2) {

    strncpy(message, "You must provide a collection name and at least one field to save.", MYSQL_ERRMSG_SIZE);

    return 1;

  }

  initid->ptr = NULL;

  return 0;

}

long long mongodb_save(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error) {
  
  if(init == 0) {
    fprintf(stderr, "mongodb connection was not initialized.\n");
    *error = 1;
    return 0;
  }

  pthread_mutex_lock(&mongodb_mutex);

  if(!mongodb_connection->connected) {

    // Attempt to reconnect
    mongo_reconnect(mongodb_connection);

  }

  pthread_mutex_unlock(&mongodb_mutex);
  
  
  bson b[1];

  // the first argument is reserved
  // for the collection name, so start
  // the loop at 1.
  int i = 1;

  bson_init(b);

  bson_append_new_oid(b, "_id");

  while (i < args->arg_count) {

    if (args->args[i]) {

      if (args->arg_type[i] == STRING_RESULT || args->arg_type[i] == DECIMAL_RESULT) {
        
        if(args->lengths[i] < strlen(args->args[i])) {

          args->args[i][args->lengths[i]] = '\0';

        }

        if(strcmp(args->args[i], "NULL") == 0 || args->lengths[i] == 0) {

          bson_append_null(b, args->attributes[i]);

        } else {

          char *item;

          item = utf8_encode(args->args[i]);
          
          bson_append_string(b, args->attributes[i],item);

          free(item);

        }

      } else if (args->arg_type[i] == INT_RESULT) {

        if(args->args[i] == NULL) {

          bson_append_null(b, args->attributes[i]);

        } else {

          bson_append_int(b, args->attributes[i],*((long long*) args->args[i]));

        }
      
      }

    }
    
    i++;
  }

  bson_finish(b);

  pthread_mutex_lock(&mongodb_mutex);

  if(mongo_insert(mongodb_connection, args->args[0], b) == MONGO_ERROR && mongodb_connection->err == MONGO_IO_ERROR) {
    
    fprintf(stderr, "saving data to mongodb failed.\n");

    *error = 1;

  }

  pthread_mutex_unlock(&mongodb_mutex);

  bson_destroy(b);

  *is_null = 1;
  return 0;

}

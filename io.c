#include "io.h"

my_bool mongodb_save_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  
  if(args->arg_count < 2) {

    strncpy(message, "You must provide a collection name and at least one field to save.", MYSQL_ERRMSG_SIZE);

    return 1;

  }

  initid->ptr = NULL;

  return 0;

}

long long mongodb_save(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error) {
 
  pool_push(connection_pool, args);

  *is_null = 1;
  return 0;

}

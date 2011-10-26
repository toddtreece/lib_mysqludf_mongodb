#include "common.h"

my_bool mongodb_udf_version_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *mongodb_udf_version(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

my_bool mongodb_udf_version_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  
  char *version_string;
  
  if(args->arg_count > 0) {
    strncpy(message, "This function requires no arguments: mongodb_udf_version()", MYSQL_ERRMSG_SIZE);
    return 1;
  }

  version_string= calloc(1, VERSION_STRING_LENGTH + 1);
  strncpy(version_string, VERSION_STRING, VERSION_STRING_LENGTH);
  initid->ptr= version_string;

  return 0;

}

char *mongodb_udf_version(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error) {
  
  char *version_string= initid->ptr;
  *length= VERSION_STRING_LENGTH;
  
  return (version_string);

}


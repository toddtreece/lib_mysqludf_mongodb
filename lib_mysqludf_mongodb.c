
#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(WIN32)
#define DLLEXP __declspec(dllexport) 
#else
#define DLLEXP
#endif

#ifdef STANDARD
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong;
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>
#endif
#include <mysql.h>
#include <m_ctype.h>
#include <m_string.h>
#include <stdlib.h>

#include <ctype.h>
#include "mongo.h"

#ifdef HAVE_DLOPEN

mongo conn[1];

#ifdef	__cplusplus
extern "C" {
#endif
  DLLEXP my_bool lib_mysqludf_mongodb_info_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
  DLLEXP void lib_mysqludf_mongodb_info_deinit(UDF_INIT *initid);
  DLLEXP char *lib_mysqludf_mongodb_info(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

  DLLEXP my_bool mongodb_save_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
  DLLEXP void mongodb_save_deinit(UDF_INIT *initid);
  DLLEXP char *mongodb_save(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

  DLLEXP my_bool mongodb_connect_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
  DLLEXP void mongodb_connect_deinit(UDF_INIT *initid);
  DLLEXP char *mongodb_connect(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

  DLLEXP my_bool mongodb_disconnect_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
  DLLEXP void mongodb_disconnect_deinit(UDF_INIT *initid);
  DLLEXP char *mongodb_disconnect(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);
#ifdef	__cplusplus
}
#endif

my_bool lib_mysqludf_mongodb_info_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  return 0;
}

void lib_mysqludf_mongodb_info_deinit(UDF_INIT *initid)
{
}

char* lib_mysqludf_mongodb_info(UDF_INIT *initid, UDF_ARGS *args, char* result, unsigned long* length,	char *is_null, char *error)
{
  strcpy(result, "v1.0");
  *length = strlen("v1.0");
  return result;
}

my_bool mongodb_connect_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  return 0;
}

void mongodb_connect_deinit(UDF_INIT *initid)
{
}

char* mongodb_connect(UDF_INIT *initid, UDF_ARGS *args, char* result, unsigned long* length,	char *is_null, char *error)
{
  mongo_connect(conn, "127.0.0.1", 27017);
  strcpy(result, "connect");
  *length = strlen("connect");

  return result;
}

my_bool mongodb_disconnect_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  return 0;
}

void mongodb_disconnect_deinit(UDF_INIT *initid)
{
}

char* mongodb_disconnect(UDF_INIT *initid, UDF_ARGS *args, char* result, unsigned long* length,	char *is_null, char *error)
{
  mongo_destroy(conn);

  strcpy(result, "disconnect");
  *length = strlen("discconnect");

  return result;
}



my_bool mongodb_save_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  return 0;
}

void mongodb_save_deinit(UDF_INIT *initid)
{
}

char* mongodb_save(UDF_INIT *initid, UDF_ARGS *args, char* result, unsigned long* length, char *is_null, char *error)
{

  bson b[1];

  bson_init(b);
  bson_append_new_oid( b, "_id" );

  int i = 0;

  while (i < args->arg_count) {

    if (args->args[i]) {

      if (args->arg_type[i] == STRING_RESULT) {
        
        //args array is not null terminated by default
        args->args[i][args->lengths[i]] = '\0';
  
        //TODO figure out how to insert non-ASCII characters
        bson_append_string(b, args->attributes[i], args->args[i]);

      } else if (args->arg_type[i] == INT_RESULT) {

        bson_append_int(b, args->attributes[i], (int64_t)args->args[i]);

      }

    }
    
    i++;
  }

  bson_finish(b);

  mongo_insert(conn, "test.foo", b );

  bson_destroy( b );

  strcpy(result, args->args[1]);
  *length = strlen(args->args[1]); 
  
  return result;
}

#endif /* HAVE_DLOPEN */


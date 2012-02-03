#ifndef MYSQLUDF_IO_H
#define MYSQLUDF_IO_H

#include "common.h"
#include "server.h"

my_bool mongodb_save_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long mongodb_save(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

#endif /* MYSQLUDF_IO_H */

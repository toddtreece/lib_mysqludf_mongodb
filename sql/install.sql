-- run this only after you have compiled 
-- and installed lib_mysqludf_mongodb.so

USE mysql;
DROP FUNCTION IF EXISTS mongodb_udf_version;
DROP FUNCTION IF EXISTS mongodb_connect;
DROP FUNCTION IF EXISTS mongodb_disconnect;
DROP FUNCTION IF EXISTS mongodb_save;

CREATE FUNCTION mongodb_udf_version RETURNS STRING SONAME "lib_mysqludf_mongodb.so";
CREATE FUNCTION mongodb_connect RETURNS STRING SONAME "lib_mysqludf_mongodb.so";
CREATE FUNCTION mongodb_disconnect RETURNS STRING SONAME "lib_mysqludf_mongodb.so";
CREATE FUNCTION mongodb_save RETURNS STRING SONAME "lib_mysqludf_mongodb.so";

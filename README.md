# lib_mysqludf_mongodb

The goal of the project is to allow MySQL triggers to store data in MongoDB.  By tracking MySQL inserts/updates/deletes will not cause the MySQL database to grow, which will make backing up system critical data faster and easier.

**Disclaimer:** *I am not a C programmer, and this code is definitely not production ready.*

## Requirements
*  [MongoDB C Driver](http://www.mongodb.org/display/DOCS/C+Language+Center)

## Installation

### Tested on Ubuntu Server 10.04.3 LTS x86_64
* MongoDB version v1.2.2
* MySQL version 5.1.41-3ubuntu12.10
* gcc version 4.4.3
  

### Compiling lib_mysqludf_mongodb
* Use git to clone the Mongo C Driver repo into /usr/local/lib
* Complile the latest version of MongoDB C Diver using the instructions provided [here](http://api.mongodb.org/c/current/building.html).  * Do not use v0.4 of the driver as the instructions state. Compile using HEAD. *

- - -

    $ gcc --std=c99 -shared -Wall -fPIC \
      -I/usr/include/mysql \
      -I/usr/local/lib/mongo-c-driver/src \
      /usr/local/lib/mongo-c-driver/libmongoc.so \
      /usr/local/lib/mongo-c-driver/libbson.so \
      -o lib_mysqludf_mongodb.so  \
      lib_mysqludf_mongodb.c
    $ sudo cp lib_mysqludf_mongodb.so /usr/lib/mysql/plugin/lib_mysqludf_mongodb.so
    $ sudo service mysql restart

### Registering the UDF functions with MySQL
    USE mysql;
    DROP FUNCTION IF EXISTS lib_mysqludf_mongodb_info;
    DROP FUNCTION IF EXISTS mongodb_connect;
    DROP FUNCTION IF EXISTS mongodb_disconnect;
    DROP FUNCTION IF EXISTS mongodb_save;

    CREATE FUNCTION lib_mysqludf_mongodb_info RETURNS STRING SONAME "lib_mysqludf_mongodb.so";
    CREATE FUNCTION mongodb_connect RETURNS STRING SONAME "lib_mysqludf_mongodb.so";
    CREATE FUNCTION mongodb_disconnect RETURNS STRING SONAME "lib_mysqludf_mongodb.so";
    CREATE FUNCTION mongodb_save RETURNS STRING SONAME "lib_mysqludf_mongodb.so";

## Usage
    -- connect to mongoDB
    -- this will only need to be called once
    -- TODO accept arguments for different servers
    SELECT mongodb_connect();

    -- sample query
    SELECT mongodb_save(firstname, lastname) FROM customers ORDER BY id DESC LIMIT 0,10;

    -- mongodb insert from mysql trigger sample  --
    DELIMITER $$

    DROP TRIGGER IF EXISTS mongodb_update_trigger $$
     
    CREATE TRIGGER mongo_update_trigger BEFORE UPDATE ON users
      
      FOR EACH ROW BEGIN
      
        SELECT mongodb_save(
          NEW.id AS 'users_id',
          NEW.username AS 'username'
          NEW.firstname AS 'firstname', 
          NEW.lastname  AS 'lastname',
          NEW.email AS 'email',
          'update' AS 'history_type',
          NOW() AS 'timestamp'
        );
     
      END;

    $$

    DELIMITER ;
    -- end trigger sample --


    -- disconnect from mongoDB
    SELECT mongodb_disconnect();

# lib_mysqludf_mongodb

lib_mysqludf_mongodb
Copyright 2011 Todd Treece

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).

## Hunh?
The goal of the project is to allow MySQL triggers to store data in MongoDB.  By tracking MySQL changes (inserts/updates/deletes) in MongoDB, backing up system critical data will become faster and easier.

## Requirements
*  [MongoDB C Driver](http://www.mongodb.org/display/DOCS/C+Language+Center)
*  libmysql-dev

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
      /usr/lib/libmongoc.so \
      /usr/lib/libbson.so \
      server.c \
      io.c \
      version.c \
      encodings.c \
      -o lib_mysqludf_mongodb.so
    $ sudo cp lib_mysqludf_mongodb.so /usr/lib/mysql/plugin/lib_mysqludf_mongodb.so
    $ sudo service mysql restart

### Registering the UDF functions with MySQL
    USE mysql;
    DROP FUNCTION IF EXISTS mongodb_udf_version;
    DROP FUNCTION IF EXISTS mongodb_connect;
    DROP FUNCTION IF EXISTS mongodb_disconnect;
    DROP FUNCTION IF EXISTS mongodb_save;

    CREATE FUNCTION mongodb_udf_version RETURNS STRING SONAME "lib_mysqludf_mongodb.so";
    CREATE FUNCTION mongodb_connect RETURNS STRING SONAME "lib_mysqludf_mongodb.so";
    CREATE FUNCTION mongodb_disconnect RETURNS STRING SONAME "lib_mysqludf_mongodb.so";
    CREATE FUNCTION mongodb_save RETURNS STRING SONAME "lib_mysqludf_mongodb.so";

## Usage
    -- connecting to mongoDB:
    -- this will only need to be called once
    -- and can be placed in a mysql startup script.
    --
    -- in my.cnf add a line in the [mysqld] block
    -- that points to your startup script.
    -- i.e. init-file = /var/lib/mysql/init.sql

    SELECT mongodb_connect("127.0.0.1", 27017);

    -- sample query
    SELECT mongodb_save('test.customers' AS 'collection', firstname, lastname) FROM customers ORDER BY id DESC LIMIT 0,10;

    -- mongodb insert from mysql trigger sample  --
    DELIMITER $$

    DROP TRIGGER IF EXISTS mongodb_update_trigger $$
     
    CREATE TRIGGER mongodb_update_trigger BEFORE UPDATE ON users
      
      FOR EACH ROW BEGIN
      
        SET @save = (
          SELECT mongodb_save(
            'history.users' AS 'collection',
            NEW.id AS 'id',
            NEW.username AS 'username',
            NEW.firstname AS 'firstname', 
            NEW.lastname  AS 'lastname',
            NEW.email AS 'email',
            'update' AS 'history_type',
            NOW() AS 'timestamp'
          )
        );
     
      END;

    $$

    DELIMITER ;
    -- end trigger sample --


    -- disconnect from mongoDB
    SELECT mongodb_disconnect();

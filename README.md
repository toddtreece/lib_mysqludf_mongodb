# lib_mysqludf_mongodb

The goal of the project is to allow MySQL triggers to store data in MongoDB.  By tracking MySQL inserts/updates/deletes will not cause the MySQL database to grow, which will make backing up system critical data faster and easier.

**Disclaimer:** *I am not a C programmer, and this code is definitely not production ready.*

## Requirements
*  [MongoDB C Driver](http://www.mongodb.org/display/DOCS/C+Language+Center)

## Installation
*  `gcc --std=c99 -shared -Wall -fPIC -I/usr/include/mysql \
   -I/usr/local/lib/mongo-c-driver/src \
   /usr/local/lib/mongo-c-driver/libmongoc.so \
   /usr/local/lib/mongo-c-driver/libbson.so \
   -o lib_mysqludf_mongodb.so lib_mysqludf_mongodb.c`
*  `sudo cp lib_mysqludf_mongodb.so /usr/lib/mysql/plugin/lib_mysqludf_mongodb.so`
*  `sudo service mysql restart`

## Usage
`
-- connect to mongoDB
-- this will only need to be called once
-- TODO accept arguments for different servers
SELECT mongodb_connect();

-- sample query
SELECT mongodb_save(firstname, lastname) FROM customers ORDER BY id DESC LIMIT 0,10;

-- disconnect from mongoDB
SELECT mongodb_disconnect();
`


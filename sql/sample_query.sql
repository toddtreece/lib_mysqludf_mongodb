-- the first argument will always be the mongodb collection name
SELECT mongodb_save('test.customers' AS 'collection', firstname, lastname) FROM customers ORDER BY id DESC LIMIT 0,10;

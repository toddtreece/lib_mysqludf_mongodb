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

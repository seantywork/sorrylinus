CREATE USER 'seantywork'@'%' IDENTIFIED BY 'letsshareitwiththewholeuniverse';

GRANT ALL PRIVILEGES ON *.* TO 'seantywork'@'%';

CREATE DATABASE frank;

USE frank;

CREATE TABLE his_onlyfriends (email VARCHAR(255), f_session VARCHAR(500), p_key TEXT);

INSERT INTO his_onlyfriends(email, f_session, p_key) VALUES('seantywork@gmail.com', 'N', 'N');

COMMIT;


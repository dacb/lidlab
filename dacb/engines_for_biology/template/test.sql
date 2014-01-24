SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_template_test;
CREATE DATABASE ha_template_test;

USE ha_template_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin template;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN template SONAME 'ha_template.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	data BLOB
) ENGINE = template CONNECTION = "/data/tmp/test.template";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	data BLOB
) ENGINE = template;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	data BLOB
) ENGINE = template CONNECTION = "does not exist filename";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(*) AS '> count of records in test <' FROM test;

SELECT 'inserting to test' AS '> task <';
INSERT INTO test (data) VALUES ('insert1'), ('insert2');

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN template;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_template_test;

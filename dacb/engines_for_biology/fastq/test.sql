SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_fastq_test;
CREATE DATABASE ha_fastq_test;

USE ha_fastq_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin fastq;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN fastq SONAME 'ha_fastq.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	id VARCHAR(32),
	sequence VARCHAR(75),
	quality VARCHAR(75)
) ENGINE = fastq CONNECTION = "/data/tmp/test.fastq";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	id VARCHAR(32),
	sequence VARCHAR(75),
	quality VARCHAR(75)
) ENGINE = fastq;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	id VARCHAR(32),
	sequence VARCHAR(75),
	quality VARCHAR(75)
) ENGINE = fastq CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with unsupported field type)' AS '> task <';
CREATE TABLE fail (
	id VARCHAR(32),
	sequence VARCHAR(75),
	quality VARCHAR(75),
	int_not_supported INT
) ENGINE = fastq CONNECTION = "/data/tmp/test.fastq";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	id BLOB,
	sequence VARCHAR(75),
	quality VARCHAR(75)
) ENGINE = fastq CONNECTION = "/data/tmp/test.fastq";

SELECT 'creating table fail (should fail with missing field "id")' AS '> task <';
CREATE TABLE fail (
	sequence VARCHAR(75),
	quality VARCHAR(75)
) ENGINE = fastq CONNECTION = "/data/tmp/test.fastq";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(id) AS '> count of records in test <' FROM test;

#SELECT 'selecting just id field from test' AS '> task <';
SELECT id, LENGTH(sequence), LENGTH(quality) FROM test;

SELECT 'selecting from test where id = "SOLEXA-1GA-1:4:1:35:76#0/1"' AS '> task <';
SELECT * FROM test WHERE id = "SOLEXA-1GA-1:4:1:35:76#0/1";

SELECT 'inserting to test' AS '> task <';
INSERT INTO test (id, sequence, quality) VALUES ('insert1', 'ATGC', '____'), ('insert2', 'CGTA', ';;;;');
SELECT COUNT(id) AS '> count of records in test <' FROM test;

SELECT 'selecting from test where id LIKE "insert%"' AS '> task <';
SELECT * FROM test WHERE id LIKE "insert%";

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN fastq;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_fastq_test;

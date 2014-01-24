SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_fasta_test;
CREATE DATABASE ha_fasta_test;

USE ha_fasta_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin fasta;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN fasta SONAME 'ha_fasta.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	id VARCHAR(16),
	comment VARCHAR(32),	# this is too small so we can test truncation
#	comment BLOB,	# probably the best way to do this
	sequence BLOB
) ENGINE = fasta CONNECTION = "/data/tmp/test.fasta";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	id VARCHAR(16),
	comment BLOB,
	sequence BLOB
) ENGINE = fasta;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	id VARCHAR(16),
	comment BLOB,
	sequence BLOB
) ENGINE = fasta CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with unsupported field type)' AS '> task <';
CREATE TABLE fail (
	id VARCHAR(16),
	int_not_supported INT,
	comment BLOB,
	sequence BLOB
) ENGINE = fasta CONNECTION = "/data/tmp/test.fasta";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	id BLOB,
	comment BLOB,
	sequence BLOB
) ENGINE = fasta CONNECTION = "/data/tmp/test.fasta";

SELECT 'creating table fail (should fail with missing field "id")' AS '> task <';
CREATE TABLE fail (
	comment BLOB,
	sequence BLOB
) ENGINE = fasta CONNECTION = "/data/tmp/test.fasta";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(id) AS '> count of records in test <' FROM test;

#SELECT 'selecting just id field from test' AS '> task <';
#SELECT id FROM test;

SELECT 'selecting from test where id = "2006680944"' AS '> task <';
SELECT * FROM test WHERE id = "2006680944";

SELECT 'inserting to test' AS '> task <';
INSERT INTO test (id, comment, sequence) VALUES ('insert1', 'first inserted sequence', 'ATGC'), ('insert2', 'second inserted sequence', 'GATTACA');
SELECT COUNT(id) AS '> count of records in test <' FROM test;

SELECT 'selecting from test where id LIKE "insert%"' AS '> task <';
SELECT * FROM test WHERE id LIKE "insert%";

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN fasta;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_fasta_test;

SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_blast_fmt6_test;
CREATE DATABASE ha_blast_fmt6_test;

USE ha_blast_fmt6_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin blast_fmt6;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN blast_fmt6 SONAME 'ha_blast_fmt6.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	query_id VARCHAR(32),
	subject_id VARCHAR(32),
	percent_identity FLOAT,
	alignment_length INT,
	mismatches INT,
	gap_opens INT,
	query_start INT,
	query_end INT,
	subject_start INT,
	subject_end INT,
	evalue DOUBLE,
	bit_score FLOAT
) ENGINE = blast_fmt6 CONNECTION = "/data/tmp/test.blast_fmt6";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	query_id VARCHAR(32),
	subject_id VARCHAR(32),
	percent_identity FLOAT,
	alignment_length INT,
	mismatches INT,
	gap_opens INT,
	query_start INT,
	query_end INT,
	subject_start INT,
	subject_end INT,
	evalue DOUBLE,
	bit_score FLOAT
) ENGINE = blast_fmt6;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	query_id VARCHAR(32),
	subject_id VARCHAR(32),
	percent_identity FLOAT,
	alignment_length INT,
	mismatches INT,
	gap_opens INT,
	query_start INT,
	query_end INT,
	subject_start INT,
	subject_end INT,
	evalue DOUBLE,
	bit_score FLOAT
) ENGINE = blast_fmt6 CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	query_id INT,
	subject_id VARCHAR(32),
	percent_identity FLOAT,
	alignment_length INT,
	mismatches INT,
	gap_opens INT,
	query_start INT,
	query_end INT,
	subject_start INT,
	subject_end INT,
	evalue DOUBLE,
	bit_score FLOAT
) ENGINE = blast_fmt6 CONNECTION = "/data/tmp/test.blast_fmt6";

SELECT 'creating table fail (should fail with missing field "query_id")' AS '> task <';
CREATE TABLE fail (
	foo INT
) ENGINE = blast_fmt6 CONNECTION = "/data/tmp/test.blast_fmt6";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(query_id) AS '> count of records in test <' FROM test;

SELECT 'inserting rows into test' AS '> task <';
INSERT INTO test (query_id, subject_id, percent_identity, alignment_length, mismatches, gap_opens, query_start, query_end, subject_start, subject_end, evalue, bit_score) VALUES ('qid', 'sid', 99.99, 100, 1, 2, 1, 100, 1001, 1101, 1e-10, 42.0);

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN blast_fmt6;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_blast_fmt6_test;

-- CREATE TABLE procedure, takes target table name as argument
DROP PROCEDURE IF EXISTS blast_create_table;

DELIMITER $$

CREATE PROCEDURE blast_create_table(IN BLAST_table VARCHAR(60))
LANGUAGE SQL
DETERMINISTIC
MODIFIES SQL DATA
SQL SECURITY INVOKER
COMMENT 'Creates table for BLAST results'
BEGIN
	SET @statement = CONCAT('CREATE TABLE ', BLAST_table, ' ( query_id varchar(40) NOT NULL, subject_id varchar(40) NOT NULL, percent_identity float NOT NULL, alignment_length int(11) NOT NULL, mismatches int(11) NOT NULL, gap_opens int(11) NOT NULL, query_start int(11) NOT NULL, query_end int(11) NOT NULL, subject_start int(11) NOT NULL, subject_end int(11) NOT NULL, evalue double NOT NULL, bit_score int(11) NOT NULL, rank INT DEFAULT NULL ) ');

	-- SELECT @statement;

	PREPARE stmt FROM @statement;

	EXECUTE stmt;

	DEALLOCATE PREPARE stmt;
END$$

DELIMITER ;

-- For a BLAST results table, this will find the best 'N' matches to each query sequence
DROP PROCEDURE IF EXISTS blast_best_matches;

DELIMITER $$
CREATE PROCEDURE blast_best_matches(IN BLAST_table VARCHAR(60), IN N INT, IN BLAST_best_match_table VARCHAR(60))
LANGUAGE SQL
DETERMINISTIC
READS SQL DATA
SQL SECURITY INVOKER
COMMENT 'Find best N results from BLAST table for each query'
BEGIN
	SET @statement = CONCAT('INSERT INTO ', BLAST_best_match_table, ' (query_id, subject_id, percent_identity, alignment_length, mismatches, gap_opens, query_start, query_end, subject_start, subject_end, evalue, bit_score, rank) SELECT * FROM ( SELECT vs.query_id, vs.subject_id, vs.percent_identity, vs.alignment_length, vs.mismatches, vs.gap_opens, vs.query_start, vs.query_end, vs.subject_start, vs.subject_end, vs.evalue, vs.bit_score, FIND_IN_SET(vs.percent_identity, x.percent_identity_list) RANK FROM ', BLAST_table, ' AS vs, ( SELECT query_id, GROUP_CONCAT(percent_identity ORDER BY percent_identity DESC) percent_identity_list FROM ', BLAST_table, ' GROUP BY query_id) AS x WHERE vs.query_id = x.query_id) AS z WHERE RANK <= ', N, ' AND RANK > 0 ORDER BY query_id, percent_identity;');

	-- SELECT @statement;

	PREPARE stmt FROM @statement;

	EXECUTE stmt;

	DEALLOCATE PREPARE stmt;
END$$

DELIMITER ;

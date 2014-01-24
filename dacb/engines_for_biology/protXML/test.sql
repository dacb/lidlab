SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_protXML_protein_group_test;
CREATE DATABASE ha_protXML_protein_group_test;

USE ha_protXML_protein_group_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin protXML_protein_group;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN protXML_protein_group SONAME 'ha_protXML_protein_group.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_protein_group CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_protein_group;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_protein_group CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	group_number INT,
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_protein_group CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with missing field "query_id")' AS '> task <';
CREATE TABLE fail (
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_protein_group CONNECTION = "/data/tmp/prot.xml";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(group_number) AS '> count of records in test <' FROM test;

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN protXML_protein_group;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_protXML_protein_group_test;


SELECT '---------------------------------------------------------------------' AS divide;
SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_protXML_protein_test;
CREATE DATABASE ha_protXML_protein_test;

USE ha_protXML_protein_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin protXML_protein;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN protXML_protein SONAME 'ha_protXML_protein.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	group_number VARCHAR(16),
	protein_name VARCHAR(16),
	n_indistinguishable_proteins INTEGER,
	probability FLOAT,
	percent_coverage FLOAT,
	unique_stripped_peptides LONGTEXT,
	group_sibling_id VARCHAR(4),
	total_number_peptides INTEGER,
	subsuming_protein_entry VARCHAR(16),
	pct_spectrum_ids FLOAT,
	confidence FLOAT,
	protein_description LONGTEXT,
	ipi_name VARCHAR(32),
	refseq_name VARCHAR(32),
	swissprot_name VARCHAR(32),
	ensembl_name VARCHAR(32),
	trembl_name VARCHAR(32),
	locus_link_name VARCHAR(32),
	flybase VARCHAR(32)
) ENGINE = protXML_protein CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	protein_name VARCHAR(16),
	n_indistinguishable_proteins INTEGER,
	probability FLOAT,
	percent_coverage FLOAT,
	unique_stripped_peptides LONGTEXT,
	group_sibling_id VARCHAR(4),
	total_number_peptides INTEGER,
	subsuming_protein_entry VARCHAR(16),
	pct_spectrum_ids FLOAT,
	confidence FLOAT,
	protein_description LONGTEXT,
	ipi_name VARCHAR(32),
	refseq_name VARCHAR(32),
	swissprot_name VARCHAR(32),
	ensembl_name VARCHAR(32),
	trembl_name VARCHAR(32),
	locus_link_name VARCHAR(32),
	flybase VARCHAR(32)
) ENGINE = protXML_protein;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	protein_name VARCHAR(16),
	n_indistinguishable_proteins INTEGER,
	probability FLOAT,
	percent_coverage FLOAT,
	unique_stripped_peptides LONGTEXT,
	group_sibling_id VARCHAR(4),
	total_number_peptides INTEGER,
	subsuming_protein_entry VARCHAR(16),
	pct_spectrum_ids FLOAT,
	confidence FLOAT,
	protein_description LONGTEXT,
	ipi_name VARCHAR(32),
	refseq_name VARCHAR(32),
	swissprot_name VARCHAR(32),
	ensembl_name VARCHAR(32),
	trembl_name VARCHAR(32),
	locus_link_name VARCHAR(32),
	flybase VARCHAR(32)
) ENGINE = protXML_protein CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	group_number INTEGER,
	protein_name VARCHAR(16),
	n_indistinguishable_proteins INTEGER,
	probability FLOAT,
	percent_coverage FLOAT,
	unique_stripped_peptides LONGTEXT,
	group_sibling_id VARCHAR(4),
	total_number_peptides INTEGER,
	subsuming_protein_entry VARCHAR(16),
	pct_spectrum_ids FLOAT,
	confidence FLOAT,
	protein_description LONGTEXT,
	ipi_name VARCHAR(32),
	refseq_name VARCHAR(32),
	swissprot_name VARCHAR(32),
	ensembl_name VARCHAR(32),
	trembl_name VARCHAR(32),
	locus_link_name VARCHAR(32),
	flybase VARCHAR(32)
) ENGINE = protXML_protein CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with missing field "group_number")' AS '> task <';
CREATE TABLE fail (
	protein_name VARCHAR(16),
	n_indistinguishable_proteins INTEGER,
	probability FLOAT,
	percent_coverage FLOAT,
	unique_stripped_peptides LONGTEXT,
	group_sibling_id VARCHAR(4),
	total_number_peptides INTEGER,
	subsuming_protein_entry VARCHAR(16),
	pct_spectrum_ids FLOAT,
	confidence FLOAT,
	protein_description LONGTEXT,
	ipi_name VARCHAR(32),
	refseq_name VARCHAR(32),
	swissprot_name VARCHAR(32),
	ensembl_name VARCHAR(32),
	trembl_name VARCHAR(32),
	locus_link_name VARCHAR(32),
	flybase VARCHAR(32)
) ENGINE = protXML_protein CONNECTION = "/data/tmp/prot.xml";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(group_number) AS '> count of records in test <' FROM test;

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN protXML_protein;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_protXML_protein_test;

SELECT '---------------------------------------------------------------------' AS divide;
SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_protXML_protein_parameter_test;
CREATE DATABASE ha_protXML_protein_parameter_test;

USE ha_protXML_protein_parameter_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin protXML_protein_parameter;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN protXML_protein_parameter SONAME 'ha_protXML_protein_parameter.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
) ENGINE = protXML_protein_parameter CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	protein_name VARCHAR(16),
	name VARCHAR(32),
	value LONGTEXT
) ENGINE = protXML_protein_parameter;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	protein_name VARCHAR(16),
	name VARCHAR(32),
	value LONGTEXT
) ENGINE = protXML_protein_parameter CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	group_number INTEGER,
	protein_name VARCHAR(16),
	name VARCHAR(32),
	value LONGTEXT
) ENGINE = protXML_protein_parameter CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with missing field "group_number")' AS '> task <';
CREATE TABLE fail (
	protein_name VARCHAR(16),
	name VARCHAR(32),
	value LONGTEXT
) ENGINE = protXML_protein_parameter CONNECTION = "/data/tmp/prot.xml";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(group_number) AS '> count of records in test <' FROM test;

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN protXML_protein_parameter;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_protXML_protein_parameter_test;

SELECT '---------------------------------------------------------------------' AS divide;
SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_protXML_indistinguishable_protein_test;
CREATE DATABASE ha_protXML_indistinguishable_protein_test;

USE ha_protXML_indistinguishable_protein_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin protXML_indistinguishable_protein;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN protXML_indistinguishable_protein SONAME 'ha_protXML_indistinguishable_protein.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	group_number VARCHAR(16),
	protein_name VARCHAR(16),
) ENGINE = protXML_indistinguishable_protein CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	protein_name VARCHAR(16),
) ENGINE = protXML_indistinguishable_protein;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	protein_name VARCHAR(16),
) ENGINE = protXML_indistinguishable_protein CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	group_number INT,
	protein_name VARCHAR(16),
) ENGINE = protXML_indistinguishable_protein CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with missing field "group_number")' AS '> task <';
CREATE TABLE fail (
	protein_name VARCHAR(16),
) ENGINE = protXML_indistinguishable_protein CONNECTION = "/data/tmp/prot.xml";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(group_number) AS '> count of records in test <' FROM test;

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN protXML_indistinguishable_protein;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_protXML_indistinguishable_protein_test;

SELECT '---------------------------------------------------------------------' AS divide;
SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_protXML_peptide_test;
CREATE DATABASE ha_protXML_peptide_test;

USE ha_protXML_peptide_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin protXML_peptide;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN protXML_peptide SONAME 'ha_protXML_peptide.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	group_number INT,
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with missing field "group_number")' AS '> task <';
CREATE TABLE fail (
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide CONNECTION = "/data/tmp/prot.xml";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(group_number) AS '> count of records in test <' FROM test;

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN protXML_peptide;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_protXML_peptide_test;

SELECT '---------------------------------------------------------------------' AS divide;
SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_protXML_peptide_parent_protein_test;
CREATE DATABASE ha_protXML_peptide_parent_protein_test;

USE ha_protXML_peptide_parent_protein_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin protXML_peptide_parent_protein;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN protXML_peptide_parent_protein SONAME 'ha_protXML_peptide_parent_protein.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide_parent_protein CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide_parent_protein;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide_parent_protein CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	group_number INT,
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide_parent_protein CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with missing field "group_number")' AS '> task <';
CREATE TABLE fail (
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide_parent_protein CONNECTION = "/data/tmp/prot.xml";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(group_number) AS '> count of records in test <' FROM test;

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN protXML_peptide_parent_protein;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_protXML_peptide_parent_protein_test;

SELECT '---------------------------------------------------------------------' AS divide;
SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_protXML_indistinguishable_peptide_test;
CREATE DATABASE ha_protXML_indistinguishable_peptide_test;

USE ha_protXML_indistinguishable_peptide_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin protXML_indistinguishable_peptide;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN protXML_indistinguishable_peptide SONAME 'ha_protXML_indistinguishable_peptide.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	group_number INT,
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with missing field "group_number")' AS '> task <';
CREATE TABLE fail (
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide CONNECTION = "/data/tmp/prot.xml";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(group_number) AS '> count of records in test <' FROM test;

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN protXML_indistinguishable_peptide;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_protXML_indistinguishable_peptide_test;

SELECT '---------------------------------------------------------------------' AS divide;
SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_protXML_peptide_parameter_test;
CREATE DATABASE ha_protXML_peptide_parameter_test;

USE ha_protXML_peptide_parameter_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin protXML_peptide_parameter;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN protXML_peptide_parameter SONAME 'ha_protXML_peptide_parameter.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide_parameter CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide_parameter;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide_parameter CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	group_number INT,
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide_parameter CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with missing field "group_number")' AS '> task <';
CREATE TABLE fail (
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_peptide_parameter CONNECTION = "/data/tmp/prot.xml";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(group_number) AS '> count of records in test <' FROM test;

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN protXML_peptide_parameter;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_protXML_peptide_parameter_test;

SELECT '---------------------------------------------------------------------' AS divide;
SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_protXML_modification_info_test;
CREATE DATABASE ha_protXML_modification_info_test;

USE ha_protXML_modification_info_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin protXML_modification_info;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN protXML_modification_info SONAME 'ha_protXML_modification_info.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_modification_info CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_modification_info;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_modification_info CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	group_number INT,
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_modification_info CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with missing field "group_number")' AS '> task <';
CREATE TABLE fail (
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_modification_info CONNECTION = "/data/tmp/prot.xml";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(group_number) AS '> count of records in test <' FROM test;

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN protXML_modification_info;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_protXML_modification_info_test;

SELECT '---------------------------------------------------------------------' AS divide;
SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_protXML_mod_aminoacid_mass_test;
CREATE DATABASE ha_protXML_mod_aminoacid_mass_test;

USE ha_protXML_mod_aminoacid_mass_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin protXML_mod_aminoacid_mass;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN protXML_mod_aminoacid_mass SONAME 'ha_protXML_mod_aminoacid_mass.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_mod_aminoacid_mass CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_mod_aminoacid_mass;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_mod_aminoacid_mass CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	group_number INT,
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_mod_aminoacid_mass CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with missing field "group_number")' AS '> task <';
CREATE TABLE fail (
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_mod_aminoacid_mass CONNECTION = "/data/tmp/prot.xml";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(group_number) AS '> count of records in test <' FROM test;

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN protXML_mod_aminoacid_mass;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_protXML_mod_aminoacid_mass_test;

SELECT '---------------------------------------------------------------------' AS divide;
SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_protXML_indistinguishable_peptide_modification_info_test;
CREATE DATABASE ha_protXML_indistinguishable_peptide_modification_info_test;

USE ha_protXML_indistinguishable_peptide_modification_info_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin protXML_indistinguishable_peptide_modification_info;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN protXML_indistinguishable_peptide_modification_info SONAME 'ha_protXML_indistinguishable_peptide_modification_info.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide_modification_info CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide_modification_info;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide_modification_info CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	group_number INT,
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide_modification_info CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with missing field "group_number")' AS '> task <';
CREATE TABLE fail (
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide_modification_info CONNECTION = "/data/tmp/prot.xml";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(group_number) AS '> count of records in test <' FROM test;

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN protXML_indistinguishable_peptide_modification_info;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_protXML_indistinguishable_peptide_modification_info_test;

SELECT '---------------------------------------------------------------------' AS divide;
SELECT 'creating database' AS '> task <';
DROP DATABASE IF EXISTS ha_protXML_indistinguishable_peptide_mod_aminoacid_mass_test;
CREATE DATABASE ha_protXML_indistinguishable_peptide_mod_aminoacid_mass_test;

USE ha_protXML_indistinguishable_peptide_mod_aminoacid_mass_test;

SELECT 'trying to remove a previous version of plugin' AS '> task <';
UNINSTALL plugin protXML_indistinguishable_peptide_mod_aminoacid_mass;
SELECT 'installing plugin' AS '> task <';
INSTALL PLUGIN protXML_indistinguishable_peptide_mod_aminoacid_mass SONAME 'ha_protXML_indistinguishable_peptide_mod_aminoacid_mass.so';

SELECT 'creating table test' AS '> task <';
CREATE TABLE test (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide_mod_aminoacid_mass CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with needs CONNECTION)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide_mod_aminoacid_mass;

SELECT 'creating table fail (should fail with table does not exist)' AS '> task <';
CREATE TABLE fail (
	group_number VARCHAR(16),
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide_mod_aminoacid_mass CONNECTION = "does not exist filename";

SELECT 'creating table fail (should fail with wrong field name / field type combo)' AS '> task <';
CREATE TABLE fail (
	group_number INT,
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide_mod_aminoacid_mass CONNECTION = "/data/tmp/prot.xml";

SELECT 'creating table fail (should fail with missing field "group_number")' AS '> task <';
CREATE TABLE fail (
	pseudo_name VARCHAR(16),
	probability FLOAT
) ENGINE = protXML_indistinguishable_peptide_mod_aminoacid_mass CONNECTION = "/data/tmp/prot.xml";

SELECT 'selecting from table that does not exists' AS '> task <';
SELECT * FROM does_not_exist;

SELECT 'selecting all fields and records from test' AS '> task <';
SELECT * FROM test;
SELECT COUNT(group_number) AS '> count of records in test <' FROM test;

SELECT 'dropping table test' AS '> task <';
DROP TABLE test;

SELECT 'uninstalling plugin' AS '> task <';
UNINSTALL PLUGIN protXML_indistinguishable_peptide_mod_aminoacid_mass;

SELECT 'dropping database' AS '> task <';
DROP DATABASE ha_protXML_indistinguishable_peptide_mod_aminoacid_mass_test;

SELECT '---------------------------------------------------------------------' AS divide;

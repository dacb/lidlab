USE efb_examples;

DROP TABLE IF EXISTS s_1_fastq;
CREATE TABLE s_1_fastq (
	id VARCHAR(32),
	sequence VARCHAR(75),
	quality VARCHAR(75)
) ENGINE = fastq CONNECTION = "/data/meta2/s_1.fastq";
show warnings;

DROP TABLE IF EXISTS s_1_fasta;
CREATE TABLE s_1_fasta (
	id VARCHAR(32),
	sequence BLOB
) ENGINE = fasta  CONNECTION = "/data/tmp/s_1.fasta";
INSERT INTO s_1_fasta (id, sequence) SELECT fq.id, fq.sequence FROM s_1_fastq AS fq;

show warnings;

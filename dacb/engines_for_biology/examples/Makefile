.SUFFIXES: .sql .log

MYSQL = /usr/local/mysql/bin/mysql

EXAMPLE_SQL = setup_db.sql install_plugins.sql convert_fastq_2_fasta.sql uninstall_plugins.sql
EXAMPLE_LOG = $(EXAMPLE_SQL:.sql=.log)

all: $(EXAMPLE_LOG)

clean:
	\rm -rf $(EXAMPLE_LOG)

.sql.log:
	cat $< | $(MYSQL) > $@

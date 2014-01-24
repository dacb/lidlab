/*  Copyright (C) 2009 David A. C. Beck

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <mysql.h>

#include "protXML.h"

#define PROTXML_PROTEIN_DEFAULT_ROOT_ALLOC 8192

#ifdef ENABLE_DISCOVER_HANDLER
int protXML_protein_discover_handler(handlerton *hton, THD* thd, const char *db, const char *name, uchar **frmblob, size_t *frmlen);
#endif

typedef struct st_protXML_protein_share {
	char *table_name;

	MEM_ROOT mem_root;

	size_t table_name_length;

	char *filename;
	size_t filename_length;

	struct st_protXML_protein_share_position {
		int end;
	} filepos;

	uint use_count;
	pthread_mutex_t mutex;
	THR_LOCK lock;
} PROTXML_PROTEIN_SHARE;

class ha_protXML_protein: public handler {
	THR_LOCK_DATA lock;	/* MySQL based lock for handler */
	PROTXML_PROTEIN_SHARE *share;	/* shared lock */

	int init_data_file();
	File protXML_protein_file;
	struct protXML_parser_data *pxpd;

	/* position data for reordering */
	struct ha_protXML_position {
		struct ha_protXML_position_protein_group {
			int current;
			int next;
			int end;
		} protein_group;
		struct ha_protXML_position_protein {
			int current;
			int next;
			int end;
		} protein;
	} iterator;

	/* do we know (from a full scan) the # of records in file */
	bool record_count_known;

	MEM_ROOT blob;

	int create_table_check(const char *name, TABLE *form, HA_CREATE_INFO *create_info);                      

	int fetch_row(uchar *buf);

	int pack_io_buffer(const char *fieldname);

	uchar byte_buffer[IO_SIZE];	/* starting buffer for io_buffer */
	String io_buffer;

public:
	ha_protXML_protein(handlerton *hton, TABLE_SHARE *table_arg);

	~ha_protXML_protein();

	const char *table_type() const { return "PROTXML_PROTEIN"; }

	const char *index_type(uint inx) { return "NONE"; }

	const char **bas_ext() const;

	ulonglong table_flags() const { return HA_NO_TRANSACTIONS | HA_REC_NOT_IN_SEQ | HA_NO_AUTO_INCREMENT; }

	ulong index_flags(uint inx, uint part, bool all_parts) const { return 0; }

	/* required */
	int open(const char *name, int mode, uint test_if_locked);    
	int close(void);                                              
	int rnd_init(bool scan = 1);
	int rnd_next(uchar *buf);                                      
	int rnd_end(void);
	int rnd_pos(uchar * buf, uchar *pos);                           
	void position(const uchar *record);                            
	int info(uint);                                               
	int create(const char *name, TABLE *form, HA_CREATE_INFO *create_info);                      
	THR_LOCK_DATA **store_lock(THD *thd, THR_LOCK_DATA **to, enum thr_lock_type lock_type);     
};

#define PROTXML_VARCHAR "VARCHAR"
#define PROTXML_FLOAT "FLOAT"
#define PROTXML_LONGTEXT "LONGTEXT"
#define PROTXML_LONG "INT"

enum protXML_protein_field { GROUP_NUMBER = 0, PROTEIN_NAME = 1, N_INDISTINGUISHABLE_PROTEINS = 2, PROBABILITY = 3, PERCENT_COVERAGE = 4, UNIQUE_STRIPPED_PEPTIDES = 5, GROUP_SIBLING_ID = 6, TOTAL_NUMBER_PEPTIDES = 7, SUBSUMING_PROTEIN_ENTRY = 8, PCT_SPECTRUM_IDS = 9, CONFIDENCE = 10, PROTEIN_DESCRIPTION = 11, IPI_NAME = 12, REFSEQ_NAME = 13, SWISSPROT_NAME = 14, ENSEMBL_NAME = 15, TREMBL_NAME = 16, LOCUS_LINK_NAME = 17, FLYBASE = 18, DONE = 19 };

struct protXML_protein_field_info {
	const char *fieldname;
	enum protXML_protein_field state;
	enum protXML_protein_field next_state;
	uint type;
	const char *type_text;
} protXML_protein_fields[] = { \
	{ PROTXML_GROUP_NUMBER, GROUP_NUMBER, PROTEIN_NAME, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	{ PROTXML_PROTEIN_NAME, PROTEIN_NAME, N_INDISTINGUISHABLE_PROTEINS, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	{ PROTXML_N_INDISTINGUISHABLE_PROTEINS, N_INDISTINGUISHABLE_PROTEINS, PROBABILITY, MYSQL_TYPE_LONG, PROTXML_LONG }, \
	{ PROTXML_PROBABILITY, PROBABILITY, PERCENT_COVERAGE, MYSQL_TYPE_FLOAT, PROTXML_FLOAT }, \
	{ PROTXML_PERCENT_COVERAGE, PERCENT_COVERAGE, UNIQUE_STRIPPED_PEPTIDES, MYSQL_TYPE_FLOAT, PROTXML_FLOAT }, \
	{ PROTXML_UNIQUE_STRIPPED_PEPTIDES, UNIQUE_STRIPPED_PEPTIDES, GROUP_SIBLING_ID, MYSQL_TYPE_BLOB, PROTXML_LONGTEXT }, \
	{ PROTXML_GROUP_SIBLING_ID, GROUP_SIBLING_ID, TOTAL_NUMBER_PEPTIDES, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	{ PROTXML_TOTAL_NUMBER_PEPTIDES, TOTAL_NUMBER_PEPTIDES, SUBSUMING_PROTEIN_ENTRY, MYSQL_TYPE_LONG, PROTXML_LONG }, \
	{ PROTXML_SUBSUMING_PROTEIN_ENTRY, SUBSUMING_PROTEIN_ENTRY, PCT_SPECTRUM_IDS, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	{ PROTXML_PCT_SPECTRUM_IDS, PCT_SPECTRUM_IDS, CONFIDENCE, MYSQL_TYPE_FLOAT, PROTXML_FLOAT }, \
	{ PROTXML_CONFIDENCE, CONFIDENCE, PROTEIN_DESCRIPTION, MYSQL_TYPE_FLOAT, PROTXML_FLOAT }, \
	{ PROTXML_PROTEIN_DESCRIPTION, PROTEIN_DESCRIPTION, IPI_NAME, MYSQL_TYPE_BLOB, PROTXML_LONGTEXT }, \
	{ PROTXML_IPI_NAME, IPI_NAME, REFSEQ_NAME, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	{ PROTXML_REFSEQ_NAME, REFSEQ_NAME, SWISSPROT_NAME, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	{ PROTXML_SWISSPROT_NAME, SWISSPROT_NAME, ENSEMBL_NAME, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	{ PROTXML_ENSEMBL_NAME, ENSEMBL_NAME, TREMBL_NAME, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	{ PROTXML_TREMBL_NAME, TREMBL_NAME, LOCUS_LINK_NAME, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	{ PROTXML_LOCUS_LINK_NAME, LOCUS_LINK_NAME, FLYBASE, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	{ PROTXML_FLYBASE, FLYBASE, DONE, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	NULL \
};

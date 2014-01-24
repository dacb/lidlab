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

#define PROTXML_MODIFICATION_INFO_DEFAULT_ROOT_ALLOC 8192

#ifdef ENABLE_DISCOVER_HANDLER
int protXML_modification_info_discover_handler(handlerton *hton, THD* thd, const char *db, const char *name, uchar **frmblob, size_t *frmlen);
#endif

typedef struct st_protXML_modification_info_share {
	char *table_name;

	MEM_ROOT mem_root;

	size_t table_name_length;

	char *filename;
	size_t filename_length;

	struct st_protXML_modification_info_share_position {
		int end;
	} filepos;

	uint use_count;
	pthread_mutex_t mutex;
	THR_LOCK lock;
} PROTXML_MODIFICATION_INFO_SHARE;

class ha_protXML_modification_info: public handler {
	THR_LOCK_DATA lock;	/* MySQL based lock for handler */
	PROTXML_MODIFICATION_INFO_SHARE *share;	/* shared lock */

	int init_data_file();
	File protXML_modification_info_file;
	struct protXML_parser_data *pxpd;

	/* position data for reordering */
	struct ha_protXML_position {
		struct ha_protXML_position_protein_group {
			int current;
			int next;
			int end;
		} protein_group;
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
	ha_protXML_modification_info(handlerton *hton, TABLE_SHARE *table_arg);

	~ha_protXML_modification_info();

	const char *table_type() const { return "PROTXML_MODIFICATION_INFO"; }

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
#define PROTXML_DOUBLE "DOUBLE"
#define PROTXML_LONG "INT"

enum protXML_modification_info_field { GROUP_NUMBER = 0, PSEUDO_NAME = 1, PROBABILITY = 2, DONE = 3 };

struct protXML_modification_info_field_info {
	const char *fieldname;
	enum protXML_modification_info_field state;
	enum protXML_modification_info_field next_state;
	uint type;
	const char *type_text;
} protXML_modification_info_fields[] = { \
	{ PROTXML_GROUP_NUMBER, GROUP_NUMBER, PSEUDO_NAME, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	{ PROTXML_PSEUDO_NAME, PSEUDO_NAME, PROBABILITY, MYSQL_TYPE_VARCHAR, PROTXML_VARCHAR }, \
	{ PROTXML_PROBABILITY, PROBABILITY, DONE, MYSQL_TYPE_FLOAT, PROTXML_FLOAT }, \
	NULL \
};

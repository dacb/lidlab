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

#include <mysql_version.h>

#include <mysql.h>

#include "transparent_file.h"

#define FASTQ	".fastq"

#define FASTQ_DEFAULT_ROOT_ALLOC 8192

#ifdef ENABLE_DISCOVER_HANDLER
int fastq_discover_handler(handlerton *hton, THD* thd, const char *db, const char *name, uchar **frmblob, size_t *frmlen);
#endif

typedef struct st_fastq_share {
	char *table_name;

	MEM_ROOT mem_root;

	size_t table_name_length;

	char *filename;
	size_t filename_length;

	struct st_fastq_share_position {
		my_off_t end;
	} filepos;

	bool write_file_opened;
	File write_file;

	uint use_count;
	pthread_mutex_t mutex;
	THR_LOCK lock;
} FASTQ_SHARE;

class ha_fastq: public handler {
	THR_LOCK_DATA lock;	/* MySQL based lock for handler */
	FASTQ_SHARE *share;	/* shared lock */

	int init_data_file();
	Transparent_file *file_buf;
	File fastq_file;

	/* position data for reordering */
	struct ha_fastq_file_position {
		my_off_t current;
		my_off_t next;
		my_off_t end;
	} filepos;

	/* do we know (from a full scan) the # of records in file */
	bool record_count_known;

	MEM_ROOT blob;

	int create_table_check(const char *name, TABLE *form, HA_CREATE_INFO *create_info);                      

	int fetch_row(uchar *buf);

	int pack_io_buffer(const char *fieldname);

	uchar byte_buffer[IO_SIZE];	/* starting buffer for io_buffer */
	String io_buffer;

	int serialize_row(uchar *buf);

public:
	ha_fastq(handlerton *hton, TABLE_SHARE *table_arg);

	~ha_fastq() {
		/* free memory associated with file buffers */
		if (file_buf)
			delete file_buf;
	}

	const char *table_type() const { return "FASTQ"; }

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
	int write_row(uchar *buf);
	int rnd_pos(uchar * buf, uchar *pos);                           
	void position(const uchar *record);                            
	int info(uint);                                               
	int create(const char *name, TABLE *form, HA_CREATE_INFO *create_info);                      
	THR_LOCK_DATA **store_lock(THD *thd, THR_LOCK_DATA **to, enum thr_lock_type lock_type);     
};

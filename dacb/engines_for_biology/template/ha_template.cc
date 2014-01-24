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

#ifdef USE_PRAGMA_IMPLEMENTATION
#pragma implementation
#endif

#include <mysql_priv.h>
#include "ha_@engine@.h"
#include <mysql/plugin.h>
#include <my_dir.h>

/* this is only defined in mysql_priv if MYSQL_SERVER is defined, so we manually declare it here */
Field *find_field_in_table_sef(TABLE *table, const char *fieldname);

/* globals for get and free share methods */
static HASH @engine@_open_tables;	/* hash of open tables */
pthread_mutex_t @engine@_mutex;		/* mutex for hash */

/* create new handler singleton */
static handler* @engine@_create_handler(handlerton *hton, TABLE_SHARE *table, MEM_ROOT *mem_root) {
	return new (mem_root) ha_@engine@(hton, table);
}

/* used by @engine@_init_func to find a table in the share */
static uchar *@engine@_get_key(@ENGINE@_SHARE *share, size_t *length, my_bool not_used __attribute__((unused))) {
	*length = share->table_name_length;
	return (uchar *)share->table_name;
}

/* plugin initialization function */
static int @engine@_init_func(void *p) {
	handlerton *@engine@_hton;

	DBUG_ENTER("@engine@_init_func");

	@engine@_hton = (handlerton *)p;

	pthread_mutex_init(&@engine@_mutex, MY_MUTEX_INIT_FAST);

	hash_init(&@engine@_open_tables, system_charset_info, 32, 0, 0, (hash_get_key)@engine@_get_key, 0, 0);

	@engine@_hton->state = SHOW_OPTION_YES;
	@engine@_hton->db_type= DB_TYPE_UNKNOWN;
	@engine@_hton->flags= HTON_ALTER_NOT_SUPPORTED | HTON_CAN_RECREATE | HTON_TEMPORARY_NOT_SUPPORTED | HTON_NO_PARTITION;

	@engine@_hton->create = @engine@_create_handler;
#ifdef ENABLE_DISCOVER_HANDLER
	@engine@_hton->discover = @engine@_discover_handler;
#endif

	DBUG_RETURN(0);
}

/* plugin destructor */
static int @engine@_done_func(void *p) {
	int retval = 0;

	DBUG_ENTER("@engine@_done_func");

	if (@engine@_open_tables.records)
		retval = 1;

	hash_free(&@engine@_open_tables);
	pthread_mutex_destroy(&@engine@_mutex);

	DBUG_RETURN(retval);
}

/* verify table parameters, e.g. connect_string, and verify referenced filename exists and is statable */
static int check_and_stat(MEM_ROOT *mem_root, @ENGINE@_SHARE *share, TABLE *table) {
	uint uretval = ER_FOREIGN_DATA_SOURCE_DOESNT_EXIST;
	MY_STAT ms;

	if (table->s->connect_string.length == 0)
		my_error(uretval, MYF(0), "Empty CONNECTION string! This should contain the @engine@ file path.");
	else if (!(my_stat(table->s->connect_string.str, &ms, MYF(0))))
		my_printf_error(uretval, "File path in CONNECTION string does not exist: %s", MYF(0), table->s->connect_string.str);
	else {
		/* setup the filename string */
		share->filename_length = table->s->connect_string.length;
		share->filename = strmake_root(mem_root, table->s->connect_string.str, table->s->connect_string.length);
		/* record the filesize */
		share->filepos.end = ms.st_size;

		uretval = 0;
	}

	DBUG_RETURN(uretval);
}

/* table shared data structure fetch or init */
static @ENGINE@_SHARE *get_share(const char *table_name, TABLE *table) {
	Field **field;
	@ENGINE@_SHARE *share = NULL, tmp_share;
	MEM_ROOT mem_root;

	DBUG_ENTER("ha_@engine@.c::get_share");

	init_alloc_root(&mem_root, 1024, 0);

	pthread_mutex_lock(&@engine@_mutex);

	tmp_share.table_name_length = strlen(table_name);
	tmp_share.table_name = strmake_root(&mem_root, table_name, tmp_share.table_name_length);
	if (check_and_stat(&mem_root, &tmp_share, table))
		goto error;

	if (!(share = (@ENGINE@_SHARE *)hash_search(&@engine@_open_tables, (uchar *)table_name, tmp_share.table_name_length))) {
		
		if (!(share = (@ENGINE@_SHARE *)memdup_root(&mem_root, (char *)&tmp_share, sizeof(*share))))
			goto error;

		share->mem_root = mem_root;

		share->use_count = 0;
		share->write_file_opened = 0;

		if (my_hash_insert(&@engine@_open_tables, (uchar *)share))
			goto error;

		thr_lock_init(&share->lock);
		pthread_mutex_init(&share->mutex, MY_MUTEX_INIT_FAST);
	} else
		free_root(&mem_root, MYF(0));

	share->use_count++;

	pthread_mutex_unlock(&@engine@_mutex);

	DBUG_RETURN(share);

	error:
		pthread_mutex_destroy(&share->mutex);
		free_root(&mem_root, MYF(0));
		DBUG_RETURN(NULL);
}

/* release share */
static int free_share(@ENGINE@_SHARE *share) {
	int retval = 0;
	MEM_ROOT mem_root = share->mem_root;
	DBUG_ENTER("free_share");

	pthread_mutex_lock(&@engine@_mutex);

	if (!--share->use_count) {
//printf("free_share: releasing share completely\n");
		if (share->write_file_opened) {
			if (my_close(share->write_file, MYF(0)))
				retval = 1;
		}

		hash_delete(&@engine@_open_tables, (uchar *)share);
		thr_lock_delete(&share->lock);
		pthread_mutex_destroy(&share->mutex);
		free_root(&mem_root, MYF(0));
	}

	pthread_mutex_unlock(&@engine@_mutex);

	DBUG_RETURN(retval);
}

/* handler constructor */
ha_@engine@::ha_@engine@(handlerton *hton, TABLE_SHARE *table_arg)
	:handler(hton, table_arg), record_count_known(0)
{
	/* setup the read buffer to start out using a 4k chuck that is part of the handler, this will change if buffer grows past IO_SIZE to dynamic memory */
	io_buffer.set((char *)byte_buffer, IO_SIZE, &my_charset_bin);
	file_buf = new Transparent_file();
}

/* handler has no local files */
static const char *ha_@engine@_exts[] = {
	NullS
};

const char **ha_@engine@::bas_ext() const {
	return ha_@engine@_exts;
}

/* open a table */
int ha_@engine@::open(const char *name, int mode, uint test_if_locked) {
	DBUG_ENTER("ha_@engine@::open");

//printf("ha_@engine@::open\n"); fflush(stdout);

	if (!(share = get_share(name, table)))
		DBUG_RETURN(HA_ERR_OUT_OF_MEM);

	pthread_mutex_lock(&share->mutex);
	filepos.end = share->filepos.end;
	pthread_mutex_unlock(&share->mutex);

	if ((@engine@_file = my_open(share->filename, O_RDONLY, MYF(MY_WME))) == -1) {
		free_share(share);
		DBUG_RETURN(ER_CANT_OPEN_FILE);
	}

	thr_lock_data_init(&share->lock, &lock, NULL);

	DBUG_RETURN(0);
}

/* close table */
int ha_@engine@::close(void) {
	int retval_free_share, retval_my_close;

	DBUG_ENTER("ha_@engine@::close");

	retval_my_close = my_close(@engine@_file, MYF(0));
	retval_free_share = free_share(share);

//printf("ha_@engine@::close\n"); fflush(stdout);

	DBUG_RETURN(retval_free_share || retval_my_close);
}

int ha_@engine@::rnd_init(bool scan) {
	DBUG_ENTER("ha_@engine@::rnd_init");

	/* rewind file and map buffer from map */
	file_buf->init_buff(@engine@_file);

	record_count_known = 0;
	stats.records = 0;
	filepos.current = 0;
	filepos.next = 0;

	init_alloc_root(&blob, @ENGINE@_DEFAULT_ROOT_ALLOC, 0);

	DBUG_RETURN(HA_ERR_WRONG_COMMAND);
}

int ha_@engine@::pack_io_buffer(const char *fieldname) {
	Field *field;

	field = find_field_in_table_sef(table, fieldname);

	if (!field) {
		return 1;
	}

//printf("%s buffer = %s\n", field->field_name, io_buffer.c_ptr_safe()); fflush(stdout);
	if (bitmap_is_set(table->read_set, field->field_index)) {
		while(io_buffer.length() > field->field_length)
			io_buffer.chop();
//printf("io buffer length = %d\nfield_length = %d\n", io_buffer.length(), field->field_length);
		/* ripped wholesalve from csv engine */
		if (field->store(io_buffer.ptr(), io_buffer.length(), io_buffer.charset(), CHECK_FIELD_WARN))
			return 0;
//printf("%s buffer made it past early exit\n", field->field_name);

		if (field->flags & BLOB_FLAG) {
			Field_blob *b = (Field_blob *)field;
			uchar *src, *tgt;
			uint length, packlength;
			packlength = b->pack_length_no_ptr();
			length = b->get_length(b->ptr);
			memcpy_fixed(&src, b->ptr + packlength, sizeof(char *));
			if (src) {
				tgt= (uchar *)alloc_root(&blob, length);
				bmove(tgt, src, length);
				memcpy_fixed(b->ptr + packlength, &tgt, sizeof(char *));
			}
		}
	}
	return 0;
}

int ha_@engine@::rnd_next(uchar *buf) {
	int retval;

	DBUG_ENTER("ha_@engine@::rnd_next");

	/* move the record offset forward */
	filepos.current = filepos.next;

	ha_statistic_increment(&SSV::ha_read_rnd_next_count);

	/* don't scan an empty file */
//printf("rnd_next -> filepos.end = %d\n", filepos.end); fflush(stdout);
//printf("rnd_next -> filepos.current = %d\n", filepos.current); fflush(stdout);
	if (!filepos.end)
		DBUG_RETURN(HA_ERR_END_OF_FILE);

	if ((retval = fetch_row(buf)))
		DBUG_RETURN(retval);
//printf("rnd_next -> filepos.next = %d\n", filepos.current); fflush(stdout);

	stats.records++;
	DBUG_RETURN(0);
}

int ha_@engine@::rnd_end(void) {
	int retval = 0;

	DBUG_ENTER("ha_@engine@::rnd_end");

	record_count_known = 1;

	free_root(&blob, MYF(0));

	DBUG_RETURN(retval);
}

int ha_@engine@::write_row(uchar *buf) {
	int retval = 0;
	int len;

	DBUG_ENTER("ha_@engine@::write_row");

	ha_statistic_increment(&SSV::ha_write_count);

	len = serialize_row(buf);

	if (!share->write_file_opened) {
		if ((share->write_file = my_open(share->filename, O_RDWR|O_APPEND, MYF(MY_WME))) == -1) {
			my_error(ER_CANT_OPEN_FILE, MYF(0), "unable to open file");
			DBUG_RETURN(-1);
		}
		share->write_file_opened = 1;
	}

//printf("len = %d\n", len); fflush(stdout);

	if (my_write(share->write_file, (uchar *)io_buffer.ptr(), len, MYF(MY_WME | MY_NABP)))
		DBUG_RETURN(-1);

	filepos.end += len;

	stats.records++;

	DBUG_RETURN(retval);
}

void ha_@engine@::position(const uchar *record) {
	DBUG_ENTER("ha_@engine@::position");
	my_store_ptr(ref, sizeof(my_off_t), filepos.current);
	DBUG_VOID_RETURN;
}

int ha_@engine@::rnd_pos(uchar * buf, uchar *pos) {
	DBUG_ENTER("ha_@engine@::rnd_pos");
	ha_statistic_increment(&SSV::ha_read_rnd_count);
	filepos.current = my_get_ptr(pos, sizeof(my_off_t));
	DBUG_RETURN(HA_ERR_WRONG_COMMAND);
}

int ha_@engine@::info(uint flag) {
	uint uretval = 0;
	MY_STAT *ms;

	DBUG_ENTER("ha_@engine@::info");

	if (!record_count_known && stats.records < 2)
		stats.records = 2;

	DBUG_RETURN(uretval);
}

THR_LOCK_DATA **ha_@engine@::store_lock(THD *thd, THR_LOCK_DATA **to, enum thr_lock_type lock_type) {
//printf("ha_@engine@::store_lock (previous = %d, now = %d)\n", lock.type, lock_type);
	if (lock_type != TL_IGNORE && lock.type == TL_UNLOCK)
		lock.type=lock_type;
	*to++= &lock;

	return to;
}

int ha_@engine@::create(const char *name, TABLE *table_arg, HA_CREATE_INFO *create_info) {
	int retval;
	MEM_ROOT mem_root;
	@ENGINE@_SHARE tmp_share;
	uint i;
	bool found_id = 0, found_sequence = 0;

	DBUG_ENTER("ha_@engine@::create");

	/* use engine specific table creation checks */
	retval = create_table_check(name, table_arg, create_info);
	if (retval != 0)
		goto error;

	/* check if file exists */
	init_alloc_root(&mem_root, @ENGINE@_DEFAULT_ROOT_ALLOC, 0);

	retval = check_and_stat(&mem_root, &tmp_share, table_arg);

	free_root(&mem_root, MYF(0));

error:
	DBUG_RETURN(retval);
}

struct st_mysql_storage_engine @engine@_storage_engine = { MYSQL_HANDLERTON_INTERFACE_VERSION };

mysql_declare_plugin(@engine@) {
	MYSQL_STORAGE_ENGINE_PLUGIN,
	&@engine@_storage_engine,
	"@ENGINE@",
	"Dave Beck, dacb@u.washington.edu",
	"@ENGINE@ storage engine",
	PLUGIN_LICENSE_BSD,
	@engine@_init_func, /* Plugin Init */
	@engine@_done_func, /* Plugin Deinit */
	0x00042 /* 0.42 */,
	NULL,                       /* status variables                */
	NULL,                       /* system variables                */
	NULL                        /* config options                  */
}
mysql_declare_plugin_end;

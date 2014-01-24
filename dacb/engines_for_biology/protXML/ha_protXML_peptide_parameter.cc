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
#include <mysql/plugin.h>
#include <my_dir.h>

#include "ha_protXML_peptide_parameter.h"

/* this is only defined in mysql_priv if MYSQL_SERVER is defined, so we manually declare it here */
Field *find_field_in_table_sef(TABLE *table, const char *fieldname);

/* globals for get and free share methods */
static HASH protXML_peptide_parameter_open_tables;	/* hash of open tables */
pthread_mutex_t protXML_peptide_parameter_mutex;		/* mutex for hash */

/* create new handler singleton */
static handler* protXML_peptide_parameter_create_handler(handlerton *hton, TABLE_SHARE *table, MEM_ROOT *mem_root) {
#ifdef DEBUG
printf("protXML_peptide_parameter_create_handler\n"); fflush(stdout);
#endif
	return new (mem_root) ha_protXML_peptide_parameter(hton, table);
}

/* used by protXML_peptide_parameter_init_func to find a table in the share */
static uchar *protXML_peptide_parameter_get_key(PROTXML_PEPTIDE_PARAMETER_SHARE *share, size_t *length, my_bool not_used __attribute__((unused))) {
#ifdef DEBUG
printf("protXML_peptide_parameter_get_key\n"); fflush(stdout);
#endif
	*length = share->table_name_length;
	return (uchar *)share->table_name;
}

/* plugin initialization function */
static int protXML_peptide_parameter_init_func(void *p) {
	handlerton *protXML_peptide_parameter_hton;
#ifdef DEBUG
printf("protXML_peptide_parameter_init_func\n"); fflush(stdout);
#endif

	DBUG_ENTER("protXML_peptide_parameter_init_func");

	protXML_peptide_parameter_hton = (handlerton *)p;

	pthread_mutex_init(&protXML_peptide_parameter_mutex, MY_MUTEX_INIT_FAST);

	hash_init(&protXML_peptide_parameter_open_tables, system_charset_info, 32, 0, 0, (hash_get_key)protXML_peptide_parameter_get_key, 0, 0);

	protXML_peptide_parameter_hton->state = SHOW_OPTION_YES;
	protXML_peptide_parameter_hton->db_type= DB_TYPE_UNKNOWN;
	protXML_peptide_parameter_hton->flags= HTON_ALTER_NOT_SUPPORTED | HTON_CAN_RECREATE | HTON_TEMPORARY_NOT_SUPPORTED | HTON_NO_PARTITION;

	protXML_peptide_parameter_hton->create = protXML_peptide_parameter_create_handler;
#ifdef ENABLE_DISCOVER_HANDLER
	protXML_peptide_parameter_hton->discover = protXML_peptide_parameter_discover_handler;
#endif

	DBUG_RETURN(0);
}

/* plugin destructor */
static int protXML_peptide_parameter_done_func(void *p) {
	int retval = 0;
#ifdef DEBUG
printf("protXML_peptide_parameter_done_func\n"); fflush(stdout);
#endif

	DBUG_ENTER("protXML_peptide_parameter_done_func");

	if (protXML_peptide_parameter_open_tables.records)
		retval = 1;

	hash_free(&protXML_peptide_parameter_open_tables);
	pthread_mutex_destroy(&protXML_peptide_parameter_mutex);

	DBUG_RETURN(retval);
}

/* verify table parameters, e.g. connect_string, and verify referenced filename exists and is statable */
static int check_and_stat(MEM_ROOT *mem_root, PROTXML_PEPTIDE_PARAMETER_SHARE *share, TABLE *table) {
	uint uretval = ER_FOREIGN_DATA_SOURCE_DOESNT_EXIST;
	MY_STAT ms;
#ifdef DEBUG
printf("check_and_stat\n"); fflush(stdout);
#endif

	if (table->s->connect_string.length == 0)
		my_error(uretval, MYF(0), "Empty CONNECTION string! This should contain the protXML_peptide_parameter file path.");
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
static PROTXML_PEPTIDE_PARAMETER_SHARE *get_share(const char *table_name, TABLE *table) {
	Field **field;
	PROTXML_PEPTIDE_PARAMETER_SHARE *share = NULL, tmp_share;
	MEM_ROOT mem_root;
#ifdef DEBUG
printf("ha_protXML_peptide_parameter.c::get_share\n"); fflush(stdout);
#endif

	DBUG_ENTER("ha_protXML_peptide_parameter.c::get_share");

	init_alloc_root(&mem_root, 1024, 0);

	pthread_mutex_lock(&protXML_peptide_parameter_mutex);

	tmp_share.table_name_length = strlen(table_name);
	tmp_share.table_name = strmake_root(&mem_root, table_name, tmp_share.table_name_length);
	if (check_and_stat(&mem_root, &tmp_share, table))
		goto error;

	if (!(share = (PROTXML_PEPTIDE_PARAMETER_SHARE *)hash_search(&protXML_peptide_parameter_open_tables, (uchar *)table_name, tmp_share.table_name_length))) {
		
		if (!(share = (PROTXML_PEPTIDE_PARAMETER_SHARE *)memdup_root(&mem_root, (char *)&tmp_share, sizeof(*share))))
			goto error;

		share->mem_root = mem_root;

		share->use_count = 0;

		if (my_hash_insert(&protXML_peptide_parameter_open_tables, (uchar *)share))
			goto error;

		thr_lock_init(&share->lock);
		pthread_mutex_init(&share->mutex, MY_MUTEX_INIT_FAST);
	} else
		free_root(&mem_root, MYF(0));

	share->use_count++;

	pthread_mutex_unlock(&protXML_peptide_parameter_mutex);

	DBUG_RETURN(share);

	error:
		pthread_mutex_destroy(&share->mutex);
		free_root(&mem_root, MYF(0));
		DBUG_RETURN(NULL);
}

/* release share */
static int free_share(PROTXML_PEPTIDE_PARAMETER_SHARE *share) {
	int retval = 0;
	MEM_ROOT mem_root = share->mem_root;
	DBUG_ENTER("free_share");
#ifdef DEBUG
printf("ha_protXML_peptide_parameter.c::free_share\n"); fflush(stdout);
#endif

	pthread_mutex_lock(&protXML_peptide_parameter_mutex);

	if (!--share->use_count) {
#ifdef DEBUG
printf("free_share: releasing share completely\n"); fflush(stdout);
#endif

		hash_delete(&protXML_peptide_parameter_open_tables, (uchar *)share);
		thr_lock_delete(&share->lock);
		pthread_mutex_destroy(&share->mutex);
		free_root(&mem_root, MYF(0));
	}

	pthread_mutex_unlock(&protXML_peptide_parameter_mutex);

	DBUG_RETURN(retval);
}

/* handler constructor */
ha_protXML_peptide_parameter::ha_protXML_peptide_parameter(handlerton *hton, TABLE_SHARE *table_arg)
	:handler(hton, table_arg), record_count_known(0)
{
#ifdef DEBUG
printf("ha_protXML_peptide_parameter\n"); fflush(stdout);
#endif

	pxpd = NULL;

	/* setup the read buffer to start out using a 4k chuck that is part of the handler, this will change if buffer grows past IO_SIZE to dynamic memory */
	io_buffer.set((char *)byte_buffer, IO_SIZE, &my_charset_bin);
}

/* handler descructor */
ha_protXML_peptide_parameter::~ha_protXML_peptide_parameter(void) {
#ifdef DEBUG
printf("~ha_protXML_peptide_parameter\n"); fflush(stdout);
#endif
	if (pxpd)
		protXML_parser_data_free(pxpd);
}

/* handler has no local files */
static const char *ha_protXML_peptide_parameter_exts[] = {
	NullS
};

const char **ha_protXML_peptide_parameter::bas_ext() const {
	return ha_protXML_peptide_parameter_exts;
}

/* open a table */
int ha_protXML_peptide_parameter::open(const char *name, int mode, uint test_if_locked) {
	DBUG_ENTER("ha_protXML_peptide_parameter::open");

#ifdef DEBUG
printf("ha_protXML_peptide_parameter::open\n"); fflush(stdout);
#endif

	if (!(share = get_share(name, table)))
		DBUG_RETURN(HA_ERR_OUT_OF_MEM);

	pxpd = protXML_parse_file(share->filename);

	thr_lock_data_init(&share->lock, &lock, NULL);

	DBUG_RETURN(0);
}

/* close table */
int ha_protXML_peptide_parameter::close(void) {
	int retval_free_share, retval_my_close;

#ifdef DEBUG
printf("ha_protXML_peptide_parameter::close\n"); fflush(stdout);
#endif

	DBUG_ENTER("ha_protXML_peptide_parameter::close");

	retval_free_share = free_share(share);

	DBUG_RETURN(retval_free_share || retval_my_close);
}

int ha_protXML_peptide_parameter::rnd_init(bool scan) {
#ifdef DEBUG
printf("ha_protXML_peptide_parameter::close\n"); fflush(stdout);
#endif

	DBUG_ENTER("ha_protXML_peptide_parameter::rnd_init");

	record_count_known = 1;
	stats.records = pxpd->protein_group->ps;
	iterator.protein_group.current = 0;
	iterator.protein_group.next = 0;
	iterator.protein_group.end = pxpd->protein_group->ps;

	init_alloc_root(&blob, PROTXML_PEPTIDE_PARAMETER_DEFAULT_ROOT_ALLOC, 0);

	DBUG_RETURN(HA_ERR_WRONG_COMMAND);
}

int ha_protXML_peptide_parameter::pack_io_buffer(const char *fieldname) {
	Field *field;

#ifdef DEBUG
printf("ha_protXML_peptide_parameter::pack_io_buffer\n"); fflush(stdout);
#endif

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

int ha_protXML_peptide_parameter::rnd_next(uchar *buf) {
	int retval;
#ifdef DEBUG
printf("ha_protXML_peptide_parameter::rnd_next\n"); fflush(stdout);
#endif

	DBUG_ENTER("ha_protXML_peptide_parameter::rnd_next");

	ha_statistic_increment(&SSV::ha_read_rnd_next_count);

	iterator.protein_group.current = iterator.protein_group.next;
	/* trap for empty list */
	if (!iterator.protein_group.end)
		DBUG_RETURN(HA_ERR_END_OF_FILE);

	if ((retval = fetch_row(buf)))
		DBUG_RETURN(retval);

	DBUG_RETURN(0);
}

int ha_protXML_peptide_parameter::rnd_end(void) {
	int retval = 0;
#ifdef DEBUG
printf("ha_protXML_peptide_parameter::rnd_end\n"); fflush(stdout);
#endif

	DBUG_ENTER("ha_protXML_peptide_parameter::rnd_end");

	free_root(&blob, MYF(0));

	DBUG_RETURN(retval);
}

void ha_protXML_peptide_parameter::position(const uchar *record) {
	DBUG_ENTER("ha_protXML_peptide_parameter::position");
	my_store_ptr(ref, sizeof(int), iterator.protein_group.current);
	DBUG_VOID_RETURN;
}

int ha_protXML_peptide_parameter::rnd_pos(uchar * buf, uchar *pos) {
	DBUG_ENTER("ha_protXML_peptide_parameter::rnd_pos");
	ha_statistic_increment(&SSV::ha_read_rnd_count);
	iterator.protein_group.current = my_get_ptr(pos, sizeof(int));
	DBUG_RETURN(HA_ERR_WRONG_COMMAND);
}

int ha_protXML_peptide_parameter::info(uint flag) {
	uint uretval = 0;
	MY_STAT *ms;

	DBUG_ENTER("ha_protXML_peptide_parameter::info");

	DBUG_RETURN(uretval);
}

THR_LOCK_DATA **ha_protXML_peptide_parameter::store_lock(THD *thd, THR_LOCK_DATA **to, enum thr_lock_type lock_type) {
#ifdef DEBUG
printf("ha_protXML_peptide_parameter::store_lock (previous = %d, now = %d)\n", lock.type, lock_type); fflush(stdout);
#endif
	if (lock_type != TL_IGNORE && lock.type == TL_UNLOCK)
		lock.type=lock_type;
	*to++= &lock;

	return to;
}

int ha_protXML_peptide_parameter::create(const char *name, TABLE *table_arg, HA_CREATE_INFO *create_info) {
	int retval;
	MEM_ROOT mem_root;
	PROTXML_PEPTIDE_PARAMETER_SHARE tmp_share;
	uint i;
#ifdef DEBUG
printf("ha_protXML_peptide_parameter::create\n"); fflush(stdout);
#endif

	DBUG_ENTER("ha_protXML_peptide_parameter::create");

	/* use engine specific table creation checks */
	retval = create_table_check(name, table_arg, create_info);
	if (retval != 0)
		goto error;

	/* check if file exists */
	init_alloc_root(&mem_root, PROTXML_PEPTIDE_PARAMETER_DEFAULT_ROOT_ALLOC, 0);

	retval = check_and_stat(&mem_root, &tmp_share, table_arg);

	free_root(&mem_root, MYF(0));

error:
	DBUG_RETURN(retval);
}

int ha_protXML_peptide_parameter::fetch_row(uchar *buf) {
	uint retval = 0;
//	enum protXML_peptide_parameter_field state = GROUP_NUMBER;
	struct protein_group *pg;

	DBUG_ENTER("ha_protXML_peptide_parameter::fetch_row");

	free_root(&blob, MYF(MY_MARK_BLOCKS_FREE));

	if (iterator.protein_group.current == iterator.protein_group.end)
		DBUG_RETURN(HA_ERR_END_OF_FILE);
	iterator.protein_group.next++;

	pg = (struct protein_group *)pxpd->protein_group->p[iterator.protein_group.current];

//printf("fetch_row, row_end_offset = %d\n", row_end_offset); fflush(stdout);

	/* copy into memory */
	memset(buf, 0, table->s->null_bytes);
	/* three fields */
	io_buffer.length(0);
	if (pg->group_number_found)
		io_buffer.append(pg->group_number);
	pack_io_buffer(PROTXML_GROUP_NUMBER);

	io_buffer.length(0);
	if (pg->pseudo_name_found)
		io_buffer.append(pg->pseudo_name);
	pack_io_buffer(PROTXML_PSEUDO_NAME);

	io_buffer.length(0);
	if (pg->probability_found)
		io_buffer.append(pg->probability);
	pack_io_buffer(PROTXML_PROBABILITY);

	DBUG_RETURN(retval);
}

int ha_protXML_peptide_parameter::create_table_check(const char *name, TABLE *table_arg, HA_CREATE_INFO *create_info) {
	enum protXML_peptide_parameter_field state = GROUP_NUMBER;
	DBUG_ENTER("ha_protXML_peptide_parameter::create_table_check");

	/* check field names and types */
	while (state != DONE) {
		Field *field = find_field_in_table_sef(table_arg, protXML_peptide_parameter_fields[state].fieldname);
		if (!field) {
			my_printf_error(ER_CHECK_NOT_IMPLEMENTED, "missing '%s' field", MYF(0), protXML_peptide_parameter_fields[state].fieldname);
			DBUG_RETURN(HA_ERR_UNSUPPORTED);
		} else if (field->type() != protXML_peptide_parameter_fields[state].type) {
			my_printf_error(ER_CHECK_NOT_IMPLEMENTED, "field '%s' having any type other than %s", MYF(0), protXML_peptide_parameter_fields[state].fieldname, protXML_peptide_parameter_fields[state].type_text);
			DBUG_RETURN(HA_ERR_UNSUPPORTED);
		}
		state = protXML_peptide_parameter_fields[state].next_state;
	}

	DBUG_RETURN(0);
}

struct st_mysql_storage_engine protXML_peptide_parameter_storage_engine = { MYSQL_HANDLERTON_INTERFACE_VERSION };

mysql_declare_plugin(protXML_peptide_parameter) {
	MYSQL_STORAGE_ENGINE_PLUGIN,
	&protXML_peptide_parameter_storage_engine,
	"PROTXML_PEPTIDE_PARAMETER",
	"Dave Beck, dacb@u.washington.edu",
	"PROTXML_PEPTIDE_PARAMETER storage engine",
	PLUGIN_LICENSE_BSD,
	protXML_peptide_parameter_init_func, /* Plugin Init */
	protXML_peptide_parameter_done_func, /* Plugin Deinit */
	0x00042 /* 0.42 */,
	NULL,                       /* status variables                */
	NULL,                       /* system variables                */
	NULL                        /* config options                  */
}
mysql_declare_plugin_end;

#ifdef ENABLE_DISCOVER_HANDLER
int protXML_peptide_parameter_discover_handler(handlerton *hton, THD* thd, const char *db, const char *name, uchar **frmblob, size_t *frmlen) {
	char filename[FN_REFLEN];
	MY_STAT ms;

	DBUG_ENTER("protXML_peptide_parameter_discover");
	DBUG_PRINT("protXML_peptide_parameter_discover", ("db: %s, name: %s", db, name));

	sql_print_warning("protXML_peptide_parameter_discover_handler not implemented");

	fn_format(filename, name, db, PROTXML_PEPTIDE_PARAMETER, MY_REPLACE_EXT | MY_UNPACK_FILENAME);

  	if (!(my_stat(filename, &ms, MYF(0))))
		goto error;

error:
	my_errno = 0;
	DBUG_RETURN(1);
}
#endif


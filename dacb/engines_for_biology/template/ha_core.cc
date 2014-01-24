#ifdef USE_PRAGMA_IMPLEMENTATION
#pragma implementation
#endif

#if MYSQL_VERSION_ID > 50500
#include <my_sys.h>
#define VOID(X) (X)
#else
#include <mysql_priv.h>
#endif

#include "ha_template.h"
#include <mysql/plugin.h>
#include <my_dir.h>

#ifdef ENABLE_DISCOVER_HANDLER
int template_discover_handler(handlerton *hton, THD* thd, const char *db, const char *name, uchar **frmblob, size_t *frmlen) {
	char filename[FN_REFLEN];
	MY_STAT ms;

	DBUG_ENTER("template_discover");
	DBUG_PRINT("template_discover", ("db: %s, name: %s", db, name));

	sql_print_warning("template_discover_handler not implemented");

	fn_format(filename, name, db, TEMPLATE, MY_REPLACE_EXT | MY_UNPACK_FILENAME);

  	if (!(my_stat(filename, &ms, MYF(0))))
		goto error;

error:
	my_errno = 0;
	DBUG_RETURN(1);
}
#endif

static my_off_t find_row_end(Transparent_file *fp, my_off_t begin, my_off_t end) {
	sql_print_warning("implement find_row_end now");

	return 0;
}

int ha_template::fetch_row(uchar *buf) {
	uint retval = 0;
	my_off_t row_end_offset, offset;

	DBUG_ENTER("ha_template::fetch_row");

	free_root(&blob, MYF(MY_MARK_BLOCKS_FREE));

	/* check for EOF in next row */
	if ((row_end_offset = find_row_end(file_buf, filepos.current, filepos.end)) == 0)
		DBUG_RETURN(HA_ERR_END_OF_FILE);
	filepos.next = row_end_offset;

	sql_print_warning("fully implement ha_template::fetch_row");

	/* copy into memory */
	memset(buf, 0, table->s->null_bytes);
	offset = filepos.current + 1;
	for (Field **field=table->field; *field; field++) {
		char c;

		/* truncate read buffer */
		io_buffer.length(0);
		for (; offset < row_end_offset; ++offset) {
			c = file_buf->get_value(offset);
		}
//printf("%s buffer = %s\n", (*field)->field_name, io_buffer.c_ptr_safe()); fflush(stdout);
		if (bitmap_is_set(table->read_set, (*field)->field_index)) {
			while(io_buffer.length() > (*field)->field_length)
				io_buffer.chop();
//printf("field length = %d\nfield_length = %d\n", io_buffer.length(), (*field)->field_length);
			/* ripped wholesalve from csv engine */
			if ((*field)->store(io_buffer.ptr(), io_buffer.length(), io_buffer.charset(), CHECK_FIELD_WARN))
				DBUG_RETURN(retval);

			if ((*field)->flags & BLOB_FLAG) {
				Field_blob *b = *(Field_blob **) field;
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
	}

	DBUG_RETURN(retval);
}

int ha_template::serialize_row(uchar *buf) {
	/* this is a buffer for the output */
	char out_buffer[IO_SIZE];
	String out(out_buffer, sizeof(out_buffer), &my_charset_bin);

	DBUG_ENTER("ha_template::serialize_row");

	io_buffer.length(0);

	sql_print_warning("implement ha_template::fetch_row");

	return io_buffer.length();
}

int ha_template::create_table_check(const char *name, TABLE *table_arg, HA_CREATE_INFO *create_info) {
	DBUG_ENTER("ha_template::create_table_check");

	sql_print_warning("implement ha_template::create_table_check");

	DBUG_RETURN(0);
}

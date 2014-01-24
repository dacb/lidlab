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
#include "ha_blast_fmt6.h"
#include "ha_core.h"
#include <mysql/plugin.h>
#include <my_dir.h>

/* this is only defined in mysql_priv if MYSQL_SERVER is defined, so we manually declare it here */
Field *find_field_in_table_sef(TABLE *table, const char *fieldname);

#ifdef ENABLE_DISCOVER_HANDLER
int blast_fmt6_discover_handler(handlerton *hton, THD* thd, const char *db, const char *name, uchar **frmblob, size_t *frmlen) {
	char filename[FN_REFLEN];
	MY_STAT ms;

	DBUG_ENTER("blast_fmt6_discover");
	DBUG_PRINT("blast_fmt6_discover", ("db: %s, name: %s", db, name));

	sql_print_warning("blast_fmt6_discover_handler not implemented");

	fn_format(filename, name, db, BLAST_FMT6, MY_REPLACE_EXT | MY_UNPACK_FILENAME);

  	if (!(my_stat(filename, &ms, MYF(0))))
		goto error;

error:
	my_errno = 0;
	DBUG_RETURN(1);
}
#endif

static my_off_t blast_fmt6_find_row_end(Transparent_file *fp, my_off_t begin, my_off_t end) {
	my_off_t i;

	for (i = begin; i < end; ++i) {
		char c = fp->get_value(i);
		if (c == '\n')
			return i + 1;
		if (c == '\r') {
			if (i + 1 == end || (fp->get_value(i + 1) != '\n'))
				return i;
			else
				return i + 1;
		}
	}

	return 0;
}

int ha_blast_fmt6::fetch_row(uchar *buf) {
	uint retval = 0;
	my_off_t row_end_offset, offset;
	enum blast_fmt6_field state = QUERY_ID;

	DBUG_ENTER("ha_blast_fmt6::fetch_row");

	free_root(&blob, MYF(MY_MARK_BLOCKS_FREE));

	/* check for EOF in next row */
	if ((row_end_offset = blast_fmt6_find_row_end(file_buf, filepos.current, filepos.end)) == 0)
		DBUG_RETURN(HA_ERR_END_OF_FILE);
	filepos.next = row_end_offset;

//printf("fetch_row, row_end_offset = %d\n", row_end_offset); fflush(stdout);

	/* copy into memory */
	memset(buf, 0, table->s->null_bytes);
	offset = filepos.current;
	while (state != DONE) {
		char c;

//printf("! state = %d\toffset = %d\trow_end_offset = %d\n", state, offset, row_end_offset); fflush(stdout);

		/* truncate read buffer */
		io_buffer.length(0);
		for (; offset < row_end_offset; ++offset) {
			enum blast_fmt6_field last_state = state;
			c = file_buf->get_value(offset);
//printf("> state = %d\toffset = %d\trow_end_offset = %d\tc = %c\n", state, offset, row_end_offset, c); fflush(stdout);
			if (c == '\t' || c == '\n' || c == '\r') {
				pack_io_buffer(blast_fmt6_fields[state].fieldname);
				state = blast_fmt6_fields[state].next_state;
				++offset;
				if (c == '\n' || c == '\r')
					state = DONE;
//printf("state change (%d) to (%d)\n", last_state, state); fflush(stdout);
				break;
			} else
				io_buffer.append(c);
		}
//printf("< state = %d\toffset = %d\trow_end_offset = %d\n", state, offset, row_end_offset); fflush(stdout);
	}

	DBUG_RETURN(retval);
}

int ha_blast_fmt6::serialize_row(uchar *buf) {
	enum blast_fmt6_field state = QUERY_ID;
	/* this is a buffer for the output */
	char out_buffer[IO_SIZE];
	String out(out_buffer, sizeof(out_buffer), &my_charset_bin);

	io_buffer.length(0);

	while (state != DONE) {
		Field *field = find_field_in_table_sef(table, blast_fmt6_fields[state].fieldname);
		field->val_str(&out);
		io_buffer.append(out);
		state = blast_fmt6_fields[state].next_state;
		if (state != DONE)
			io_buffer.append('\t');
	}

	io_buffer.append('\n');

	return io_buffer.length();
}

int ha_blast_fmt6::create_table_check(const char *name, TABLE *table_arg, HA_CREATE_INFO *create_info) {
	enum blast_fmt6_field state = QUERY_ID;
	DBUG_ENTER("ha_blast_fmt6::create_table_check");

	/* check field names and types */
	while (state != DONE) {
		Field *field = find_field_in_table_sef(table_arg, blast_fmt6_fields[state].fieldname);
		if (!field) {
			my_printf_error(ER_CHECK_NOT_IMPLEMENTED, "missing '%s' field", MYF(0), blast_fmt6_fields[state].fieldname);
			DBUG_RETURN(HA_ERR_UNSUPPORTED);
		} else if (field->type() != blast_fmt6_fields[state].type) {
			my_printf_error(ER_CHECK_NOT_IMPLEMENTED, "field '%s' having any type other than %s", MYF(0), blast_fmt6_fields[state].fieldname, blast_fmt6_fields[state].type_text);
			DBUG_RETURN(HA_ERR_UNSUPPORTED);
		}
		state = blast_fmt6_fields[state].next_state;
	}

	DBUG_RETURN(0);
}

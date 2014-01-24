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
#include "ha_fasta.h"
#include "ha_core.h"
#include <mysql/plugin.h>
#include <my_dir.h>

/* this is only defined in mysql_priv if MYSQL_SERVER is defined, so we manually declare it here */
Field *find_field_in_table_sef(TABLE *table, const char *fieldname);

#ifdef ENABLE_DISCOVER_HANDLER
int fasta_discover_handler(handlerton *hton, THD* thd, const char *db, const char *name, uchar **frmblob, size_t *frmlen) {
	char filename[FN_REFLEN];
	MY_STAT ms;

	DBUG_ENTER("fasta_discover");
	DBUG_PRINT("fasta_discover", ("db: %s, name: %s", db, name));

	sql_print_warning("fasta_discover_handler not implemented");

	fn_format(filename, name, db, FASTA, MY_REPLACE_EXT | MY_UNPACK_FILENAME);

  	if (!(my_stat(filename, &ms, MYF(0))))
		goto error;

	/* see http://lists.mysql.org/internals/35096 for some examples */

error:
	my_errno = 0;
	DBUG_RETURN(1);
}
#endif

static my_off_t fasta_find_row_end(Transparent_file *fp, my_off_t begin, my_off_t end) {
	enum fasta_field state = ID;
	my_off_t i;

	/* do some error checking at beginning of record */
//printf("fp->get_value(begin = %d) = %c\n", begin, fp->get_value(begin)); fflush(stdout);
	if (fp->get_value(begin) != '>') {
		return 0;
	}
	for (i = begin + 1; i < end; ++i) {
		char c = fp->get_value(i);
		switch (state) {
			case ID:
				if (c == ' ') {
					state = COMMENT;
				} else if (c == '\n' || c == '\r') {
					if (i + 1 < end && fp->get_value(i + 1) == '\n')
						++i;
					state = SEQUENCE;
				}
				break;
			case COMMENT:
				if (c == '\n' || c == '\r') {
					if (i + 1 < end && fp->get_value(i + 1) == '\n')
						++i;
					state = SEQUENCE;
				}
				break;
			case SEQUENCE:
				if (c == '\n' || c == '\r') {
					if (i + 1 < end && fp->get_value(i + 1) == '\n')
						++i;
					if (i + 1 == end || (i + 1 < end && fp->get_value(i + 1) == '>'))
						state = DONE;
				}
				break;
			case DONE:
			default:
				break;
		};
		if (state == DONE)
			return i + 1;
	}
	return i;
}

int ha_fasta::fetch_row(uchar *buf) {
	uint retval = 0;
	my_off_t row_end_offset, offset;
	enum fasta_field state = ID;

	DBUG_ENTER("ha_fasta::fetch_row");

	free_root(&blob, MYF(MY_MARK_BLOCKS_FREE));

	/* check for EOF in next row */
	if ((row_end_offset = fasta_find_row_end(file_buf, filepos.current, filepos.end)) == 0)
		DBUG_RETURN(HA_ERR_END_OF_FILE);
	filepos.next = row_end_offset;

//printf("filepos.end = %d fetch_row, row_end_offset = %d\n", filepos.end, row_end_offset); fflush(stdout);

	/* copy into memory */
	memset(buf, 0, table->s->null_bytes);

	offset = filepos.current + 1;
	while (state != DONE) {
		char c;

		/* truncate read buffer */
		io_buffer.length(0);
		for (; offset < row_end_offset; ++offset) {
			enum fasta_field last_state = state;
			c = file_buf->get_value(offset);
			switch (state) {
				case ID:
//printf("in id field\n"); fflush(stdout);
					if (c == ' ' || c == '\t') {
						while (offset + 1 < row_end_offset && file_buf->get_value(offset + 1) == ' ')
							++ offset;
						pack_io_buffer(FASTA_ID_FIELDNAME);
						state = COMMENT;
					} else if (c == '\n' || c == '\r') {
						if (offset + 1 < row_end_offset && file_buf->get_value(offset + 1) == '\n')
							++offset;
						pack_io_buffer(FASTA_ID_FIELDNAME);
						state = SEQUENCE;
					} else
						io_buffer.append(c);
					break;
				case COMMENT:
//printf("in comment field (%s)\n", io_buffer.c_ptr()); fflush(stdout);
					if (c == '\n' || c == '\r') {
						if (offset + 1 < row_end_offset && file_buf->get_value(offset + 1) == '\n')
							++offset;
						pack_io_buffer(FASTA_COMMENT_FIELDNAME);
						state = SEQUENCE;
					} else
						io_buffer.append(c);
//printf("end of in comment field state = %d %d %d\n", state, offset, row_end_offset); fflush(stdout);
					break;
				case SEQUENCE:
//printf("in sequence field\n"); fflush(stdout);
					if (c == '\n' || c == '\r') {
						if (offset + 1 == row_end_offset) {
							++offset;
							pack_io_buffer(FASTA_SEQUENCE_FIELDNAME);
							state = DONE;
						}
						continue;
					} else 
						io_buffer.append(c);
					break;
				case DONE:
				default:
					break;
			};
			if (state != last_state) {
//printf("state changed field new %d last %d\n", state, last_state); fflush(stdout);
				++offset;
				break;
			}
		}
	}

	DBUG_RETURN(retval);
}

int ha_fasta::serialize_row(uchar *buf) {
	/* this is a buffer for the output */
	char out_buffer[IO_SIZE];
	String out(out_buffer, sizeof(out_buffer), &my_charset_bin);

	io_buffer.length(0);

	io_buffer.append('>');
	
	Field *field = find_field_in_table_sef(table, FASTA_ID_FIELDNAME);
	if (field) {
		field->val_str(&out);
		io_buffer.append(out);
	} /* should report an error here? */
	field = find_field_in_table_sef(table, FASTA_COMMENT_FIELDNAME);
	if (field) {
		field->val_str(&out);
		io_buffer.append("  ");
		io_buffer.append(out);
	}
	io_buffer.append('\n');
	field = find_field_in_table_sef(table, FASTA_SEQUENCE_FIELDNAME);	/* should wrap output */
	if (field) {
		field->val_str(&out);
		io_buffer.append(out);
		io_buffer.append('\n');
	}

	return io_buffer.length();
}

int ha_fasta::create_table_check(const char *name, TABLE *table_arg, HA_CREATE_INFO *create_info) {
	uint i;
	bool found_id = 0, found_sequence = 0;
	DBUG_ENTER("ha_fasta::create_table_check");

	/* check field names and types */
	for (i = 0; i < table_arg->s->fields; ++i) {
		Field *field = table_arg->field[i];
		uint type = field->type();
		if (strcmp(field->field_name, FASTA_ID_FIELDNAME) == 0) {
			if (type != MYSQL_TYPE_VARCHAR)
				goto error_name_type_mismatch;
			found_id = 1;
		} else if (strcmp(field->field_name, FASTA_COMMENT_FIELDNAME) == 0) {
			if (type != MYSQL_TYPE_VARCHAR && type != MYSQL_TYPE_BLOB)
				goto error_name_type_mismatch;
		} else if (strcmp(field->field_name, FASTA_SEQUENCE_FIELDNAME) == 0) {
			if (type != MYSQL_TYPE_VARCHAR && type != MYSQL_TYPE_BLOB) {
error_name_type_mismatch:
				my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "field of this name with specified field type");
				DBUG_RETURN(HA_ERR_UNSUPPORTED);
			}
			found_sequence = 1;
		} else {
			my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "the specified field type");
			DBUG_RETURN(HA_ERR_UNSUPPORTED);
		}
	}

	if (!found_id || !found_sequence) {
		my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "missing 'id' and / or 'sequence' fields");
		DBUG_RETURN(HA_ERR_UNSUPPORTED);
	}

	DBUG_RETURN(0);
}

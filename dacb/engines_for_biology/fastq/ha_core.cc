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
#include "ha_fastq.h"
#include "ha_core.h"
#include <mysql/plugin.h>
#include <my_dir.h>

#ifdef ENABLE_DISCOVER_HANDLER
int fastq_discover_handler(handlerton *hton, THD* thd, const char *db, const char *name, uchar **frmblob, size_t *frmlen) {
	char filename[FN_REFLEN];
	MY_STAT ms;

	DBUG_ENTER("fastq_discover");
	DBUG_PRINT("fastq_discover", ("db: %s, name: %s", db, name));

	sql_print_warning("fastq_discover_handler not implemented");

	fn_format(filename, name, db, FASTQ, MY_REPLACE_EXT | MY_UNPACK_FILENAME);

  	if (!(my_stat(filename, &ms, MYF(0))))
		goto error;

error:
	my_errno = 0;
	DBUG_RETURN(1);
}
#endif

static my_off_t fastq_find_row_end(Transparent_file *fp, my_off_t begin, my_off_t end) {
	bool last_was_nl = 0;
	enum fastq_field state = ID;
	uint sequence_length = 0, quality_length = 0;
	my_off_t i;

	/* do some error checking at beginning of record */
	if (fp->get_value(begin) != '@') {
		return 0;
	}
	for (i = begin + 1; i < end; ++i) {
		char c = fp->get_value(i);
		switch (state) {
			case ID:
				if (c == '\n' || c == '\r') {
					/* if this is a double unix nl or DOS nl (\r\n) skip forward) */
					if (i + 1 < end && fp->get_value(i + 1) == '\n')
						++i;
					state = SEQUENCE;
				}
				break;
			case SEQUENCE:
				if (c == '\n' || c == '\r') {
					if (i + 1 < end && fp->get_value(i + 1) == '\n')
						++i;
					last_was_nl = 1;
					state = ID2;
				} else
					++sequence_length;
				break;
			case ID2:
				if (last_was_nl) {
					if (c != '+')
						return 0;
					last_was_nl = 0;
				}
				if (c == '\n' || c == '\r') {
					if (i + 1 < end && fp->get_value(i + 1) == '\n')
						++i;
					state = QUALITY;
				}
				break;
			case QUALITY:
				if (c == '\n' || c == '\r') {
					if (i + 1 < end && fp->get_value(i + 1) == '\n')
						++i;
					state = DONE;
				} else
					++quality_length;
				break;
			case DONE:
			default:
				break;
		};
		if (state == DONE) {
			if (sequence_length != quality_length) {
				sql_print_warning("bad record found where sequence_length (%d) != quality_length (%d)", sequence_length, quality_length);
				return 0;
			}
			return i + 1;
		}
	}
	if (state == DONE && i == end)	/* final record */
		return i;

	return 0;
}

int ha_fastq::fetch_row(uchar *buf) {
	uint retval = 0;
	my_off_t row_end_offset, offset;
	enum fastq_field state = ID;

	DBUG_ENTER("ha_fastq::fetch_row");

	free_root(&blob, MYF(MY_MARK_BLOCKS_FREE));

	/* check for EOF in next row */
	if ((row_end_offset = fastq_find_row_end(file_buf, filepos.current, filepos.end)) == 0)
		DBUG_RETURN(HA_ERR_END_OF_FILE);
	filepos.next = row_end_offset;

//printf("fetch_row, row_end_offset = %d\n", row_end_offset); fflush(stdout);

	/* copy into memory */
	memset(buf, 0, table->s->null_bytes);
	offset = filepos.current + 1;
	while (state != DONE) {
		char c;

//printf("state = %d\toffset = %d\trow_end_offset = %d\n", state, offset, row_end_offset); fflush(stdout);

		/* truncate read buffer */
		io_buffer.length(0);
		for (; offset < row_end_offset; ++offset) {
			enum fastq_field last_state = state;
//printf("> state = %d\toffset = %d\trow_end_offset = %d\n", state, offset, row_end_offset); fflush(stdout);
			c = file_buf->get_value(offset);
			switch (state) {
				case ID:
//printf("ID offset = %d, s = %c\n", offset, c); fflush(stdout);
					if (c == '\n' || c == '\r') {
						if (offset + 1 < row_end_offset && file_buf->get_value(offset + 1) == '\n')
							++offset;
						pack_io_buffer(FASTQ_ID_FIELDNAME);
						state = SEQUENCE;
					} else
						io_buffer.append(c);
					break;
				case SEQUENCE:
//printf("SEQ offset = %d, s = %c\n", offset, c); fflush(stdout);
					if (c == '\n' || c == '\r') {
						if (offset + 1 < row_end_offset && file_buf->get_value(offset + 1) == '\n')
							++offset;
						if (offset + 1 < row_end_offset && file_buf->get_value(offset + 1) == '+')
							++offset;
						pack_io_buffer(FASTQ_SEQUENCE_FIELDNAME);
						state = ID2;
					} else
						io_buffer.append(c);
					break;
				case ID2:
//printf("ID2 offset = %d, s = %c\n", offset, c); fflush(stdout);
					if (c == '\n' || c == '\r') {
						if (offset + 1 < row_end_offset && file_buf->get_value(offset + 1) == '\n')
							++offset;
						state = QUALITY;
					}
					break;
				case QUALITY:
//printf("QUAL offset = %d, s = %c\n", offset, c); fflush(stdout);
					if (c == '\n' || c == '\r') {
						if (offset + 1 < row_end_offset && file_buf->get_value(offset + 1) == '\n')
							++offset;
						pack_io_buffer(FASTQ_QUALITY_FIELDNAME);
						state = DONE;
					} else
						io_buffer.append(c);
					break;
				case DONE:
				default:
					break;
			};
			if (state != ID2 && state != last_state) {
				++offset;
				break;
			}
		}
	}

	DBUG_RETURN(retval);
}

int ha_fastq::serialize_row(uchar *buf) {
	/* this is a buffer for the output */
	char out_buffer[IO_SIZE];
	String out(out_buffer, sizeof(out_buffer), &my_charset_bin);
	char id_out_buffer[IO_SIZE];
	String id_out(out_buffer, sizeof(out_buffer), &my_charset_bin);

	io_buffer.length(0);

	io_buffer.append('@');
	for (Field **field=table->field ; *field ; field++)
		if (strcmp((*field)->field_name, FASTQ_ID_FIELDNAME) == 0)
			(*field)->val_str(&id_out);
	io_buffer.append(id_out);
	io_buffer.append("\n");
	for (Field **field=table->field ; *field ; field++)
		if (strcmp((*field)->field_name, FASTQ_SEQUENCE_FIELDNAME) == 0)
			(*field)->val_str(&out);
	io_buffer.append(out);
	io_buffer.append("\n+");
	io_buffer.append(id_out);
	io_buffer.append('\n');
	for (Field **field=table->field ; *field ; field++)
		if (strcmp((*field)->field_name, FASTQ_QUALITY_FIELDNAME) == 0)
			(*field)->val_str(&out);
	io_buffer.append(out);
	io_buffer.append('\n');

	return io_buffer.length();
}

int ha_fastq::create_table_check(const char *name, TABLE *table_arg, HA_CREATE_INFO *create_info) {
	uint i;
	bool found_id = 0, found_quality = 0, found_sequence = 0;
	DBUG_ENTER("ha_fastq::create_table_check");

	/* check field names and types */
	for (i = 0; i < table_arg->s->fields; ++i) {
		Field *field = table_arg->field[i];
		uint type = field->type();
		if (strcmp(field->field_name, FASTQ_ID_FIELDNAME) == 0) {
			if (type != MYSQL_TYPE_VARCHAR)
				goto error_name_type_mismatch;
			found_id = 1;
		} else if (strcmp(field->field_name, FASTQ_QUALITY_FIELDNAME) == 0) {
			if (type != MYSQL_TYPE_VARCHAR && type != MYSQL_TYPE_BLOB)
				goto error_name_type_mismatch;
			found_quality = 1;
		} else if (strcmp(field->field_name, FASTQ_SEQUENCE_FIELDNAME) == 0) {
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

	if (!found_id || !found_sequence || !found_quality) {
		my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "missing 'id' and / or 'sequence' and / or 'quality' fields");
		DBUG_RETURN(HA_ERR_UNSUPPORTED);
	}

	DBUG_RETURN(0);
}

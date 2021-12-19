/*
 * editbuffer - editable buffer container with standard I/O semantics
 * Copyright (c) 2020-2021, Tommi Leino <namhas@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "editbuffer.h"

int
ebget(TxtBuffer *buffer)
{
	int ch;

	if (buffer->root != NULL)
		ch = (unsigned char) buffer->root->text[LOCAL_OFFSET(buffer)];
	else
		ch = EOF;

	ebseek(buffer, buffer->offset + 1);
	return ch;
}

/*
 * Gets a slice up to maximum length or up to encountering delim and
 * stores the result to s, up to its maximum length.
 *
 * Returns the length of the slice we got.
 *
 * XXX No matter how helpful this seems, not many practical
 *     caller side routines could benefit from this API.
 */
ssize_t
ebslice(TxtBuffer *buffer, char *delim, size_t slice, char *s, size_t len)
{
	int i;

	for (i = 0; i < slice && i < len; i++) {
		if (buffer->offset == buffer->len)
			break;

		if (buffer->root != NULL) {
			s[i] = buffer->root->text[LOCAL_OFFSET(buffer)];
			if (delim != NULL && strchr(delim, s[i]) != NULL) {
				ebseek(buffer, buffer->offset + 1);
				break;
			}
		} else
			break;	/* Should not happen */

		ebseek(buffer, buffer->offset + 1);
	}

	return i;
}

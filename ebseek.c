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

static void _findroot(TxtBlock **root, size_t *offset, size_t target);

int
ebseek(TxtBuffer *buffer, size_t target_offset)
{
	if (target_offset > buffer->len)
		target_offset = buffer->len;

	/*
	 * Special case handling for backtracking from a NULL block
	 * when going to a negative direction.
	 */
	if (target_offset < buffer->root_offset && buffer->root == NULL) {
		buffer->root = buffer->last;	/* can also be NULL */
		buffer->root_offset -= buffer->root->len;
	}

	_findroot(&buffer->root, &buffer->root_offset, target_offset);

	return (buffer->offset = target_offset);
}

static void
_findroot(TxtBlock **root, size_t *offset, size_t target)
{
	int local;

	while (*root != NULL) {
		local = target - *offset;
		if (local < 0) {
			if ((*root)->prev == NULL)
				break;	/* this is the first node */
			*root = (*root)->prev;
			if (*root != NULL)
				*offset -= (*root)->len;
		} else if (local >= (*root)->len && (*root)->len > 0 &&
		    1 /*(*root)->len == TXTBLOCK_MAXLEN*/
			/*
			 * last condition was disabled due to problems,
			 * however it solved some other problems
			 */) {
			*offset += (*root)->len;
			*root = (*root)->next;
		} else
			break;	/* success: block has the offset */
	}
}

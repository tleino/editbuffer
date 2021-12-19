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
#include <stdint.h>

static void _backtrack(TxtBuffer *buffer);
static TxtBlock* delempty(TxtBuffer *buffer, TxtBlock *block);

void
ebdel(TxtBuffer *buffer, size_t len)
{
	ssize_t begin, end;
	char *dst, *src;

	begin = buffer->offset;
	if (begin > buffer->len)
		begin = buffer->len;

	end = buffer->offset - len;

	ebseek(buffer, begin);
	_backtrack(buffer);

	while (begin != end) {
		/* 0123[cursor] = just decrement len */
		/* 01[cursor]23 = overwrite '1' by '23'  */
		/* [cursor]0123 = should be transformed to prev 0123[cursor] */
		if (buffer->root && LOCAL_OFFSET(buffer) == 0) {
			if (buffer->root->prev) {
				buffer->root->prev->len--;
				buffer->len--;
				begin--;
				buffer->offset--;
				buffer->root_offset--;
				buffer->root->prev =
				    delempty(buffer, buffer->root->prev);
			} else {
				break;
			}
		}
		else if (buffer->root && buffer->root->len) {
			if (begin - buffer->root_offset < buffer->root->len &&
			    LOCAL_OFFSET(buffer) > 0) {
				dst = &(buffer->root->text[LOCAL_OFFSET(buffer) - 1]);
				src = &(buffer->root->text[LOCAL_OFFSET(buffer)]);
				memmove(dst, src, 
				    (buffer->root->len - LOCAL_OFFSET(buffer)) *
				    sizeof(char));
			}
			buffer->root->len--;
			begin--;
			buffer->len--;
			buffer->offset--;

			if (buffer->root && buffer->root->len == 0) {
				buffer->root = delempty(buffer, buffer->root);
				if (buffer->root)
					buffer->root_offset -= buffer->root->len;
				else
					buffer->root_offset = 0;
			}
		}
	}
}

static TxtBlock*
delempty(TxtBuffer *buffer, TxtBlock *block)
{
	TxtBlock *tmp;

	if (block && block->len == 0) {
		tmp = block;

		if (tmp->prev)
			tmp->prev->next = tmp->next;
		if (tmp->next)
			tmp->next->prev = tmp->prev;

		if (tmp == buffer->last)
			buffer->last = tmp->prev;

		block = tmp->prev;	/* This can become new root */

		buffer->alloc -= (TXTBLOCK_ALLOC + sizeof(TxtBlock));
		buffer->blocks--;
		free(tmp);
		tmp = NULL;
	}

	return block;
}

/*
 * Backtrack if necessary in some cases.
 */
static void
_backtrack(TxtBuffer *buffer)
{
	if (buffer->root == NULL && buffer->last != NULL) {
		buffer->root_offset -= buffer->last->len;
		buffer->root = buffer->last;
	}
}

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

static void _backtrack_or_create_new(TxtBuffer *buffer);
static TxtBlock* _new(TxtBuffer *buffer, TxtBlock *parent);
static size_t _insert(TxtBuffer *buffer, TxtBlock *block, char *s, size_t len);
static void _split(TxtBuffer *buffer, TxtBlock *block);

void
ebput(TxtBuffer *buffer, char *s, size_t len)
{
	size_t i, n, offset;

	offset = buffer->offset;
	for (i = 0; i < len; i += n) {
		ebseek(buffer, offset + i);
		_backtrack_or_create_new(buffer);
		n = _insert(buffer, buffer->root, &s[i], len - i);
	}
}

/*
 * Backtrack if necessary in some cases.
 */
static void
_backtrack_or_create_new(TxtBuffer *buffer)
{
	if (buffer->root == NULL && buffer->last != NULL) {
		buffer->root_offset -= buffer->last->len;
		buffer->root = buffer->last;
	} else if (buffer->root == NULL) {
		buffer->root = _new(buffer, NULL);
	}
}

static TxtBlock*
_new(TxtBuffer *buffer, TxtBlock *parent)
{
	TxtBlock *block;
	static size_t blockno = 0;
	
	if ((block = calloc(1, sizeof(TxtBlock))) == NULL)
		goto alloc_err;

	block->blockno = ++blockno;

	block->prev = parent;
	if (parent != NULL) {
		block->next = parent->next;
		if (block->next)
			block->next->prev = block;
		parent->next = block;
	}

	if (block->next == NULL)
		buffer->last = block;

	block->len = 0;

	buffer->alloc += (TXTBLOCK_ALLOC + sizeof(TxtBlock));
	buffer->blocks++;

	if ((block->text = calloc(1, TXTBLOCK_ALLOC)) == NULL)
		goto alloc_err;

	return block;

alloc_err:
	err(1, "making space for new text");
}

static void
_split(TxtBuffer *buffer, TxtBlock *block)
{
	TxtBlock *new_block;
	char *src, *dst;

	new_block = _new(buffer, block);

	src = &(block->text[block->len / 2]);
	dst = &(new_block->text[0]);

	memcpy(dst, src, block->len / 2);

	new_block->len = TXTBLOCK_MAXLEN / 2;
	block->len /= 2;
}

static size_t
_insert(TxtBuffer *buffer, TxtBlock *block, char *s, size_t len)
{
	char *dst, *src;
	size_t loffset, space, clear, i;

	loffset = LOCAL_OFFSET(buffer);
	if (block->len == TXTBLOCK_MAXLEN && loffset < block->len) {
		_split(buffer, block);
		ebseek(buffer, buffer->offset);
		_backtrack_or_create_new(buffer);
		loffset = LOCAL_OFFSET(buffer);
	} else if (block->len == TXTBLOCK_MAXLEN) {
		_new(buffer, block);
		ebseek(buffer, buffer->offset);
		_backtrack_or_create_new(buffer);
		loffset = LOCAL_OFFSET(buffer);
	}
	block = buffer->root;

	space = (TXTBLOCK_MAXLEN - block->len);
	clear = len > space ? space : len;

	if (loffset < block->len) {
		dst = &(block->text[loffset + clear]);
		src = &(block->text[loffset]);
		memmove(dst, src, (block->len - loffset) * sizeof(char));
	}

	block->len += clear;
	buffer->len += clear;

	for (i = 0; i < clear; i++)
		block->text[loffset++] = s[i];

	return clear;
}

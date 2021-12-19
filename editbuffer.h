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

#ifndef EDITBUFFER_H
#define EDITBUFFER_H

/*
 * Example:
 *
 * TxtBuffer *eb;
 * int i;
 * 
 * ebput(&eb, "bar", 3);
 * ebseek(&eb, 1);
 * ebdel(&eb, 1);
 * ebseek(&eb, 0);
 * while (ebget(&eb) != EOF)
 *   == "br"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef struct txt_block TxtBlock;
typedef struct editbuffer TxtBuffer;

#if 1
#define TXTBLOCK_MAXLEN	(2048)	/* Needs to be dividable by 2 */
#else
#define TXTBLOCK_MAXLEN 4	/* Use this for testing */
#endif

#define TXTBLOCK_ALLOC	\
	(TXTBLOCK_MAXLEN) * sizeof(char)

struct editbuffer {
	size_t alloc;		/* Debug aid */
	size_t blocks;		/* Debug aid */
	size_t len;		/* Maximum offset */
	size_t root_offset;	/* Offset of node within the buffer */
	size_t offset;		/* Offset within the node */
	TxtBlock *root;		/* Current node */
	TxtBlock *last;		/* Used when root is NULL */
};

#define LOCAL_OFFSET(x)	((x)->offset - (x)->root_offset)

/* XXX: We could perhaps have ebtell(x) (x)->offset !!! :) */

#if 0
#define ebtell(x) ((x)->root_offset + LOCAL_OFFSET(x))
#else
#define ebtell(x) (x)->offset
#endif

struct txt_block {
	size_t blockno;		/* Debug aid */
	size_t len;		/* Finding and splitting */
	TxtBlock *prev, *next;	/* Insertions in the middle */
	char *text;		/* Preallocated and not grown */
};

int     ebseek(TxtBuffer *buffer, size_t offset);
int     ebget (TxtBuffer *buffer);
void    ebput (TxtBuffer *buffer, char *s, size_t len);
void	ebdel (TxtBuffer *buffer, size_t len);
void    ebdump(TxtBuffer *buffer);

#if 0
size_t  ebtell(TxtBuffer *);
#endif

int     ebfind(TxtBuffer *b, char c, int i, int incr);

/* Helpers */
int     ebfindprev(TxtBuffer *eb, int cursor);
int     ebfindnext(TxtBuffer *eb, int cursor);
int     ebfindbol(TxtBuffer *eb, int cursor);
int     ebfindeol(TxtBuffer *eb, int cursor);
int     ebfindcol(TxtBuffer *b, int i);
int     ebfindxy(TxtBuffer *b, int x, int y, int pos);
void    ebfindxydelta(TxtBuffer *b, int p1, int p2, int *x, int *y);

char *ebwordat(TxtBuffer *b, int i);

int     ebscroll(TxtBuffer *b, int pos, int n);

ssize_t ebslice(TxtBuffer *, char *, size_t, char *, size_t);

/*
ssize_t ebread(TxtBuffer *buffer, char *s, size_t len);
*/

/* ucs2.c */
int     editbuffer_get_ucs2   (struct editbuffer *);
int     editbuffer_del_ucs2   (struct editbuffer *, ssize_t);
int     editbuffer_seek_ucs2  (struct editbuffer *, ssize_t);

#endif

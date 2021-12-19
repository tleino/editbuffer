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

/*
 * Find character (c) from cursor index (i) onwards in increment (incr) steps,
 * until EOF or BOF from buffer (b). Returns the cursor index of the found
 * character or cursor index of EOF / BOF.
 */
int
ebfind(TxtBuffer *b, char c, int i, int incr)
{
	char ch;

	for (;;) {
		if (i < 0 || i > b->len)
			break;
		ebseek(b, i);
		ch = ebget(b);
		if (ch == EOF && incr > 0)
			break;
		if (i < 0)
			break;
		if (c == ch) {
			break;
		}
		i += incr;
	}

	return i;
}

/*
 * Returns the beginning of next line from offset (i) in buffer (b).
 */
int
ebfindnext(TxtBuffer *b, int i)
{
	if ((i = ebfind(b, '\n', i, 1)) == b->len)
		return b->len;
	return ++i;
}

/*
 * Returns the beginning of previous line from offset (i) in buffer (b).
 */
int
ebfindprev(TxtBuffer *b, int i)
{
	if (i > 0)
		i--;
	i = ebfind(b, '\n', i, -1);
	if (i > 0)
		i--;

	return ebfindbol(b, i);
}

/*
 * Returns the end of line offset ("EOL") from offset (i) in buffer (b).
 */
int
ebfindeol(TxtBuffer *b, int i)
{
	return ebfind(b, '\n', i, 1);
}

/*
 * Returns the beginning of line offset ("BOL") from offset (i) in buffer (b).
 *
 * Special case handling for the beginning of buffer: we'd simply return
 * the beginning of buffer offset as is which is always 0.
 */
int
ebfindbol(TxtBuffer *b, int i)
{
	if ((i = ebfind(b, '\n', i, -1)) == 0)
		return 0;
	return ++i;
}

int
eblinelen(TxtBuffer *b, int i)
{
	int eol, bol;

	bol = ebfindbol(b, i);
	eol = ebfindeol(b, i);
	if (bol > eol)	/* We're at EOL */
		bol = ebfindbol(b, i - 1);

	return (eol - bol);
}

/*
 * Returns col within a line.
 */
int
ebfindcol(TxtBuffer *b, int i)
{
	int col;

	col = (i - ebfindbol(b, i));
	if (col < 0)	/* We're at EOL */
		return eblinelen(b, i);

	return col;
}

/*
 * Returns cursor for x/y from pos
 */
int
ebfindxy(TxtBuffer *b, int x, int y, int pos)
{
	while (y > 0 && y--)
		pos = ebfindnext(b, pos);
	return (pos + x);
}

/*
 * Returns x/y delta from index p1 to index p2.
 */
void
ebfindxydelta(TxtBuffer *b, int p1, int p2, int *x, int *y)
{
	int tmp;

	*x = *y = 0;

	while (p1 < p2 && p1 < b->len) {
		tmp = ebfindnext(b, p1);
		if (tmp - 1 < p2) {
			p1 = tmp;
			*y = *y + 1;
		} else
			break;
	}
	if (p1 < b->len)
		p1 = ebfindbol(b, p1);

	*x = ebfindcol(b, p2);
}

char *
ebwordat(TxtBuffer *b, int i)
{
	static char word[256 + 1];
	int begin, end, ch;

	begin = ebfind(b, ' ', i, -1);
	end = ebfind(b, ' ', i, 1);

	ebseek(b, begin + 1);

	/* Normalize */
	if (end != b->len && end != 0)
		end--;
	end = end - begin;
	begin = 0;
	while (begin < end) {
		if (begin == sizeof(word) - 1)
			break;
		ch = ebget(b);
		if (ch == EOF)
			break;
		word[begin] = (char) ch;
		begin++;
	}
	word[begin] = '\0';

	return word;
}

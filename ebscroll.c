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
 * Returns cursor index after scrolling buffer a number of lines (n)
 * from source cursor index (pos).
 */
int
ebscroll(TxtBuffer *b, int pos, int n)
{
	if (n > 0)
		while (n--)
			pos = ebfindnext(b, pos);
	else
		while (n++ < 0)
			pos = ebfindprev(b, pos);

	return pos;
}

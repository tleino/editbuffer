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

/*
 * UCS-2 support is useful for interacting with some legacy base API,
 * e.g. the core X11 API that does not have a native support for UTF-8
 * but has support for UCS-2. For most "hacker" users, however, UCS-2
 * is good enough, just like US-ASCII is good enough.
 */

/*
 * TODO: We could rename this to utf8 and handle truncation to ucs2
 * separately.
 */

#include "editbuffer.h"

#ifndef WANT_WARN
#define WANT_WARN 1
#endif

#ifdef WANT_WARN
#include <err.h>
#endif

int
editbuffer_get_ucs2(struct editbuffer *b)
{
	int ch, nbytes, cursor, u;

	cursor = ebtell(b);
	/* We iterate maximum of 4 bytes */
	for (nbytes = 3; nbytes >= 0; nbytes--) {
		ch = ebget(b);

		if (ch == EOF && nbytes == 3) {
			/*
			 * We're on EOF. If we were parsing UTF-8
			 * and got this EOF prematurely, the logic
			 * falls on the error handler because all
			 * ch ranges here expect ch >= 0 while EOF
			 * is < 0.
			 */
			nbytes = 0;
			u = EOF;
		} else if (ch >= 0xC2 && ch < 0xF5 && nbytes == 3) {
			/*
			 * We're on first byte of multibyte UTF-8.
			 */
			if (ch >= 0xF0) {
				u = (ch & 0b00000111) << (6 * nbytes);
			} else if (ch >= 0xE0) {
				nbytes = 2;
				u = (ch & 0b00001111) << (6 * nbytes);
			} else {
				nbytes = 1;
				u = (ch & 0b00011111) << (6 * nbytes);
			}
		} else if (ch < 0x80 && ch >= 0 && nbytes == 3) {
			/*
			 * We're on ASCII.
			 */
			nbytes = 0;
			u = ch;
		} else if (ch >= 0x80 && ch < 0xC0) {
			/*
			 * We're on continuation.
			 */
			u |= ((ch & 0b00111111) << (6 * nbytes));
		} else {
			/*
			 * We're on error. Rewind back, advance one
			 * byte.
			 */
			nbytes = 0;
			u = 0xFFFD;
			ebseek(b, cursor + 1);
		}
	}

	return u;
}

#if 0
int
editbuffer_del_ucs2(struct editbuffer *b, ssize_t n)
{

}
#endif

static int ucs2_seek_decr(struct editbuffer *, ssize_t);
static int ucs2_seek_incr(struct editbuffer *, ssize_t);

/*
 * Changes the given editbuffer index by xchar2b units to either
 * direction.
 *
 * Returns the new index in editbuffer units.
 */
int
editbuffer_seek_ucs2(struct editbuffer *b, ssize_t n)
{
	if (n < 0)
		return ucs2_seek_decr(b, n * -1);
	else
		return ucs2_seek_incr(b, n);
}

/*
 * Decreases index in a UTF-8 encoded buffer one UTF-8 byte sequence
 * at a time.
 *
 * Returns the new cursor position.
 */
static int
ucs2_seek_decr(struct editbuffer *b, ssize_t n)
{
	int cursor, begin, ch, i;

	cursor = ebtell(b);
	while (n-- && cursor > 0) {
		begin = ebtell(b);
		ebseek(b, --cursor);
		ch = ebget(b);
		/*
		 * We cannot actually begin from a start byte when
		 * we're travelling backwards. Count as one character.
		 */
		if (ch >= 0xC0)
			continue;
		/*
		 * We continue decreasing until we're on first byte
		 * in the multibyte sequence which means we're not
		 * on a UTF-8 continuation byte. However, we honor
		 * the maximum of 3 continuation bytes in UTF-8 spec.
		 *
		 * Continue bytes: >= 0b10xxxxxx (0x80).
		 * Start bytes:    >= 0b110xxxxx (0xC0).
		 */
		i = 3;
		while (i-- && ch >= 0x80 && ch < 0xC0 && cursor > 0) {
			ebseek(b, --cursor);
			ch = ebget(b);
		}

		/*
		 * Check if forward parsing does not match with our
		 * reverse iteration, if so, trust the forward parsing,
		 * but only do this in case we actually parsed some
		 * UTF-8.
		 */
		if (i != 3) {
			ebseek(b, cursor);
			editbuffer_get_ucs2(b);
			if (ebtell(b) != begin)
				cursor = ebtell(b);
		}
	}

	return ebseek(b, cursor);	/* Undo effect of get */
}

/*
 * Increases index in a UTF-8 encoded buffer one UCS2 code at a time.
 *
 * Returns the new cursor position.
 */
static int
ucs2_seek_incr(struct editbuffer *b, ssize_t n)
{
	while (n-- && editbuffer_get_ucs2(b) != EOF)
		;

	return ebtell(b);
}

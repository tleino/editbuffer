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

void
ebdump(TxtBuffer *buffer)
{
	int i;
	char ch;
	size_t offset, o;
	int invbg = 0;
	TxtBlock *first, *np;

	offset = buffer->offset;

	printf("buffer->len: %zu\n", buffer->len);
	printf("buffer->offset: %zu\n", buffer->offset);
	printf("buffer->root_offset: %zu\n", buffer->root_offset);
	printf("buffer->alloc: %zu\n", buffer->alloc);
	printf("buffer->blocks: %zu\n", buffer->blocks);

	ebseek(buffer, 0);

	first = buffer->root;
	np = first;
	o = 0;
	while (np) {
		printf("!! %lx blockno=%zu offset=%zu len=%zu util=%.2f%%\n",
		    (intptr_t) np, np->blockno, o, np->len,
		    ((double) np->len / (double) TXTBLOCK_MAXLEN * 100.0));
		o += np->len;
		np = np->next;
	}
	
	for (i = 0; i < buffer->len; i++) {
		if (i != 0 && i % 24 == 0)
			putchar('\n');
		printf("%2d ", i);
	}
	putchar('\n');

#if 1
	for (i = 0; i < buffer->len; i++) {
		if (i != 0 && i % 24 == 0)
			putchar('\n');

		ch = ebget(buffer);
		if (ch == 0) {
			printf("at offset %d got NULL ch\n", i);
			printf("- why? buffer->root %lx\n",
			    (intptr_t) buffer->root);
			printf("- buffer->root_offset %zu\n",
			    buffer->root_offset);
		}

		if (i - buffer->root_offset == 0) {
			invbg++;
			invbg %= 2;
		}

		if (invbg)
			printf("\033[7m");
		else
			printf("\033[0m");

		if (i == offset)
			putchar('#');
		else
			putchar(' ');

		if (ch == ' ')
			putchar('_');
		else
			putchar(ch);
		
		if (i == offset)
			putchar('#');
		else
			putchar(' ');

		printf("\033[0m");
	}
	putchar('\n');
#else
	for (i = 0; i < buffer->len; i += sizeof(s)) {
		n = ebread(buffer, s, sizeof(s));
		fwrite(s, 1, n, stdout);
	}
#endif
	putchar('\n');
}

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
 * This file shows example how to use editbuffer / provides tests for
 * the algorithm.
 */

#include "editbuffer.h"

int
main(int argc, char *argv[])
{
	static TxtBuffer buffer;

	char *hello = "HelLo world!";
	char *what = "[ WHAT YOU DOING? ]";
	char *nothing = " Doing nothing. Move along.";

	ebput(&buffer, hello, strlen(hello));
	printf("buffer: %zu %zu\n", buffer.offset, ebtell(&buffer));
	ebdump(&buffer);

	ebseek(&buffer, 5);
	printf("buffer: %zu %zu\n", buffer.offset, ebtell(&buffer));

	ebdel(&buffer, 3);
	printf("buffer: %zu %zu\n", buffer.offset, ebtell(&buffer));
	ebdump(&buffer);

	ebdel(&buffer, 1);
	printf("buffer: %zu %zu\n", buffer.offset, ebtell(&buffer));
	ebdump(&buffer);

	ebput(&buffer, what, strlen(what));
	printf("buffer: %zu %zu\n", buffer.offset, ebtell(&buffer));
	ebdump(&buffer);

	ebseek(&buffer, 2);
	printf("buffer: %zu %zu\n", buffer.offset, ebtell(&buffer));
	ebput(&buffer, nothing, strlen(nothing));
	printf("buffer: %zu %zu\n", buffer.offset, ebtell(&buffer));

	ebdump(&buffer);

	ebseek(&buffer, 43434343);
	printf("buffer: %zu %zu\n", buffer.offset, ebtell(&buffer));

	ebput(&buffer, hello, strlen(hello));
	ebseek(&buffer, buffer.len);
	ebdump(&buffer);


	printf("GOING to 10 and performing 3 BS\n");
	ebseek(&buffer, 10);
	ebdump(&buffer);
	ebseek(&buffer, 10);

	ebdel(&buffer, 2);
	ebdump(&buffer);

	printf("ONE MORE, now from 8, 4 BS (one block)\n");
	ebseek(&buffer, 8);
	ebdump(&buffer);
	ebseek(&buffer, 8);
	ebdel(&buffer, 4);

	ebseek(&buffer, 4);
	ebdel(&buffer, 4);	/* Should leave just 'H' */

	ebdump(&buffer);

	ebseek(&buffer, 1);
	ebdel(&buffer, 1);

	ebdump(&buffer);

	ebseek(&buffer, 1);
	ebdel(&buffer, 1);

	ebdump(&buffer);

	ebseek(&buffer, 0);
	ebput(&buffer, "Hello!", 6);
	ebdump(&buffer);

	ebseek(&buffer, buffer.len);
	ebdel(&buffer, buffer.len);	/* XXX Unlimited del still fails */

	ebput(&buffer, hello, strlen(hello));
	ebseek(&buffer, 3);

	ebput(&buffer, what, strlen(what));
	ebseek(&buffer, buffer.len);
	ebput(&buffer, nothing, strlen(nothing));
	ebdump(&buffer);

	ebseek(&buffer, 10);
	ebput(&buffer, "FOOBAR", 6);

	ebdump(&buffer);
	return 0;
}

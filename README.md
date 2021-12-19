# editbuffer

*editbuffer* is an embeddable C module / library providing editable
buffer container with *standard I/O semantics* which simplifies
cursor handling as the cursor is simply a seek position. It also
gives *high performance* and *low memory overhead* because of block
allocation strategy.

Originally written for [cocovt](https://github.com/tleino/cocovt)
and [vtsh](https://github.com/tleino/vtsh), but can be used in any
programs that require a buffer container with simple API that has
text editor-like functionality i.e. supports adding and removing
random-sized chunks with random access pattern.

Normally the naive approach for text editor buffer container requires
keeping track of two pointers: row and col, but here only one pointer
is needed, the *seek position*, just like in the standard I/O API,
yet the performance is greatly increased over a naive dynamic array
implementation.

## Dependencies

None.

Should work in many Unix-like environments. Developed on a OpenBSD
base system without dependencies. Compiling for other systems might
require installing some standard programs such as C compiler.

## Configure

Edit editbuffer.c and edit variables to taste:

* TXTBLOCK_MAXLEN

## Example

	TxtBuffer *eb;
	int i;
	char ch;
	
	ebput(&eb, "bar", 3);
	ebseek(&eb, 1);
	ebdel(&eb, 1);
	ebseek(&eb, 0);
	while ((ch = ebget(&eb)) != EOF)
		putchar(ch);

## Compile

	make

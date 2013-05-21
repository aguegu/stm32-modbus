#include <sys/types.h>

extern const int _end;

caddr_t _sbrk(int incr) {
	static unsigned char *heap = NULL;
	unsigned char *prev_heap;

	if (heap == NULL ) {
		heap = (unsigned char *) &_end;
	}
	prev_heap = heap;
	/* check removed to show basic approach */

	heap += incr;

	return (caddr_t) prev_heap;
}


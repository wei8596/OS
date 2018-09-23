/**
*memory management header file
*/
#ifndef MM_H
#define MM_H
#include <stdlib.h>
#include <string.h>
/*  minimum #units to request  */
#define NALLOC 1024

typedef long Align;  /*  for alignment to long boundary  */

union header {  /*  block header:  */
    struct {
        union header *ptr;  /*  next block if on free list  */
        unsigned size;  /*  size of this blcok  */
    } s;
    Align x;  /*  force alignment of blocks  */
};

typedef union header Header;

/*  mymalloc: general-purpose storage allocator  */
void *mymalloc(unsigned nbytes);
/*  morecore: ask system for more memory  */
Header *morecore(unsigned nu);
/*  myfree: put block ap in free list  */
void myfree(void *ap);
void *mycalloc(unsigned n, unsigned size);
void *myrealloc(void *ptr, unsigned size);

#endif


#include "brk.h"
#include <unistd.h>
#include <string.h> 
#include <errno.h> 
#include <sys/mman.h>
#include <stdio.h>

#ifndef STRATEGY
#define STRATEGY 1
#endif 

#define STRATEGY_FIRST 1
#define STRATEGY_BEST 2



#if STRATEGY < 1 || STRATEGY > 2
#error STRATEGY must contain a value over 1 and below 2
#endif





#define _GNU_SOURCE
#define NALLOC 1024    




                                /* minimum #units to request */

typedef long Align;                                     /* for alignment to long boundary */

union header {                                          /* block header */
  struct {
    union header *ptr;                                  /* next block if on free list */
    unsigned size;                                      /* size of this block  - what unit? */ 
  } s;
  Align x;                                              /* force alignment of blocks */
};

typedef union header Header;

static Header base;                                     /* empty list to get started */
static Header *freep = NULL;                            /* start of free list */

/* free: put block ap in the free list */

void free(void * ap)
{
  Header *bp, *p;

  if(ap == NULL) return;                                /* Nothing to do */

  bp = (Header *) ap - 1;                               /* point to block header */
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;                                            /* freed block at atrt or end of arena */

  if(bp + bp->s.size == p->s.ptr) {                     /* join to upper nb */
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  }
  else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp) {                             /* join to lower nbr */
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

/* morecore: ask system for more memory */

#ifdef MMAP

static void * __endHeap = 0;

void * endHeap(void)
{
  if(__endHeap == 0) __endHeap = sbrk(0);
  return __endHeap;
}
#endif


static Header *morecore(unsigned nu)
{
  void *cp;
  Header *up;
#ifdef MMAP
  unsigned noPages;
  if(__endHeap == 0) __endHeap = sbrk(0);
#endif

  if(nu < NALLOC)
    nu = NALLOC;
#ifdef MMAP
  noPages = ((nu*sizeof(Header))-1)/getpagesize() + 1;
  cp = mmap(__endHeap, noPages*getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  nu = (noPages*getpagesize())/sizeof(Header);
  __endHeap += noPages*getpagesize();
#else
  cp = sbrk(nu*sizeof(Header));
#endif
  if(cp == (void *) -1){                                 /* no space at all */
    perror("failed to get more memory");
    return NULL;
  }
  up = (Header *) cp;
  up->s.size = nu;
  free((void *)(up+1));
  return freep;
}

void * malloc(size_t nbytes){

  Header *p, *prevp;
  Header * morecore(unsigned);
  unsigned nunits;

  if(nbytes == 0) return NULL;

  nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) +1; /*Calculate units of memory to allocate*/

  if((prevp = freep) == NULL) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }

  
  /*Implements strategy first fit*/

  if(STRATEGY == STRATEGY_FIRST){
    
    for(p= prevp->s.ptr;  ; prevp = p, p = p->s.ptr) {
      if(p->s.size >= nunits) {                           /* big enough */
        if (p->s.size == nunits)                          /* exactly */
	        prevp->s.ptr = p->s.ptr;
        else {                                            /* allocate tail end */
	        p->s.size -= nunits;
	        p += p->s.size;
	        p->s.size = nunits;
        }
        freep = prevp;
        return (void *)(p+1);                             /*Return the block that fits first*/
      }
      if(p == freep){                                    /* wrapped around free list */
        if((p = morecore(nunits)) == NULL)
	       return NULL;
         }                                    /* none left *t*/
    }
  }

  /*Implements strategy best fit*/
  else if (STRATEGY == STRATEGY_BEST){
    Header * best, * prevbest;

     for(p= prevp->s.ptr;  ; prevp = p, p = p->s.ptr) {

        if (p->s.size == nunits)  { /* We have a perfect fit*/
          best = p;
          freep = prevp;
          break; /* Break the loop becaúse a best fit has been found*/
        }

        else if(p->s.size > nunits){ /* We have a fit but not a perfect fit*/
          if (best == 0){/* No previous best fit*/
              best = p;
              prevbest = prevp;
          } 
          else if(best->s.size > p->s.size) { /* The size of the new block is a fit but smaller than the current best fit*/
              best = p;
              prevbest = prevp;
          }
          
        }
        if(p == freep) { /* Looped through the free list space */
          if(best == NULL) { /* No best fit*/
            if((p = morecore(nunits)) == NULL) /* Call morecore to try allocate more memory */
                return NULL;
          }

          else {/*We had best match!!!*/
            best->s.size -= nunits;/*Remove space from block*/
            best += best->s.size;/*Make the best fit a header */
            best->s.size = nunits; /* Set the size of this block to the requested amount*/
            freep = prevp;
            break;
          } 
        }

     }

     return (void *)(best+1);


  }
}

void *realloc(void * ptr, size_t size){

    Header *newp; /* the new pointer*/
    size_t currSize;


    if (ptr == NULL){
      return malloc(size);
    }
    else if (size == 0){
      free(ptr);
      return NULL;
    }

    newp = ((Header *)ptr) - 1; 
    currSize = (newp->s.size-1) * sizeof(Header);

    if (currSize > size){
      currSize = size;
    }
    
    newp = malloc(size); /*ALlocate new space*/
    if(newp == NULL)
      return NULL;
    
    memcpy(newp, ptr, currSize); /* Copy the old pointer to new pointer */
    free(ptr); /* Free the old pointer*/

    return newp;

}
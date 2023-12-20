// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
  uint num_free_pages; //number of free pages
  uint pgrefcount[PHYSTOP >> PGSHIFT]; //reference counter for physical memory pages
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  kmem.num_free_pages = 0; //initialize number of free page as 0
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE){
    kmem.pgrefcount[V2P(p) >> PGSHIFT] = 0; //initailize pgrefcount
    kfree(p);
  }
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);

  //add for cow
  if(get_refcount(V2P(v)) > 0)
    dec_refcount(V2P(v));  //decrease refcount after free

  if(get_refcount(V2P(v)) == 0) 
  {
    memset(v, 1, PGSIZE);
    kmem.num_free_pages++; //++
    
    r = (struct run*)v;
    r->next = kmem.freelist;
    kmem.freelist = r;
  }
  //end

  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);

  kmem.num_free_pages--; //decrease free page number in allocation page
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    kmem.pgrefcount[V2P((char*)r) >> PGSHIFT] = 1;//set pgrefcount to 1 when allocating 
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

uint numFreePages(void)
{
  acquire(&kmem.lock);
  uint fp = kmem.num_free_pages;
  release(&kmem.lock);
  return fp;
}

uint get_refcount(uint pa){
  return kmem.pgrefcount[pa >> PGSHIFT];
}

void inc_refcount(uint pa){
  ++kmem.pgrefcount[pa >> PGSHIFT];
}

void dec_refcount(uint pa){
  --kmem.pgrefcount[pa >> PGSHIFT];
}

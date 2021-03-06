#ifndef _BUF_H_
#define _BUF_H_

#ifndef __ASSEMBLER__

#include <include/lock.h>
#include <include/types.h>
#include <include/kernel.h>

#define HASHSLOT  7

#define B_BUSY  0x1
#define B_VALID	0x2
#define B_DIRTY	0x4

struct dozenbufs;

struct buf {
	char *data;
	int32_t  flag;
	int32_t refcnt;
	uint32_t dev;
	uint32_t blockno;
	struct dozenbufs *dozen_ptr;
	struct list_head  hash_node;
	struct list_head  free_list_node;
	struct list_head  ide_queue_node;
	struct list_head  waiting_for_io;
	struct list_head  waiting_proc_list;
};

struct dozenbufs {
	int n_bufs;
	int n_request;
	struct buf **buf_array;
	struct list_head waiting_for_io;
};

// a hash table and free_list.
struct blk_cache {
	struct spinlock  blk_cache_lk;
	struct list_head  free_list_head;
	struct list_head  waiting_proc_list;
	struct list_head  hash_table[HASHSLOT];
};

static struct buf *find_blk_in_hash(uint32_t dev, uint32_t blockno);
static void put_blk_in_hash(struct buf *b);
// Initialize block cache
void buffer_init(void);
struct dozenbufs *dozens_init(int nblks);
// read blcok from disk to memory
struct buf *getblk(uint32_t dev, uint32_t blockno);
// High level functions call it to get the contents of the indicated block.
struct buf *bread(uint32_t dev, uint32_t blockno);
// High level functions call it to write modified block back to disk
void bwrite(struct buf *b);
void bwrite_delay(struct buf *b);
void bwrite_dozens(struct dozenbufs *dozens);
// Release a locked buffer
void brelse(struct buf *b);
void brelse_dozens(struct dozenbufs *dozens);
void bzero(uint32_t dev, uint32_t blkno);
uint32_t balloc(uint32_t dev);
int bfree(uint32_t dev, uint32_t blkno);


#endif

#endif

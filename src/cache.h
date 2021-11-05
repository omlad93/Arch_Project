#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "mesi.h"

#define WORDS 256
#define BLOCKS 64
#define BLK_SIZE 4
//#define MEM_MASK 0x000FFFFF // 20 bits mask
#define TAG_MASK 0x000FFF00 // 12 bits ( 0 : 11) 
#define IDX_MASK 0x000000FC // 6 bits  (12 : 17)
//#define OST_MASK 0x00000003 // 2 bits  (18 : 19)

#define _get_tag(address) (address & TAG_MASK) // get tag of address
#define _get_idx(address) (address & IDX_MASK) // get cachline from address
#define alligned(address) (address / BLK_SIZE) // get first address in the block

typedef struct cahce{
    int cache_data[WORDS];      // the actual data stored by cachelines (word)
    int tags[BLOCKS];           // the tags of the stored blocks 
    int mesi_state[BLOCKS];     // the mesi status of the stored blocks
} cache;

int hit(int address, cache* cache);

int read_word(int address, int* ack, cache* cache);

int write_word(int address, int* ack, cache* cache);

int fetch_block(int alligned_address); //request a block when a miss occur


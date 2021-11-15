#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "mesi.h"

#define CACHE_COUNT 4
#define HIT 1
#define MISS -1
#define WORDS 256
#define BLOCKS 64
#define BLK_SIZE 4

//#define MEM_MASK 0x000FFFFF // 20 bits mask
#define TAG_MASK 0x000FFF00 // 12 bits ( 0 : 11) 
#define IDX_MASK 0x000000FC // 6 bits  (12 : git p17)
//#define OST_MASK 0x00000003 // 2 bits  (18 : 19)

#define _get_tag(address) (address & TAG_MASK) // get tag of address
#define _get_idx(address) (address & IDX_MASK) // get cachline from address
#define alligned(address) (address / BLK_SIZE) // get first address in the block


static int idx =0; //static idx for cache id
static cache* CACHES[CACHE_COUNT]; // array of caches


typedef struct bus_request{
    int cmd;
    int addr;
    int data;
} bus_request;

typedef struct cahce{
    int cache_data[WORDS];      // the actual data stored by cachelines (word)
    int tags[BLOCKS];           // the tags of the stored blocks 
    int mesi_state[BLOCKS];     // the mesi status of the stored blocks
    int idx;
    int busy;
    bus_request* next_req;
} cache;


void init_cache(cache* cache){
    cache->idx = idx;       // create unique idx
    CACHES[idx] = cache;    // add to array
    idx++;                  // inc idx
    cache-> ack = 0;

    for (int i=0; i<WORDS; i++){    /// zero all data
        cache->cache_data[i] = 0;
        if (i < BLOCKS){
            cache->tags[i] = 0;
            cache->mesi_state[i] = 0;
        }
    }

    cache ->next_req = {0};
}

// check if address is cached, return HIT or MISS
int query(int address, cache* cache);

// read word from cache. if MISS, fetched it throug messi and stall
int read_word(int address, cache* cache);

// write data to cache. if MISS, fetched it throug messi and stall
int write_word(int address, int data, cache* cache);

// for debugging loading cache not through file
// fills only cahe_data without mesi & tags
// allows to make sure read and writes are ok before MESI
void fill_cache_manually(cache* cache);
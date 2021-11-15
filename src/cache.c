#include "cache.h"

// check if address is cached, return HIT or MISS
int query(int address, cache* cache, int mode){
    int block_tag = _get_tag(alligned(address));
    int block_idx = _get_idx(address);
    int valid;
    int stored = (block_tag == cache->tags[block_idx]);
    if (mode == BusRd) valid = cache->mesi_state[block_idx] != Invalid; // for read, except all modes exept invalid
    else if (mode == BusRdX) valid = cache->mesi_state[block_idx] == Exclusive; // for write, accept only exclusive
    return (stored & valid) ? HIT : MISS;
}

// read word from cache. if MISS, fetched it throug messi and stall
int read_word(int address, cache* cache){
    if (query(address,cache,BusRd) == HIT){
        return cache->cache_data[address];
    }
    else if(cache->busy = 0) {
        // generate a mesi transaction
        cache -> busy = 1;
        cache -> next_req -> cmd = BusRd;
        cache -> next_req -> addr = address;
        cache -> next_req -> data = 0;
        requests[cache->idx] = cache->next_req;
        return NULL;
    } else if (cache -> busy = 1){
        // do nothing, requrst is on mesi.
        return NULL;
    }
}

// write data to cache. if MISS, fetched it throug messi and stall
int write_word(int address, int data, int* ack, cache* cache){
    if (query(address,cache, BusRdX) == HIT){
        cache->cache_data[address] = data;
        return HIT; 
    }
    else if(cache->busy = 0) {
        // generate a mesi transaction
        cache -> busy = 1;
        cache -> next_req -> cmd = BusRdX;
        cache -> next_req -> addr = address;
        cache -> next_req -> data = data;
        generate_transaction(cache);
        return NULL;
    } else if (cache -> busy = 1){
        // do nothing, requrst is on mesi.
        return NULL;
    }
}


// for debugging loadinf cache not through file
// fills only cahe_data without mesi & tags
// allows to make sure read and writes are ok before MESI
void fill_cache_manually(cache* cache){
    for (int i=0 ; i<WORDS ; i++){
        cache->cache_data[i] = i;
    }
}
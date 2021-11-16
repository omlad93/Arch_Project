#include "cache.h"

// check if address is cached, return HIT or MISS
// for BusRd - if data is valid it's enough
// for BusRdX - data has to be in M state
int query(int address, cache* cache, int mode){
    int block_tag = _get_tag(alligned(address));
    int block_idx = _get_block(address);
    int valid;
    int stored = (block_tag == cache->tags[block_idx]);
    if (mode == BusRd) valid = cache->mesi_state[block_idx] != Invalid; // for read, except all modes exept invalid
    else if (mode == BusRdX) valid = cache->mesi_state[block_idx] == Exclusive; // for write, accept only exclusive
    return (stored & valid) ? HIT : MISS;
}

// read word from cache. if MISS, fetched it throug messi and stall
int read_word(int address, cache* cache){
    if (query(address,cache,BusRd) == HIT){
        return cache->cache_data[_get_idx(address)];
    }
    else if(cache->busy = 0) {
        // generate a mesi transaction
        cache -> busy = 1;                      // ignore same calls
        cache -> next_req -> cmd = BusRd;       // Read request
        cache -> next_req -> addr = address;    // address which read is needed
        cache -> next_req -> data = 0;          // no data for read
        requests_queue[cache->idx] = cache->next_req; // load request to mesi pool
        return NULL;
    } else if (cache -> busy = 1){
        // do nothing, requrst is on mesi.
        return NULL;
    }
}

// write data to cache. if MISS, fetched it throug messi and stall
int write_word(int address, int data, int* ack, cache* cache){
    if (query(address,cache, BusRdX) == HIT){
        cache->cache_data[_get_idx(address)] = data;
        cache->mesi_state[_get_block(address)] = Modified;
        return HIT; 
    }
    else if(cache->busy = 0) {
        // generate a mesi transaction
        cache -> busy = 1;
        cache -> next_req -> cmd = BusRdX;
        cache -> next_req -> addr = address;
        cache -> next_req -> data = data;
        return NULL;
    } else if (cache -> busy = 1){
        // do nothing, requrst is on mesi.
        return NULL;
    }
}


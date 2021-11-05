#include "cache.h"

int query(int address, cache* cache){
    int block_tag = _get_tag(alligned(address));
    int block_idx = _get_idx(address);
    int valid = cache->mesi_state[block_idx] != Invalid;
    return ((block_tag == cache->tags[block_idx])&valid) ? HIT : MISS;
}

int read_word(int address, int* ack, cache* cache){
    if (query(address,cache) == HIT){
        *ack = 1;
        return cache->cache_data[address];
    }
    else{
        *ack = 0;
        fetch_block(alligned(address), cache);
        return NULL;
    }
}

int write_word(int address, int data, int* ack, cache* cache){
    if (query(address,cache) == HIT){
        *ack = 1;
        cache->cache_data[address] = data;
        // update MESI
    }
    else{
        *ack = 0;
        fetch_block(alligned(address), cache);
        return NULL;
    }
}

int fetch_block(int alligned_address, cache* cache); //request a block when a miss occur
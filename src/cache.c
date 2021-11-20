#include "memory.h"

// call when on an instace upon creation.
void init_cache(cache_p cache){
    cache->idx = idx;       // create unique idx
    CACHES[idx] = cache;    // add to array
    idx++;                  // inc idx
    cache-> busy = 0;

    cache->next_req = NULL;
    for (int i=0; i<WORDS; i++){    /// zero all data
        cache->cache_data[i] = 0;
        if (i < BLOCKS){
            cache->tags[i] = 0;
            cache->mesi_state[i] = 0;
        }
    }


}

// check if address is cached, return HIT or MISS
// for BusRd - if data is valid it's enough
// for BusRdX - data has to be in M state
int query(int address, cache_p cache, int mode){
    int block_tag = _get_tag(alligned(address));
    int block_idx = _get_block(address);
    int valid;
    int stored = (block_tag == cache->tags[block_idx]);
    if (mode == BusRd) valid = cache->mesi_state[block_idx] != Invalid; // for read, except all modes exept invalid
    else if (mode == BusRdX) valid = cache->mesi_state[block_idx] == Exclusive; // for write, accept only exclusive
    return (stored & valid) ? HIT : MISS;
}

// read word from cache. if MISS, fetched it throug messi and stall
int read_word(int address, cache_p cache, int* dest_reg){
    if (query(address,cache,BusRd) == HIT){
        *dest_reg = cache->cache_data[_get_idx(address)];
        return HIT;
    }
    else if(cache->busy = 0) {
        // generate a mesi transaction
        cache -> busy = 1;                      // ignore same calls
        cache -> next_req -> cmd = BusRd;       // Read request
        cache -> next_req -> addr = address;    // address which read is needed
        cache -> next_req -> data = 0;          // no data for read
        cache -> next_req -> id = request_id++;
        pending_req[cache->idx] = cache->next_req; // load request to mesi pool
        return MISS;
    } else if (cache -> busy = 1){
        // do nothing, requrst is on mesi.
        return MISS;
    }
}

// write data to cache. if MISS, fetched it throug messi and stall
int write_word(int address, cache_p cache, int* src_reg){
    int data = *src_reg;
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
        cache -> next_req -> id = request_id++;
        pending_req[cache->idx] = cache->next_req; // load request to mesi pool
        return  MISS;
    } else if (cache -> busy = 1){
        // do nothing, requrst is on mesi.
        return MISS;
    }


}


int main(int argc, char* argv[]){
    printf("\n\t > Main Function Of %s\n\n", argv[0]);
}
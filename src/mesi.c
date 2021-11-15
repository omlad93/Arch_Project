#include "mesi.h"


int read(int alligned_address, cache* cache){
    int tag = _get_tag(alligned_address);
    int block = _get_idx(alligned_address);
    cache->tags[block] = tag;
    for(int word=0; word<BLK_SIZE; word ++){
        cache->cache_data[block+word] = Memory->data[alligned_address+word];
    }
}

int read_ex(int alligned_address, cache* cache){
    int block = _get_idx(alligned_address);
    invalidate(block, cache);
    read(alligned_address,cache, Bus);
    cache->mesi_state[block] = Exclusive;
}

int generate_transaction(cache* requestor){
    
    requests[requestor->idx] = requestor->next_req;
}

int nop(){ }


int flush(int alligned_address, cache* cache);

int invalidate(int block, cache* invalidator){for (int i=0; i < CACHE_COUNT; i++){
    if (i != invalidator->idx){
        CACHES[i]-> mesi_state[block] = Invalid;
    }
    }}

int read_mode(int address, cache* requestor){
    int block = _get_idx(address);
    int tag = _get_tag(alligned(address));
    int stored, valid;
    // int exclusive;
    for (int i=0; i<CACHE_COUNT; i++){
        if (i != requestor->idx){
            stored = CACHES[i]->tags[block] == tag;
            valid =  CACHES[i]->mesi_state[block] != Invalid;
            //exclusive = CACHES[i]->mesi_state[block] == Exclusive; // if exclusive, need to change ?
            if (stored & valid) return Shared;
        }
    }
    return Exclusive;
}

int get_handler(int address){
    int block = _get_idx(address);
    int tag = _get_tag(alligned(address));
    int stored,modified;
    for (int i=0; i<CACHE_COUNT; i++){
         stored = (CACHES[i]->tags[block] == tag);
         modified = (CACHES[i]->mesi_state[block] == Modified);
         if (stored & modified) return i;
    }
    return main;
}

int round_robin();
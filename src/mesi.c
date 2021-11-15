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

int generate_transaction(bus_request* request, int origin){
    Bus->state = kick;
    Bus->addr = request->addr;
    Bus->cmd  = request->cmd;
    Bus->data = request->data;
    Bus->origin =  origin;
    Bus->shared = is_shared(origin, request->addr);
}

int is_shared(int requestor, int address){
    for (int i=0; i< CACHE_COUNT; i++){
        if (i != requestor){
            if query(CACHES[i], address, BusRd){
                return 1;
            }
        }
    }
    return 0;
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

int mesi_state_machine(){
    int handler;
    switch (Bus->state){
        case empty:
            break;
        case kick:
            handler = get_handler(Bus->addr);
            if _cache_handled(handler){
                // next cycle will be finish
                Bus->state = finish;
            else{
                // main memory latency
                Bus->state = busy;
                duration = 15; so_far=0;
            }
            break;
        case busy:
            if (so_far < duration){
                so_far++;
            } else {
                duration =0;
                so_far =0;
                Bus->state = finish;
            }
            break;
        case finish:
            handler = get_handler(Bus->addr);
            if _cache_handled(handler){
                Bus->data = CACHES[handler]->data[Bus->addr];
            } else {
                Bus->data = Memory[Bus->addr];
            }
            Bus->cmd = BusFlush;
            Bus->addr = Bus->addr;
            Bus->origin = handler;
            Bus->shared = is_shared(origin,Bus->addr);
            break;
        default:
            break;
        }
    }
}
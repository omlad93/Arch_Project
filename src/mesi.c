#include "mesi.h"


void generate_transaction(bus_request* request, int origin){
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


void flush0(){
    int alligned_address = alligned(Bus->addr);
    Bus->resp->request = Bus->origin;
    Bus->cmd = BusFlush;
    Bus->addr = Bus->addr;
    Bus->origin = Bus->resp->handler;
    Bus->shared = is_shared(handler,Bus->addr);
    if _cache_handled(handler){
        Bus->data = CACHES[handler]->data[alligned_address]
    } else {
        Bus->data = Memory[alligned_address]
    }
    Bus->state = flush1;    

}

void flush_cont(int alligned_address, int step){
    if _cache_handled(Bus->resp->handler){
        Bus->data = CACHES[Bus->resp->handler]->data[alligned_address + step]
        CACHES[Bus->resp->handler]->data[]
    } else {
        Bus->data = Memory[alligned_address + step]
    }
}

// Invalidate all other caches instances of block
int invalidate(int address, cache* invalidator){for (int i=0; i < CACHE_COUNT; i++){
    int block = _get_block(address);
    if (i != invalidator->idx && query(alligned(address), BusRd)){
        CACHES[i]-> mesi_state[block] = Invalid;
    }
    }}

// return mode for BusRd (Shared / Exclusive)
int BusRd_mode(int address, cache* requestor){
    int block = _get_block(address);
    int tag = _get_tag(alligned(address));
    int stored, valid;
    // int exclusive;
    for (int i=0; i<CACHE_COUNT; i++){
        if (i != requestor->idx){
            stored = (CACHES[i]->tags[block] == tag);
            valid =  (CACHES[i]->mesi_state[block] != Invalid);
            //exclusive = CACHES[i]->mesi_state[block] == Exclusive; // if exclusive, need to change ?
            if (stored & valid) return Shared;
        }
    }
    return Exclusive;
}

// determain the handler of the request: if (stored & modified) handler = cache.
void set_handler(int address){
    int block = _get_block(address);
    int tag = _get_tag(alligned(address));
    int stored,modified;
    for (int i=0; i<CACHE_COUNT; i++){
         stored = (CACHES[i]->tags[block] == tag);
         modified = (CACHES[i]->mesi_state[block] == Modified);
         if (stored & modified) Bus->resp->handler = i;
    }
    return  Bus->resp->handler = main;
}

// chose a request to handle from requests array
int round_robin();

// load a request to the mesi bus
void load_request();            

// first step of the request life-cycle
void kick_mesi(){

    set_handler(Bus->addr);
    if _cache_handled(Bus->resp->handler){
        // next cycle will be finish
        Bus->state = flush;
    }else{
        // main memory latency
        Bus->state = busy;
        waited_cycles=0;
    }
}

// it's pretty sraight forward
void wait_for_response(){
    if (waited_cycles < Memory->latency){
        waited_cycles++;
    }else{
        waited_cycles = 0;
        Bus->state = flush;
    }
}

// response for the reqiest
void flushing(){
    int word = Bus->resp->copyed;
    Bus->state = flush;
    Bus->origin = Bus->resp->handler;
    Bus->cmd = BusFlush;
    if (word < BLK_SIZE){
        if _cache_handled(Bus->resp->handler){
            Bus->data = CACHES[Bus->resp->handler]->data[_get_idx(Bus->addr)+word];
        } else {
            Bus->data = Memory->data[Bus->addr + word];
        }
        Bus->resp->copyed ++;
    } else{
        Bus->state = kick_mesi()
    }
    
}

//snoop the line
void snoop(){}


int mesi_state_machine(){
    switch (Bus->state){
        case idle:
            // ??
            break;
        case start:
            // add: load request()
            kick_mesi();
            break;
        case memory_wait:
            wait_for_response()
            break;
        case flush:
            flushing();
            break;
        default:
            break;
        }
    }
}
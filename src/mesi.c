#include "mesi.h"


void generate_transaction(bus_request* request){
    if (request){
        Bus->state = start;
        Bus->addr = request->addr;
        Bus->cmd  = request->cmd;
        Bus->data = request->data;
        Bus->req_id = request ->id;
        Bus->shared = is_shared(Bus->origin, request->addr);
    } 
    else {
        Bus->state = start;
        Bus->addr = 0;
        Bus->cmd  = BusNOP;
        Bus->data = 0;
        Bus->req_id = 0;
        Bus->shared = 0;
    }
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
int round_robin(){

}

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
    if (word == 0){
        Bus->state = flush;
        Bus->origin = Bus->resp->handler;
        Bus->cmd = BusFlush;
    }
    if (word < BLK_SIZE){
        if _cache_handled(Bus->resp->handler){
            Bus->data = CACHES[Bus->resp->handler]->data[_get_idx(Bus->addr)+word];
        } else {
            Bus->data = Memory->data[Bus->addr + word];
        }
        Bus->resp->copyed ++;
    } else{
        Bus->state = start;
    }
    
}

//snoop the line
void snoop(){}

//update requests priority
void pending_priority(){

}

//get longest request waiting
void get_next_request(){
    int pending_longest = -1, origin=-1;
    for (int i=0; i<CACHE_COUNT; i++){
        if (pending_time[i] > pending_longest){
            pending_longest = pending_time[i];
            origin = i;
        }
        pending_time[i] = inc_positive(pending_time[i]);
    }
    Bus->origin = origin;
    return (req != -1) ? (pending_req[req]) : (NULL) 
}



int mesi_state_machine(){
    switch (Bus->state){
         case start:
            generate_transaction(get_next_request());
            break;
        case memory_wait:
            kick_mesi();
            break;
        case flush:
            flushing();
            break;
        default:
            break;
        }
    
    snoop();
}


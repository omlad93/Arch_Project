#include "memory.h"

// call upon loding request on the bus
void clear_request_from_cahce(int c){
    pending_req[c] = NULL;
    _cache_on_bus(c);
}

// looad a transaction on the bus (request)
void generate_transaction(bus_request_p request){
    if (request){
        Bus->state = start;
        Bus->addr = request->addr;
        Bus->cmd  = request->cmd;
        Bus->data = request->data;
        Bus->req_id = request ->id;
        Bus->shared = is_shared(Bus->origin, request->addr);
        _cache_on_bus(Bus->origin);
        clear_request_from_cahce(Bus->resp->requestor);
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

// check if data is also cached in other caches
int is_shared(int requestor, int address){
    int stored, need_to_check;
    for (int i=0; i< CACHE_COUNT; i++){
        need_to_check = i != requestor;
        stored = query(address, CACHES[i], BusRd);
        if (need_to_check && stored)  {
            return 1;
        }
    }
    return 0;
}

// return mode for BusRd (Shared / Exclusive)
int BusRd_mode(int address, cache_p requestor){
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
         if (stored & modified){
            Bus->resp->handler = i;
            return;
         }
    }
    Bus->resp->handler = main_mem;
}

// load a request to the mesi bus
void load_request();            

// call when a request is loaded on the bus
void kick_mesi(){
    set_handler(Bus->addr);
    if _cache_handled(Bus->resp->handler){
        // next cycle will be finish
        Bus->state = flush;
    }else{
        // main memory latency
        Bus->state = memory_wait;
        waited_cycles=0;
    }
}

// no one uses the bus if waiting for memory
void wait_for_response(){
    if (waited_cycles < Memory->latency){
        waited_cycles++;
    }else{
        waited_cycles = 0;
        Bus->state = flush;
    }
}

// transfer request over the line
void flushing(){
    int word = Bus->resp->copyed;
    if (word == 0){
        Bus->state = flush;
        Bus->origin = Bus->resp->handler;
        Bus->cmd = BusFlush;
    }
    if (word < BLK_SIZE){
        if _cache_handled(Bus->resp->handler){
            Bus->data = CACHES[Bus->resp->handler]->cache_data[_get_idx(Bus->addr)+word];
            _cache_on_bus(Bus->resp->handler);
        } else {
            Bus->data = Memory->data[Bus->addr + word];
        }
        Bus->resp->copyed ++;
    } else{
        Bus->state = start;
    }
    
}

// go over caches, and invalidate the data if needed, skip given caches
void invalidate_caches(int client, int provider, int block_idx){
    int cache_is_relevant;
    for (int j=0; j<3; j++){
        cache_is_relevant = (j != client) && (j != provider);
        if ( cache_is_relevant && query(Bus->addr,CACHES[j],BusRd)){
             CACHES[j]->mesi_state[block_idx] = Invalid;
        }
    }
}

// perform memory copy on flush when request was Rd
void snoop_Rd(int handler, int client){
    int shared;
    if _cache_handled(handler){
        // copy data fro mmemory to requesting cache
        CACHES[client]->cache_data[_get_idx(Bus->addr)] = Memory->data[Bus->addr];
    } else { 
        // copy data from handler cache to memory and to cache.
        Memory->data[Bus->addr] = CACHES[handler]->cache_data[_get_idx(Bus->addr)]; // copy data
        CACHES[client]->cache_data[_get_idx(Bus->addr)] = CACHES[handler]->cache_data[_get_idx(Bus->addr)];
    }
    if (Bus->resp->copyed == LAST_CPY){
        // make data valid in client and invalid in handler
        shared = (Bus->shared) ||  _cache_handled(handler);
        CACHES[client]->mesi_state[_get_block(Bus->addr)] = shared ? Shared : Exclusive;
        CACHES[client]->tags[_get_block(Bus->addr)] = _get_tag(Bus->addr);
         if ( _cache_handled(handler)) {CACHES[handler]->mesi_state[_get_block(Bus->addr)] = Shared;}
    }
}

// perform memory copy on flush when request was RdX
void snoop_RdX(int handler, int client){
    if _cache_handled(handler){
        // invalidate other caches (wait with handler cache)
        // copy data fro mmemory to requesting cache
        invalidate_caches(client, handler, _get_block(Bus->addr));
        Memory->data[Bus->addr] = CACHES[handler]->cache_data[_get_idx(Bus->addr)]; // copy data
        CACHES[client]->cache_data[_get_idx(Bus->addr)] = CACHES[handler]->cache_data[_get_idx(Bus->addr)];
        
    } else { 
        // invalidate other caches
        // copy data from handler cache to memory and to cache.
        invalidate_caches(client, handler, _get_block(Bus->addr));
        CACHES[client]->cache_data[_get_idx(Bus->addr)] = Memory->data[Bus->addr];  
    }
    if (Bus->resp->copyed == LAST_CPY){
        // make data valid in client and invalid in handler
        CACHES[client]->mesi_state[_get_block(Bus->addr)] = Exclusive;
        CACHES[client]->tags[_get_block(Bus->addr)] = _get_tag(Bus->addr);
        if ( _cache_handled(handler)) {CACHES[handler]->mesi_state[_get_block(Bus->addr)] = Invalid;}
    }
}

//snoop the line on flush: modify memory elements
void snoop(){
    // cache_p requestor = CACHES[Bus->resp->requestor]
    int client = Bus->resp->requestor;
    int handler = Bus->resp->handler;
    switch (Bus->resp->cmd){
        case BusRd:
            snoop_Rd(handler,client);
            return;
        case BusRdX:
            snoop_RdX(handler,client);
            return;
        default:
            // function is called upon flush op, causing the change to be inside requeting cache
            // no possible to have a flush over flush or flush over nop.
            return;
    }

}

// get the next core to serve
int next_core_to_serve(){
    int most_neglected = -1, max_neglect_time = -1;
    int current_neglect, update;
    for (int j=0; j<CACHE_COUNT;j++){
        if (pending_req[j] != NULL){
            current_neglect =  time_diff(cycle,last_time_served[j]);
            update = (max_neglect_time < current_neglect);

            most_neglected = (update) ? j : most_neglected;
            max_neglect_time = (update) ? current_neglect: max_neglect_time ;
        }
    }
    return most_neglected;
}

// get longest request waiting
bus_request_p get_next_request(){
    int origin = next_core_to_serve();
    Bus->origin = origin;
    Bus->resp->requestor =  origin;
    Bus->resp->cmd = (origin >= 0) ? pending_req[origin]->cmd : BusNOP;
    return (origin >= 0) ? (pending_req[origin]) : (NULL);
}

// manage transaction over messi using state machine
void mesi_state_machine(){   
    switch (Bus->state){
         case start:
            generate_transaction(get_next_request()); // load transaction on the bus
            kick_mesi(); //move state machine according to transaction
            break;
        case memory_wait:
            wait_for_response(); // do nothing untill response is ready
            break;
        case flush:
            flushing();
            snoop();
            break;
        default:
            break;
        }
    cycle++;
}


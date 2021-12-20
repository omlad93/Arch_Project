
#include "memory.h"

/* ******************       Utility Functios        ******************* */
/* ****************** Use for Set & Monitor System  ******************* */


// Allocate cache fields
void init_cache(cache_p cache){
    cache->idx = cache_idx;       // create unique idx
    CACHES[cache_idx] = cache;    // add to array
    cache_idx++;                  // inc idx
    cache-> busy = 0;

    cache->next_req = calloc(1,sizeof(bus_request));
    cache->next_evict = calloc(1,sizeof(bus_request));
    for (int i=0; i<WORDS; i++){    /// zero all data
        cache->cache_data[i] = 0;
        if (i < BLOCKS){
            cache->tags[i] = 0;
            cache->mesi_state[i] = 0;
        }
    }


}

// Allocate memory strict & MESI bus
void initiate_memory_system(){

    cache_idx = 0;                                                 //static idx for cache id
    request_id = 0;                                                // each request will have unique id
    for (int i=0; i < CACHE_COUNT ; i++){
        pending_evc[i] = NULL;
        pending_req[i] = NULL;
        last_time_served[i] =0;
    }
    // pending_req = {NULL, NULL, NULL, NULL}; // all the pending requests
    // pending_evc = {NULL, NULL, NULL, NULL}; // all the pending evicts
    // last_time_served = {0};                           // array for use of round robin
    Memory = calloc(1, sizeof(main_memory));
    Memory->latency = memory_latency;
    Bus = calloc(1,sizeof(mesi_bus));
    Bus->resp = calloc(1,sizeof(response));
    bus_trace = fopen("bustrace_qad.txt","w");

}

// free memory struct & MESI bus
void close_memory_system(){
    free(Memory);
    free(Bus->resp);
    free(Bus);
    for (int c=0; c<CACHE_COUNT; c++){
        release_cache(CACHES[c]);
    }
    fclose(bus_trace);
}

// free cache fields & pointer
void release_cache(cache_p cache){
    // free(cache->next_evict);
    free(cache->next_req);
    free(cache); // ?
}



// print bus trace file
// CYCLE[%d] bus_origid[1] bus_cmd[1] bus_addr[1] bus_data[8] bus_shared[1]
void write_bus_trace(FILE* file_w, int currect_cycle){
    if(Bus->cmd != BusNOP){
        fprintf(file_w,"%.5i %01X %01X %05X %08X %01X\n", currect_cycle, Bus->origin, Bus->cmd, Bus->addr, Bus->data, Bus->shared );
    }
}




// print memory cached in c in 'dumb' format
void dump_cache(cache_p c, FILE* dsram, FILE* tsram){
    int meta_data, blk;
    for (int i=0; i <WORDS; i++ ){
        if (i == aligned(i)){
            blk = _get_block(i);
            meta_data = ( ((c->mesi_state[blk]) << 0xC) | (c->tags[blk]));
            fprintf(tsram,"%08x\n",meta_data);
        }
        fprintf(dsram,"%08x\n", c->cache_data[i]);
    }
}

// print all memory components memory in 'dumb' format
void dump_memory(sim_files_p files_p){
    int meta_data, blk;
    for (int i=0; i <mem_size; i++ ){
        fprintf(files_p->memout,"%08x\n",Memory->data[i]);
    }
    dump_cache(CACHES[0], files_p->dsram0, files_p->tsram0);
    dump_cache(CACHES[1], files_p->dsram1, files_p->tsram1);
    dump_cache(CACHES[2], files_p->dsram2, files_p->tsram2);
    dump_cache(CACHES[3], files_p->dsram3, files_p->tsram3);
}

// TODO: I/O functions (Read, Monitor, Store)

/* ******************    Core - Memory Interface    ******************* */
/* ****************** Use for Load / Store opcodes  ******************* */


// check if address is cached, return HIT or MISS
// for BusRd - if data is valid it's enough
// for BusRdX - data has to be in M state
int query(int address, cache_p cache, int mode){
    int block_tag = _get_tag(aligned(address));
    int block_idx = _get_block(address);
    int valid;
    int stored = (block_tag == cache->tags[block_idx]);
    if (mode == BusRd) valid = cache->mesi_state[block_idx] != Invalid; // for read, except all modes but invalid
    else if (mode == BusRdX) valid = (cache->mesi_state[block_idx] == Exclusive) || (cache->mesi_state[block_idx] == Modified); // for write, accept only exclusive
    return (stored & valid) ? HIT : MISS;
}

// read word from cache. if MISS, fetched it through mesi and stall
int read_word(int address, cache_p cache, int* dest_reg){
    int idx = _get_idx(aligned(address));
    int blk = _get_block(address);
    if (query(address,cache,BusRd) == HIT){
        *dest_reg = cache->cache_data[_get_idx(address)];
        return HIT;
    }
    else if( (cache->busy) == 0) {
        // generate a mesi transaction
        cache -> busy = 1;                      // ignore same calls
        if (need_to_evict( blk, cache) ) {
            // generate eviction
            cache -> next_evict -> cmd = BusFlush;
            cache -> next_evict -> addr = construct_address(cache->tags[blk],idx) ; 
            cache -> next_evict -> data = 0;
            cache -> next_evict -> id = request_id ++;
            pending_evc[cache->idx] = cache->next_evict;
        }
        cache -> next_req -> cmd = BusRd;       // Read request
        cache -> next_req -> addr = address;    // address which read is needed
        cache -> next_req -> data = 0;          // no data for read
        cache -> next_req -> id = request_id++;
        pending_req[cache->idx] = cache->next_req; // load request to mesi pool

        // printf("\n\t > queued request %.5x\n", cache->next_req->id);
        return MISS;
    }
    return MISS;
    
}

// write data to cache. if MISS, fetched it through mesi and stall
int write_word(int address, cache_p cache, int* src_reg){
    int data = *src_reg;
    if (query(address,cache, BusRdX) == HIT){
        cache->cache_data[_get_idx(address)] = data;
        cache->mesi_state[_get_block(address)] = Modified;
        return HIT; 
    }
    else if( (cache->busy) == 0) {
        // generate a mesi transaction
        cache -> busy = 1;
        cache -> next_req -> cmd = BusRdX;
        cache -> next_req -> addr = address;
        cache -> next_req -> data = 0;
        // cache -> next_req -> data = data;
        cache -> next_req -> id = request_id++;
        pending_req[cache->idx] = cache->next_req; // load request to mesi pool
        
        // printf("\n\t > queued request %.5x\n", cache->next_req->id);
        return  MISS;
    } 
    return MISS;
}



/* ******************    Cache -> MESI Interface    ******************* */
/* ****************** Use to assure coherency in mem ******************* */


// call upon loading request on the bus
void clear_request_from_cache(int c, int clock_cycle){
    if (Bus->cmd == BusFlush ){
        // on dirty evict
        pending_evc[c] = NULL;
    }else {
        // om classic request
        pending_req[c] = NULL;
    }
    _cache_on_bus(c,clock_cycle);
}

// load a transaction on the bus (request)
void generate_transaction(bus_request_p request, int clock_cycle){
    if (request){
        Bus->state = (request->cmd != BusFlush) ? start : dirty_evict ;
        Bus->addr = request->addr;
        Bus->cmd  = request->cmd;
        Bus->data = request->data;
        Bus->req_id = request ->id;
        Bus->shared = is_shared(Bus->origin, request->addr);  
        clear_request_from_cache(Bus->resp->requestor, clock_cycle);
        if(Bus->shared){
            // make sure no one is Exclusive
            for(int c=0; c<CACHE_COUNT; c++){
                int is_exclusive = CACHES[c]->mesi_state[_get_block(Bus->addr)] == Exclusive;
                int stored = ( _get_tag(aligned(Bus->addr)) == CACHES[c]->tags[_get_block(Bus->addr)]);
                if (stored && is_exclusive){
                    CACHES[c]->mesi_state[_get_block(Bus->addr)] = Shared;
                }
            }
        }
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
    int stored, need_to_check, shared=0;
    for (int i=0; i< CACHE_COUNT; i++){
        need_to_check = ((i != requestor) && (CACHES[i] != NULL));
        if (need_to_check)  {
            shared = shared || is_hit((query(address, CACHES[i], BusRd) == HIT));
        }
    }
    return shared;
}

// determain the handler of the request: if (stored & modified) handler = cache.
void set_handler(int address){
    int block = _get_block(address);
    int tag = _get_tag(aligned(address));
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

// call when a request is loaded on the bus
void kick_mesi(){
    if (!Bus->cmd || Bus -> cmd == BusFlush){
        return;
    }
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
    // if (waited_cycles < Memory->latency - 1){
    //     waited_cycles++;
    // }else{
    //     waited_cycles = 0;
    //     Bus->state = flush;
    // }
    if(waited_cycles == Memory->latency - 1){
        Bus->state = flush;
        waited_cycles = 0;
    } else {
        waited_cycles++;
    }
}

// transfer request over the line
void flushing(int clock_cycle){
    int word = Bus->resp->copied;
    if (word == 0){
        // Bus->state = flush;
        Bus->origin = Bus->resp->handler;
        Bus->cmd = BusFlush;
    }
    if (word < BLK_SIZE){
        Bus->addr = aligned(Bus->addr) + word;
        if _cache_handled(Bus->resp->handler){
            Bus->data = CACHES[Bus->resp->handler]->cache_data[_get_idx(Bus->addr)];
            _cache_on_bus(Bus->resp->handler, clock_cycle);
        } else {
            Bus->data = Memory->data[Bus->addr];
        }
        Bus->resp->copied ++;
    } else{
        Bus->state = start;
    }
    
}

// go over caches, and invalidate the data if needed, skip given caches
void invalidate_caches(int client, int provider, int block_idx){
    int cache_is_relevant;
    for (int j=0; j<CACHE_COUNT; j++){
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
        // copy data from handler cache to memory and to cache.
        Memory->data[Bus->addr] = CACHES[handler]->cache_data[_get_idx(Bus->addr)]; // copy data
        CACHES[client]->cache_data[_get_idx(Bus->addr)] = CACHES[handler]->cache_data[_get_idx(Bus->addr)];
    } else { 
        // copy data from memory to requesting cache
        CACHES[client]->cache_data[_get_idx(Bus->addr)] = Memory->data[Bus->addr];
    }
    if (Bus->resp->copied == BLK_SIZE){
        // make data valid in client and invalid in handler
        CACHES[client]->busy = 0;
        shared = (Bus->shared) ||  _cache_handled(handler);
        CACHES[client]->mesi_state[_get_block(Bus->addr)] = shared ? Shared : Exclusive;
        CACHES[client]->tags[_get_block(Bus->addr)] = _get_tag(Bus->addr);
        if ( _cache_handled(handler)) {
            CACHES[handler]->mesi_state[_get_block(Bus->addr)] = Shared;
            }
        
        Bus->resp->copied = 0;
        Bus->state = start;
    }
}

// perform memory copy on flush when request was RdX
void snoop_RdX(int handler, int client){
    if _cache_handled(handler){
        // invalidate other caches (wait with handler cache)
        // copy data fro memory to requesting cache
        invalidate_caches(client, main_mem, _get_block(Bus->addr));
        Memory->data[Bus->addr] = CACHES[handler]->cache_data[_get_idx(Bus->addr)]; // copy data
        CACHES[client]->cache_data[_get_idx(Bus->addr)] = CACHES[handler]->cache_data[_get_idx(Bus->addr)];
        
    } else { 
        // invalidate other caches
        // copy data from handler cache to memory and to cache.
        invalidate_caches(client, main_mem, _get_block(Bus->addr));
        CACHES[client]->cache_data[_get_idx(Bus->addr)] = Memory->data[Bus->addr];  
    }
    if (Bus->resp->copied == BLK_SIZE){
        // make data valid in client and invalid in handler
        CACHES[client]->busy = 0;
        CACHES[client]->mesi_state[_get_block(Bus->addr)] = Exclusive;
        CACHES[client]->tags[_get_block(Bus->addr)] = _get_tag(Bus->addr);
        if ( _cache_handled(handler)) {CACHES[handler]->mesi_state[_get_block(Bus->addr)] = Invalid;}
        Bus->resp->copied = 0;
        Bus->state = start;

    }
}

// perform memory copy on evict
void evict(){
    int cache_idx = Bus->resp->handler;
    Memory->data[Bus->addr] = CACHES[cache_idx]->cache_data[_get_idx(Bus->addr)]; // copy data        
    if (Bus->resp->copied == BLK_SIZE){
        CACHES[cache_idx]->mesi_state[_get_block(Bus->addr)] = Invalid;
        Bus->resp->copied = 0;
        Bus->state = start;
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
            // no possible to have a flush over flush or flush over nop.
            return;
    }

}

// get the next core to serve
int next_core_to_serve(int clock_cycle){
    int most_neglected = -1, max_neglect_time = -1;
    int current_neglect, update;
    for (int j=0; j<CACHE_COUNT;j++){
        if (pending_req[j] != NULL){
            current_neglect =  time_diff(clock_cycle,last_time_served[j]);
            update = (max_neglect_time < current_neglect);
            most_neglected = (update) ? j : most_neglected;
            max_neglect_time = (update) ? current_neglect: max_neglect_time ;
        }
    }
    // printf(" Last Time Served:[%.8i, %.8i, %.8i, %.8i] \n",last_time_served[0], last_time_served[1], last_time_served[2], last_time_served[3]);
    return most_neglected;
}

// chose next command on bus (used on dirty evict)
bus_request_p get_request_per_cache(int cache_idx){
    return (evict_first(cache_idx)) ? (pending_evc[cache_idx]) : (pending_req[cache_idx]); 

}

// when evict is no longer needed
void clear_old_evicts(){
    for (int c=0; c<CACHE_COUNT; c++){
        if (pending_evc[c] != NULL){ // if there is a pending eviction
            if(CACHES[c]->mesi_state[_get_block(pending_evc[c]->addr)] != Modified){ // if do not need to evict
                pending_evc[c] = NULL; // remove evict
            }
        }
    }
}

// get longest request waiting
bus_request_p get_next_request(int clock_cycle){
    clear_old_evicts();
    int origin = next_core_to_serve(clock_cycle);
    bus_request_p upload = get_request_per_cache(origin);
    Bus->origin = (origin > 0) ? origin : 0;
    Bus->resp->requestor = (origin >0) ? origin : 0;
    Bus->resp->cmd = (origin >= 0) ? (upload->cmd) : (BusNOP);
    return (origin >= 0) ? (upload) : (NULL);
}

// manage transaction over mesi using state machine
void mesi_state_machine(sim_files_p files, int clock_cycle){
    int state = Bus->state;
    switch (state){
         case start:
            generate_transaction(get_next_request(clock_cycle), clock_cycle);
            kick_mesi();         //move state machine according to transaction
            write_bus_trace(files->bustrace, clock_cycle);
            break;
        case memory_wait:
            wait_for_response(); // do nothing until response is ready          
            break;
        case flush:
            flushing(clock_cycle);          // return requested data
            snoop();
            write_bus_trace(files->bustrace, clock_cycle);
            break;
        case dirty_evict:
            flushing(clock_cycle);          // send dirty block on evict
            evict();
            write_bus_trace(files->bustrace, clock_cycle);
            break;
        default:
            printf("OOPS!");
            exit(1);
        }
    // write_bus_trace(files->bustrace, clock_cycle);
}


/* ******************      Debugging functions      ****************** */
/* ****************** Used for Memory verifications ****************** */

// Query in single core mode
int non_mesi_query(int address, cache_p cache){
    int block_tag = _get_tag(aligned(address));
    int block_idx = _get_block(address);
    int valid = cache->mesi_state[block_idx] != Invalid;
    int stored = (block_tag == cache->tags[block_idx]);
    return (stored & valid) ? HIT : MISS;
}

// Read in single core mode
int non_mesi_read(int address, cache_p cache, int* dest_reg){
    if (non_mesi_query(address,cache) == HIT){

        *dest_reg = cache->cache_data[_get_idx(address)];
        printf("\t\t > hit  Rd @ %.8i: %.8i\n", address,cache->cache_data[_get_idx(address)]);
        return HIT;
    } else {
        printf("\t\t > miss Rd @ %.8i: ", address);
        fetch_block_immediate(aligned(address), cache);
        return MISS;
    }
}

// Write in single core mode
int non_mesi_write(int address, cache_p cache, int* src_reg){
    if (non_mesi_query(address,cache) == HIT){
        cache->cache_data[_get_idx(address)] = *src_reg;
        printf("\t\t > hit  Wr @ %.8i: %.8i\n", address, cache->cache_data[_get_idx(address)] );
        return HIT;
    } else {
        printf("\t\t > miss Wr @ %.8i: ", address);
        fetch_block_immediate(aligned(address), cache);
        return MISS;
    }
}

// Copy block of data from memory (1 cycle)
void fetch_block_immediate(int aligned_address, cache_p cache){

    for (int j=0; j<BLK_SIZE; j++){
        cache->cache_data[_get_idx(aligned_address+j)] = Memory->data[(aligned_address+j)];
    }
    cache->tags[_get_block(aligned_address)] = _get_tag(aligned_address);
    cache->mesi_state[_get_block(aligned_address)] = Exclusive;
    printf(" Fetched Memory[%i:%i] {Block:%i with Tag:%i}\n", \
             aligned_address, aligned_address+LAST_CPY, _get_block(aligned_address), _get_tag(aligned_address));


}

// Initate memory with values: MEM[i] = i
void load_mem_manually(){
    for (int word=0; word<mem_size; word++){
        Memory->data[word] = word;
    }
    printf("\tMemory was loaded correctly {Memory[i] = i}\n");
    Memory->latency = memory_latency;
}

// Initate memory with values manually
void load_mem_manually_for_core_debug(){
    for (int word=0; word<mem_size; word++){
        Memory->data[word] = word;
    }
    Memory->data[15] = 100;
    Memory->data[17] = 16;
    Memory->data[18] = 32;
    Memory->data[63] = 7;
    Memory->latency = memory_latency;

}

// Print cache to file, as a table with fields
void print_cache(FILE* file_w, cache_p cache){
    char mesi_chars[4] = {'I', 'S', 'E', 'M'};
    char mesi;
    int tag, word, blk;
    fprintf(file_w,"| Bk | Index |  Tag  |   Word   | MESI |\n");
    fprintf(file_w,"----------------------------------------\n");

    for (int i =0; i < WORDS; i++){
        blk = _get_block(i);
        mesi = mesi_chars[cache->mesi_state[blk]];
        tag = cache->tags[blk];
        word = cache->cache_data[_get_idx(i)];
        fprintf(file_w,"| %.2i | %.5i | %.5i | %.8i |  %c   | \n",blk ,i, tag, word, mesi);
        if (i % BLK_SIZE == LAST_CPY){
            fprintf(file_w,"|----|-------|-------|----------|------| \n");
        }
    }
}

// Print mem to file, as a table with fields
void print_mem(FILE* file_w){
    int tag, word, blk;
    //fprintf(file_w,"| Bk | Index |  Tag  |   Word   |    Addr    |\n");
    fprintf(file_w,"| Address |   word   | block |  Tag  |\n");
    for (int i =0; i < mem_size; i++){
        blk = _get_block(i);
        tag = _get_tag(i);
        word = Memory -> data[i];
        fprintf(file_w,"| %.7i | %.8i | %.5i | %.5i | \n",i ,word, blk, tag);
        if (i % BLK_SIZE == LAST_CPY){
            fprintf(file_w,"|---------|----------|-------|-------|\n");
        }
    }
}





#include "memory.h"

// call when on an instace upon creation.
void init_cache(cache_p cache){
    cache->idx = cache_idx;       // create unique idx
    CACHES[cache_idx] = cache;    // add to array
    cache_idx++;                  // inc idx
    cache-> busy = 0;

    cache->next_req = calloc(1,sizeof(bus_request));
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
    else if( (cache->busy) == 0) {
        // generate a mesi transaction
        cache -> busy = 1;                      // ignore same calls
        cache -> next_req -> cmd = BusRd;       // Read request
        cache -> next_req -> addr = address;    // address which read is needed
        cache -> next_req -> data = 0;          // no data for read
        cache -> next_req -> id = request_id++;
        pending_req[cache->idx] = cache->next_req; // load request to mesi pool
        printf("\n\t > queued request %.5x\n", cache->next_req->id);
        return MISS;
    }
    printf("\n\t\t > polled request %.5x\n", cache->next_req->id);
    return MISS;
    
}

// write data to cache. if MISS, fetched it throug messi and stall
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
        cache -> next_req -> data = data;
        cache -> next_req -> id = request_id++;
        pending_req[cache->idx] = cache->next_req; // load request to mesi pool
        return  MISS;
    } 
    return MISS;
}



/* ************************ Utility Functios  ************************* */

void load_mem_manually(){
    Memory = calloc(1, sizeof(main_memory));
    for (int word=0; word<mem_size; word++){
        Memory->data[word] = word;
    }
    printf("\tMemory was loaded correctly {Memory[i] = i}\n");
    Memory->latency = 1;
}

void print_cache(FILE* file_w, cache_p cache){
    char mesi_chars[4] = {'I', 'S', 'E', 'M'};
    char mesi;
    int tag, word, blk;
    fprintf(file_w,"| Bk | Index |  Tag  |   Word   | MESI |\n");
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

void print_bus(){
    char* states[3] = {"start", "memory_wait", "flush"};
    char* comnds[4] = {"BusNop","BusRd", "BusRdX", "Flush"};
    char* origin[5] = {"core0", "core1", "core2", "core2", "Main Memory"};
    printf("\n\tBus{\n\t\tState : %s\n", states[Bus->state]);
    printf("\t\tCMD   : %s (%s)->(%s)\n", comnds[Bus->cmd], origin[Bus->origin], origin[Bus->resp->handler]);
    printf("\t\tAddr  : %.8i\n\t}\n", Bus->addr);
}

/* *********************** Debugging functions *********************** */

int non_mesi_query(int address, cache_p cache){
    int block_tag = _get_tag(alligned(address));
    int block_idx = _get_block(address);
    int valid = cache->mesi_state[block_idx] != Invalid;
    int stored = (block_tag == cache->tags[block_idx]);
    return (stored & valid) ? HIT : MISS;
}

int non_mesi_read(int address, cache_p cache, int* dest_reg){
    if (non_mesi_query(address,cache) == HIT){

        *dest_reg = cache->cache_data[_get_idx(address)];
        printf("\t\t > hit  Rd @ %.8i: %.8i\n", address,cache->cache_data[_get_idx(address)]);
        return HIT;
    } else {
        printf("\t\t > miss Rd @ %.8i: ", address);
        fetch_block_immediate(alligned(address), cache);
        return MISS;
    }
}

int non_mesi_write(int address, cache_p cache, int* src_reg){
    if (non_mesi_query(address,cache) == HIT){
        cache->cache_data[_get_idx(address)] = *src_reg;
        printf("\t\t > hit  Wr @ %.8i: %.8i\n", address, cache->cache_data[_get_idx(address)] );
        return HIT;
    } else {
        printf("\t\t > miss Wr @ %.8i: ", address);
        fetch_block_immediate(alligned(address), cache);
        return MISS;
    }
}

void fetch_block_immediate(int alligned_address, cache_p cache){

    for (int j=0; j<BLK_SIZE; j++){
        cache->cache_data[_get_idx(alligned_address+j)] = Memory->data[(alligned_address+j)];
    }
    cache->tags[_get_block(alligned_address)] = _get_tag(alligned_address);
    cache->mesi_state[_get_block(alligned_address)] = Exclusive;
    printf(" Fetched Memory[%i:%i] {Block:%i with Tag:%i}\n", \
             alligned_address, alligned_address+LAST_CPY, _get_block(alligned_address), _get_tag(alligned_address));


}


/* *********************** MESI bus Functios  ************************ */

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
        stored = query(address, &CACHES[i], BusRd);
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
         stored = (&CACHES[i]->tags[block] == tag);
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
void mesi_state_machine(mesi_bus_p bus){
    char* states[3] = {"start", "memory_wait", "flush"};
    int state = bus->state;
    // print_bus(); 
    printf("\n\t $ MSM: %s", states[state]);
    switch (bus->state){
         case start:
            printf("\n\t  > MSM: Start");
            generate_transaction(get_next_request()); // load transaction on the bus
            kick_mesi(); //move state machine according to transaction
            break;
        case memory_wait:
            printf("\n\t MSM: Mwait");
            wait_for_response(); // do nothing untill response is ready
            break;
        case flush:
            printf("\n\t MSM: Flush");
            flushing();
            snoop();
            break;
        default:
            printf("OOPS!");
            exit(1);
        }
    cycle++;
}


/* ***************** Main Function - inner debug  ****************** */


int main(int argc, char* argv[]){
    FILE* cache_0 = fopen("data_c0.txt","w");
    // FILE* cache_1 = fopen("data_c1.txt","w");
    // FILE* cache_2 = fopen("data_c2.txt","w");
    // FILE* cache_3 = fopen("data_c3.txt","w");

    cache_p c0 = calloc(1,sizeof(cache)); init_cache(c0);
    // cache_p c1 = calloc(1,sizeof(cache)); init_cache(c1);
    // cache_p c2 = calloc(1,sizeof(cache)); init_cache(c2);
    // cache_p c3 = calloc(1,sizeof(cache)); init_cache(c3);
    
    Bus = calloc(1,sizeof(mesi_bus)); Bus->resp = calloc(1,sizeof(response));
    // print_bus();

    printf("\n\tCache Debug Main Function: %s\n", argv[argc-1]);

    int run=1, counter =0 ,word=0;

    int st0 ,rv0; //,wv0;
    // int wv1,wv3;


    load_mem_manually();
    while(run){
        // if (word%2 == 1 && (_get_block(word)%4 != 1)){
        //     wv = word%10;
        //     st = non_mesi_write(word,c0,&wv);
        //     word = (st==HIT) ? (word+1) : (word);
        //     counter = (st==HIT) ? (counter+1) : (counter);
        //     run = (counter < 1.5*WORDS);
        // }else{
        //     word++;
        //     counter++;
        // }
        // wv1 = 1912;
        // wv3 = 4693;
        st0 = read_word(word,c0,&rv0);
        mesi_state_machine(Bus);
        // st1 = write_word(word+1,c0,&wv1);
        // mesi_state_machine();
        // st2 = read_word(word+2,c0,&rv2);
        // mesi_state_machine();
        // st1 = write_word(word+3,c0,&wv3);
        // mesi_state_machine();

        counter++;
        run = (st0 != HIT);
        if (counter > 2*memory_latency){
            printf("\n >> stopped <<\n");
            run = 0;
        }

    }
    printf(" >> took %i cycles", cycle);
    print_cache(cache_0,c0);
    //  print_cache(cache_1,c1); 
    // print_cache(cache_2,c2); print_cache(cache_3,c3);

    fclose(cache_0);
    // fclose(cache_1);fclose(cache_2);fclose(cache_3);
    free(Memory);

    printf("\t Finished [V]");
}
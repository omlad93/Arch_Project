#include "memory.h"

// call when on an instace upon creation.
void init_cache(cache_p cache){
    cache->idx = cache_idx;       // create unique idx
    CACHES[cache_idx] = cache;    // add to array
    cache_idx++;                  // inc idx
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
    else if( (cache->busy) == 0) {
        // generate a mesi transaction
        cache -> busy = 1;                      // ignore same calls
        cache -> next_req -> cmd = BusRd;       // Read request
        cache -> next_req -> addr = address;    // address which read is needed
        cache -> next_req -> data = 0;          // no data for read
        cache -> next_req -> id = request_id++;
        pending_req[cache->idx] = cache->next_req; // load request to mesi pool
        return MISS;
    }
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


/* ******************* Debugging functions ******************** */

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
        // printf("\t\t > hit  Rd @ %i\n", address);
        return HIT;
    } else {
        printf("\t\t > miss Rd @ %.8i: ", address);
        fetch_block_immediate(address, cache);
        return MISS;
    }
}

int non_mesi_write(int address, cache_p cache, int* src_reg){
    if (non_mesi_query(address,cache) == HIT){
        cache->cache_data[_get_idx(address)] = *src_reg;
        // printf("\t\t > hit  Wr @ %i\n", address);
        return HIT;
    } else {
        printf("\t\t > miss Wr @ %.8i\n", address);
        fetch_block_immediate(address, cache);
        return MISS;
    }
}

void fetch_block_immediate(int address, cache_p cache){
    for (int j=0; j<BLK_SIZE; j++){
        cache->cache_data[_get_idx( (address+j) )] = Memory->data[(address+j)];
    }
    cache->tags[_get_block(address)] = _get_tag(address);
    cache->mesi_state[_get_block(address)] = Exclusive;
    printf(" Fetched Memory[%i:%i] {Block:%i with Tag:%i}\n", \
             address, address+LAST_CPY, _get_block(address), _get_tag(address));


}

void load_mem_manually(){
    Memory = calloc(1, sizeof(main_memory));
    for (int word=0; word<mem_size; word++){
        Memory->data[word] = word;
    }
    printf("\tMemory was loaded correctly {Memory[i] = i}\n");
    Memory->latency = 1;
}

int main(int argc, char* argv[]){
    cache_p c0 = calloc(1,sizeof(cache));
    init_cache(c0);
    int value = 0;
    int rquested_status;
    int run = 1, word=0;

    printf("\n\tCache Debug Main Function:\n");
    load_mem_manually();
    while(run){
        // printf("\tRequest: Read %i\n", word);
        rquested_status = non_mesi_read(word, c0, &value);
        if (rquested_status == HIT){
            if (word % 4 ==0){
                printf("\t\t  {%.8i, ",value);
            }else if(word %4 == 3){
                printf("%.8i}\n", value);
            }else{
                printf("%.8i, ", value);
            }
        }
        word = (rquested_status == HIT) ? word + 1 : word; 
        run = (word < 511);
    }
    printf("\t Finished [V]");
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef MEMORY_H
#define MEMORY_H

// cache parameters
#define CACHE_COUNT 4
#define HIT 1
#define MISS -1
#define WORDS 256
#define BLOCKS 64
#define BLK_SIZE 4
#define LAST_CPY 3 // BLK_SIZE - 1

// masks and shifts
#define MEM_MASK 0x000FFFFF // 20 bits mask
#define TAG_MASK 0x000FFF00 // 12 bits ( 0 : 11) 
#define BLK_MASK 0x000000FC // 6 bits  (12 : git p17)
#define OST_MASK 0x00000003 // 2 bits  (18 : 19)
#define IDX_MASK 0x000000FF // 8 bits  (7:0)
#define TAG_SHFT 8
#define BLK_SHFT 2

// macros for indexing
#define _get_tag(address)   ((((unsigned int)address) & TAG_MASK) >> TAG_SHFT)  // get tag of address
#define _get_block(address) ((((unsigned int)address) & BLK_MASK) >> BLK_SHFT)  // get cachline from address
#define _get_idx(address)   (((unsigned int)address) & IDX_MASK)                // get word idx in cache
#define alligned(address)   (((unsigned int)address) - (((unsigned int)address) % BLK_SIZE))    // get first address in the block


// mesi states
#define Invalid   0
#define Shared    1
#define Exclusive 2
#define Modified  3

// mesi ops
#define BusNOP   0
#define BusRd    1
#define BusRdX   2
#define BusFlush 3

//mesi origins
#define core0 0
#define core1 1
#define core2 2
#define core3 3 
#define main_mem  4

// memory 
#define mem_size 1048576        //  in words
#define memory_latency 16

// Bus states
#define start 0
#define memory_wait 1
#define flush 2

// macros mesi behaviour
#define _cache_handled(handler) (handler < 4)
#define inc_positive(time) ((time >= 0) ? (time + 1) : time)
#define _cache_on_bus(idx) (last_time_served[idx] = cycle)
#define time_diff(time_a, time_b) (time_a - time_b)
/* ************************** Structures *************************** */

typedef struct bus_request{
    int cmd;                    // command type
    int addr;                   // requested address
    int data;                   // data
    int id;
} bus_request;
typedef bus_request* bus_request_p; 

typedef struct cache{
    int cache_data[WORDS];      // the actual data stored by cachelines (word)
    int tags[BLOCKS];           // the tags of the stored blocks 
    int mesi_state[BLOCKS];     // the mesi status of the stored blocks
    int idx;                    // serial id of cache instance
    int busy;                   // a flag used by cache to indicate that the cache request is being processed.
    bus_request* next_req;      // store the request that will be send on MESI BUS
} cache ;
typedef cache* cache_p;

typedef struct response{
    int handler;
    int requestor;
    int copyed;
    int cmd;
} response;

typedef struct mesi_bus{
    int state;          // for implemetation
    int origin;         // the origin of transaction
    int cmd;            // type of transaction
    int addr;           // address
    int data;           // data
    int shared;         // Shared state indicator
    int req_id;         // request_id
    response* resp;     // used for flush
} mesi_bus;
typedef mesi_bus* mesi_bus_p;

typedef struct main_memory{
    int data[mem_size];
    int latency;
} main_memory;
typedef main_memory* main_memory_p;




/* *********************** static variables ************************ */

static int cycle = 0 ;

static int cache_idx = 0;                 //static idx for cache id
static int request_id = 0;          // each request will have unique id
static cache_p CACHES[CACHE_COUNT]; // array of caches used by MESI

static main_memory_p Memory = NULL;         // main memmory
static mesi_bus_p Bus = NULL;           // the main mesi bus

static bus_request_p pending_req[CACHE_COUNT] = {NULL, NULL, NULL, NULL}; // all the pending requests
static int last_time_served[CACHE_COUNT] = {0};                           // array for use of round robin
static int waited_cycles;                                                 // counter when accessing main memory;
 
static inline int compilation_crap(int x) {return (cache_idx + request_id + cycle + waited_cycles +\
    last_time_served[x%4] + CACHES[0]->busy + Memory->latency + (pending_req[0] == NULL) + Bus->state );}



/* ******************* cache functions headers ******************** */

// call when on an instace upon creation.
void init_cache(cache_p cache);

// check if address is cached, return HIT or MISS
int query(int address, cache_p cache, int mode);

// read word from cache to value in *dest_reg. if MISS, fetch it using messi and stall
int read_word(int address, cache_p cache, int* dest_reg);

// write data to cache. if MISS, fetched it throug messi and stall
int write_word(int address, cache_p cache, int* src_reg);


/* ******************* MESI functions headers ******************** */

// call upon loding request on the bus
void clear_request_from_cahce(int c);

// looad a transaction on the bus (request)
void generate_transaction(bus_request_p request);

int is_shared(int requestor, int address);

// return mode for BusRd (Shared / Exclusive)
int BusRd_mode(int address, cache_p requestor);

// determain the handler of the request: if (stored & modified) handler = cache.
void set_handler(int address);

// load a request to the mesi bus
void load_request();

// first step of the request life-cycle
void kick_mesi();

// it's pretty sraight forward
void wait_for_response();

// response for the reqiest
void flushing();

// go over caches, and invalidate the data if needed, skip given caches
void invalidate_caches(int client, int provider, int block_idx);

// perform memory copy on flush when request was Rd
void snoop_Rd(int handler, int client);

// perform memory copy on flush when request was RdX
void snoop_RdX(int handler, int client);

//snoop the line
void snoop();

// get the next core to serve
int next_core_to_serve();

//get the next request
bus_request_p get_next_request();

// manage transaction over messi using state machine
void mesi_state_machine();


/* ************************ Utility Functios  ************************* */

void load_mem_manually();

void print_cache(FILE* file_w, cache_p cache);

void print_bus();


/* ******************* Debugging functions headers ******************** */
/*  
    Providing a no-coherncy cach system
    to check cache behaviour without mesi
*/

int non_mesi_query(int address, cache_p cache);

int non_mesi_read(int address, cache_p cache, int* dest_reg);

int non_mesi_write(int address, cache_p cache, int* src_reg);

void fetch_block_immediate(int address, cache_p cache);

void load_mem_manually();

#endif

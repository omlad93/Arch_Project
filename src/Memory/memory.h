#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef MEMORY_H
#define MEMORY_H

/* ******************************************************************** */
/* ******************    Memory Macro Definitions    ****************** */
/* ******************        Used in memory.c        ****************** */
/* ******************************************************************** */

// cache parameters
#define CACHE_COUNT 4
#define HIT 1
#define MISS -1
#define WORDS 256
#define BLOCKS 64
#define BLK_SIZE 4
#define LAST_CPY 3 // BLK_SIZE - 1

// masks and shifts
#define MEM_MASK 0x000FFFFF // 20 bits (19:0)
#define TAG_MASK 0x000FFF00 // 12 bits (19:8) 
#define BLK_MASK 0x000000FC // 6 bits  (7:3)
#define OST_MASK 0x00000003 // 2 bits  (3:0)
#define IDX_MASK 0x000000FF // 8 bits  (7:0)
#define TAG_SHFT 8
#define BLK_SHFT 2

// macros for indexing
#define _get_tag(address)   ((((unsigned int)address) & TAG_MASK) >> TAG_SHFT)  // get tag of address
#define _get_block(address) ((((unsigned int)address) & BLK_MASK) >> BLK_SHFT)  // get cachline from address
#define _get_idx(address)   (((unsigned int)address) & IDX_MASK)                // get word idx in cache
#define alligned(address)   (((unsigned int)address) - (((unsigned int)address) % BLK_SIZE))    // get first address in the block
#define construct_address(tag,idx)   (alligned( ((tag << TAG_SHFT) | (idx)) ) )

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
#define dirty_evict 3

/* ******************************************************************** */
/* ******************     Memory Macro Functions     ****************** */
/* ******************        Used in memory.c        ****************** */
/* ******************************************************************** */

#define _cache_handled(handler) (handler < 4)
#define inc_positive(time) ((time >= 0) ? (time + 1) : time)
#define _cache_on_bus(idx) (last_time_served[idx] = cycle)
#define time_diff(time_a, time_b) (time_a - time_b)
#define is_hit(st) (st == HIT)
#define is_miss(st) (st == MISS)
#define evict_first(c) ( (pending_evc[c] != NULL) & (pending_req[c] != NULL) )
#define need_to_evict(blk, c) (c->mesi_state[blk] == Modified)


/* ******************************************************************** */
/* ******************    Memory System Structures    ****************** */
/* ******************        Used in memory.c        ****************** */
/* ******************************************************************** */

// A struct implentation of a cache request from bus
typedef struct bus_request{
    int cmd;                    // command type
    int addr;                   // requested address
    int data;                   // data
    int id;
} bus_request;
typedef bus_request* bus_request_p; // pointer to bus_request

// A struct implentation of a Cache
typedef struct cache{
    int cache_data[WORDS];      // the actual data stored by cachelines (word)
    int tags[BLOCKS];           // the tags of the stored blocks 
    int mesi_state[BLOCKS];     // the mesi status of the stored blocks
    int idx;                    // serial id of cache instance
    int busy;                   // a flag used by cache to indicate that the cache request is being processed.
    bus_request_p next_req;     // store the request that will be send on MESI BUS
    bus_request_p next_evict;   // store evict if such is needed
} cache ;
typedef cache* cache_p; // pointer to cache

// A Virtual data for simulation MESI: to simplify snooping
typedef struct response{
    int handler;                // handler of current request
    int requestor;              // origin of current request
    int copyed;                 // counter of transaction in current request
    int cmd;                    // current request type (to remember when flushing)
} response;
typedef response* response_p; // pointer to response

// A struct for implentation of MESI bus with additional programming assistance
typedef struct mesi_bus{
    int state;                  // for implemetation
    int origin;                 // the origin of transaction
    int cmd;                    // type of transaction
    int addr;                   // address
    int data;                   // data
    int shared;                 // Shared state indicator
    int req_id;                 // request_id (for debugging)
    response_p resp;            // used for flush
} mesi_bus;
typedef mesi_bus* mesi_bus_p; // pointer to mesi_bus

// A struct for implentation of main memory
typedef struct main_memory{
    int data[mem_size];
    int latency;
} main_memory;
typedef main_memory* main_memory_p; // pointer to main_memory



/* ******************************************************************** */
/* ****************** Memory System Static Variables ****************** */
/* ******************        Used in memory.c        ****************** */
/* ******************************************************************** */
static int cycle = 1 ;

static int cache_idx = 0;                                                 //static idx for cache id
static int request_id = 0;                                                // each request will have unique id
static cache_p CACHES[CACHE_COUNT];                                       // array of caches used by MESI

static main_memory_p Memory = NULL;                                       // main memmory
static mesi_bus_p Bus = NULL;                                             // the main mesi bus

static bus_request_p pending_req[CACHE_COUNT] = {NULL, NULL, NULL, NULL}; // all the pending requests
static bus_request_p pending_evc[CACHE_COUNT] = {NULL, NULL, NULL, NULL}; // all the pending evicts
static int last_time_served[CACHE_COUNT] = {0};                           // array for use of round robin
static int waited_cycles;                                                 // counter when accessing main memory;
 

// Use static Variables to allow strict compilation :)
static inline int compilation_crap(int x) {return (cache_idx + request_id + cycle + waited_cycles +\
    last_time_served[x%4] + CACHES[0]->busy + Memory->latency + (pending_req[0] == NULL) + Bus->state + (pending_evc[1] != NULL) );}


/* ******************************************************************** */
/* ******************     Functions Declerations     ****************** */
/* ******************     Implemeted in memory.c     ****************** */
/* ******************************************************************** */



/* ******************       Utility Functios        ******************* */
/* ****************** Use for Set & Monitor System  ******************* */


// Allocate cache fields
void init_cache(cache_p cache);

// Allocate memory strict & MESI bus
void initiate_memory_system();

// free memory struct & MESI bus
void close_memory_system();

// free cache fields & pointer
void release_cache(cache_p cache);


// TODO: I/O functions (Read, Monitor, Store)

/* ******************    Core - Memory Interface    ******************* */
/* ****************** Use for Load / Store opcodes  ******************* */


// check if address is cached, return HIT or MISS
// for BusRd - if data is valid it's enough
// for BusRdX - data has to be in M state
int query(int address, cache_p cache, int mode);

// read word from cache. if MISS, fetched it throug messi and stall
int read_word(int address, cache_p cache, int* dest_reg);

// write data to cache. if MISS, fetched it throug messi and stall
int write_word(int address, cache_p cache, int* src_reg);



/* ******************    Cache -> MESI Interface    ******************* */
/* ****************** Use to assure coherncy in mem ******************* */


// call upon loding request on the bus
void clear_request_from_cahce(int c);

// looad a transaction on the bus (request)
void generate_transaction(bus_request_p request);

// check if data is also cached in other caches
int is_shared(int requestor, int address);

// determain the handler of the request: if (stored & modified) handler = cache.
void set_handler(int address);

// call when a request is loaded on the bus
void kick_mesi();

// no one uses the bus if waiting for memory
void wait_for_response();

// transfer request over the line
void flushing();

// go over caches, and invalidate the data if needed, skip given caches
void invalidate_caches(int client, int provider, int block_idx);

// perform memory copy on flush when request was Rd
void snoop_Rd(int handler, int client);

// perform memory copy on flush when request was RdX
void snoop_RdX(int handler, int client);

// perform memory copy on evict
void evict();

//snoop the line on flush: modify memory elements
void snoop();

// get the next core to serve
int next_core_to_serve();

// chose next command on bus (used on dirty evict)
bus_request_p get_request_per_cache(int cache_idx);

void clear_old_evicts();

// get longest request waiting
bus_request_p get_next_request();

// manage transaction over messi using state machine
void mesi_state_machine();


/* ******************      Debugging functions      ****************** */
/* ****************** Used for Memory verifications ****************** */

// Query in single core mode
int non_mesi_query(int address, cache_p cache);

// Read in single core mode
int non_mesi_read(int address, cache_p cache, int* dest_reg);

// Write in single core mode
int non_mesi_write(int address, cache_p cache, int* src_reg);

// Copy block of data from memory (1 cycle)
void fetch_block_immediate(int alligned_address, cache_p cache);

// Initate memory with values: MEM[i] = i
void load_mem_manually();

// Initate memory with values: in memin.txt manually
void load_mem_manually_for_core_debug();

// Print cache to file, as a table with fields
void print_cache(FILE* file_w, cache_p cache);

// Print mem to file, as a table with fields
void print_mem(FILE* file_w);

// Print Bus status
void print_bus();

#endif
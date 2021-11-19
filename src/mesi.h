#ifndef MESI_H
#define MESI_H
#include "cache.h"

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
#define main  4

// memory 
#define mem_size 1048576        //  in words
#define memory_latency 16

// Bus states
#define idle 0
#define start 1
#define memory_wait 2
#define flush 3

static int cycle = 0 ;

#define _cache_handled(handler) (handler < 4)
#define inc_positive(time) ((time >= 0) ? (time + 1) : time)
#define _cache_on_bus(idx) (last_time_served[idx] = cycle)
#define clear_request_from_cahce(c) (pending_req[c] = NULL; _cache_on_bus(c))
#define time_diff(time_a, time_b) (time_a - time_b)


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

typedef struct main_memory{
    int data[mem_size];
    int latency = memory_latency;
} main_memory;


static main_memory* Memory;
static mesi_bus*    Bus;

// round robin for requests
static bus_request* pending_req[CACHE_COUNT] = {NULL, NULL, NULL, NULL}; 
static int last_time_served[CACHE_COUNT] = {0};


static int waited_cycles; // counter when accessing main memory;

int generate_transaction(cache* requestor);

int is_shared(int requestor, int address);

// return mode for BusRd (Shared / Exclusive)
int BusRd_mode(int address, cache* requestor);

// determain the handler of the request: if (stored & modified) handler = cache.
void set_handler(int address);

// chose a request to handle from requests array
int round_robin(mesi_bus* bus);

// load a request to the mesi bus
void load_request();

// first step of the request life-cycle
void kick_mesi();

// it's pretty sraight forward
void wait_for_response();

// response for the reqiest
void flushing();

// go over caches, and invalidate the data if needed
void invalidate_caches(int client, int block_idx);

// go over caches, and invalidate the data if needed
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
bus_request* get_next_request();

// manage transaction over messi using state machine
int mesi_state_machine();
#endif
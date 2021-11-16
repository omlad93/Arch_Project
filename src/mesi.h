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

#define _cache_handled(handler) (handler < 4)

typedef struct response{
    int handler;
    int requestor;
    int copyed;
} response;

typedef struct mesi_bus{
    int state;          // for implemetation
    int origin;         // the origin of transaction
    int cmd;            // type of transaction
    int addr;           // address
    int data;           // data
    int shared;         // Shared state indicator
    response* resp;     // used for flush
} mesi_bus;

typedef struct main_memory{
    int data[mem_size];
    int latency = memory_latency;
} main_memory;


static main_memory* Memory;
static mesi_bus*    Bus;
static int requests[CACHE_COUNT] = {0}; 
static int waited_cycles;

int generate_transaction(cache* requestor);

int nop();

int flush(int alligned_address, cache* cache);

// Invalidate all other caches instances of block
int invalidate(int block, cache* invalidator);

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


#endif
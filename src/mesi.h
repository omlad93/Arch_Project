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
#define mem_size 1048576 //  in words
#define cycle_count 16

// Bus states
#define empty 0
#define kick 1
#define busy 2
#define finish 3


#define _cache_handled(handler) (handler <4)


typedef struct mesi_bus{
    int state;
    int origin;
    int cmd;
    int addr;
    int data;
    int shared;
} mesi_bus;

typedef struct main_memory{
    int data[mem_size];
    int cycle;
} main_memory;


static main_memory* Memory;
static mesi_bus*    Bus;
static int requests[CACHE_COUNT] = {0}; 
static int duration, so_far;


int read(int alligned_address, cache* cache);

int read_ex(int alligned_address, cache* cache);

int generate_transaction(cache* requestor);

int nop();

int flush(int alligned_address, cache* cache);

int invalidate(int block, cache* invalidator);

int read_mode(int address, cache* requestor);

int better_version_cached(int block);

int round_robin(mesi_bus* bus);

#endif
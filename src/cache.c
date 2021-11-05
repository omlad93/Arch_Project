#include "cache.h"

int hit(int address, cache* cache);

int read_word(int address, int* ack, cache* cache);

int write_word(int address, int* ack, cache* cache);

int fetch_block(int alligned_address); //request a block when a miss occur
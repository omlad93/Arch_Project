

typedef struct cahce{
    int x;
} cache;

int read_word(int address, int* ack, cache* cache);

int write_word(int address, int* ack, cache* cache);


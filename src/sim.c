#include "memory.h"

//memory parameters
#define word 32
#

typedef struct simulator {
    main_memory main_memory = 
}

//initiate the main memory 
void init_main_memory(char* memin) {
    char line[word];
    int curr_line = 0;
    FILE* memin_file = fopen(memin,"r");
    if (memin_file == NULL) {
		fprintf(stderr, "Can't open memin file \n");
		exit(1);
	}

    while (fgets(line, 32, memin_file) != NULL) {
		memory[curr_line] = strtoul(line, NULL, 16);
		curr_line++;
	}
	while (curr_line < MEMSIZE) {//in case the file we got does not have all the 0 lines
		memory[curr_line] = 0;
		curr_line++;
	}
	fclose(dmemin);
}
}
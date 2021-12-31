#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef FILES_FH
#define FILES_FH


typedef struct sim_files{
    FILE *imem0;
    FILE *imem1;
    FILE *imem2;
    FILE *imem3;
    FILE *memin;
    FILE *memout;
    FILE *regout0;
    FILE *regout1;
    FILE *regout2;
    FILE *regout3;
    FILE *core0trace;
    FILE *core1trace;
    FILE *core2trace;
    FILE *core3trace; 
    FILE *bustrace; 
    FILE *dsram0; 
    FILE *dsram1; 
    FILE *dsram2; 
    FILE *dsram3; 
    FILE *tsram0; 
    FILE *tsram1; 
    FILE *tsram2; 
    FILE *tsram3;
    FILE *stats0;
    FILE *stats1;
    FILE *stats2;
    FILE *stats3;
}sim_files;
typedef sim_files* sim_files_p;

// sim_files_p files;


sim_files_p init_files_def(sim_files_p files);
sim_files_p init_files(sim_files_p files, char* argv[]);

#endif
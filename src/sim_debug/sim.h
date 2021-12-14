#include "core.h"
#include "files.h"
#include <stdio.h>
#ifndef SIM_H
#define SIM_H

sim_files_p files;

//init declarations 
void init_cores(single_core** cores);
void init_main_memory(FILE *memin); 
void init_cores_done(int **cores_done);
int main(int argc, char* argv[]);

// other function declarations 
void init_main_memory(FILE *memin);

#endif
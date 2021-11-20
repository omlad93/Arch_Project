#include "memory.h"
#include "core.h"


//memory parameters
#define word 32

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


typedef struct cpu{
    main_memory_p main_memory ;
    single_core *core_0 ;
    single_core *core_1 ;
    single_core *core_2 ;
    single_core *core_3 ;
    mesi_bus_p mesi_bus;
    sim_files_p files;
} cpu;
typedef cpu* cpu_p;

//init declarations 
void init_files_def(sim_files_p files);
void init_files(sim_files_p files, char** argv[]);

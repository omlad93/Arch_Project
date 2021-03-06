#define _CRT_SECURE_NO_WARNINGS
#include "files.h"


//called when sim.exe got no parameters
sim_files_p init_files_def(sim_files_p files){
    files = (sim_files_p)(calloc(sizeof(sim_files),1));
    files->imem0 = fopen("imem0.txt" ,"r") ;
    files->imem1 = fopen("imem1.txt" ,"r") ;
    files->imem2 = fopen("imem2.txt" ,"r") ;
    files->imem3 = fopen("imem3.txt" ,"r") ;
    files->memin = fopen("memin.txt" ,"r") ;
    files->memout = fopen("memout.txt" ,"w") ;
    files->regout0 = fopen("regout0.txt" ,"w") ;
    files->regout1 = fopen("regout1.txt" ,"w") ;
    files->regout2 = fopen("regout2.txt" ,"w") ;
    files->regout3 = fopen("regout3.txt" ,"w") ;
    files->core0trace = fopen("core0trace.txt" ,"w") ;
    files->core1trace = fopen("core1trace.txt" ,"w") ;
    files->core2trace = fopen("core2trace.txt" ,"w") ;
    files->core3trace = fopen("core3trace.txt" ,"w") ;
    files->bustrace = fopen("bustrace.txt" ,"w") ;
    files->dsram0 = fopen("dsram0.txt" ,"w") ;
    files->dsram1 = fopen("dsram1.txt" ,"w") ;
    files->dsram2 = fopen("dsram2.txt" ,"w") ;
    files->dsram3 = fopen("dsram3.txt" ,"w") ;
    files->tsram0 = fopen("tsram0.txt" ,"w") ;
    files->tsram1 = fopen("tsram1.txt" ,"w") ;
    files->tsram2 = fopen("tsram2.txt" ,"w") ;
    files->tsram3 = fopen("tsram3.txt" ,"w") ;
    files->stats0 = fopen("stats0.txt" ,"w") ;
    files->stats1 = fopen("stats1.txt" ,"w") ;
    files->stats2 = fopen("stats2.txt" ,"w") ;
    files->stats3 = fopen("stats3.txt" ,"w") ;
    return files;

    
}

// opens files when start to run 
sim_files_p init_files(sim_files_p files, char* argv[]){
    files = (sim_files_p)(calloc(sizeof(sim_files),1));
    files->imem0 = fopen(argv[1] ,"r") ;
    files->imem1 = fopen(argv[2] ,"r") ;
    files->imem2 = fopen(argv[3] ,"r") ;
    files->imem3 = fopen(argv[4] ,"r") ;
    files->memin = fopen(argv[5] ,"r") ;
    files->memout = fopen(argv[6] ,"w") ;
    files->regout0 = fopen(argv[7] ,"w") ;
    files->regout1 = fopen(argv[8] ,"w") ;
    files->regout2 = fopen(argv[9] ,"w") ;
    files->regout3 = fopen(argv[10] ,"w") ;
    files->core0trace = fopen(argv[11] ,"w") ;
    files->core1trace = fopen(argv[12] ,"w") ;
    files->core2trace = fopen(argv[13] ,"w") ;
    files->core3trace = fopen(argv[14] ,"w") ;
    files->bustrace = fopen(argv[15] ,"w") ;
    files->dsram0 = fopen(argv[16] ,"w") ;
    files->dsram1 = fopen(argv[17] ,"w") ;
    files->dsram2 = fopen(argv[18] ,"w") ;
    files->dsram3 = fopen(argv[19],"w") ;
    files->tsram0 = fopen(argv[20],"w") ;
    files->tsram1 = fopen(argv[21] ,"w") ;
    files->tsram2 = fopen(argv[22] ,"w") ;
    files->tsram3 = fopen(argv[23] ,"w") ;
    files->stats0 = fopen(argv[24] ,"w") ;
    files->stats1 = fopen(argv[25] ,"w") ;
    files->stats2 = fopen(argv[26] ,"w") ;
    files->stats3 = fopen(argv[27] ,"w") ;
    return files;
}


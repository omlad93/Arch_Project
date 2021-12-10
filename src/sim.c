#include "sim.h"

// Check if the files exists any way and just needs the parametetrs because there wasnt on the argv 
// or I understand right and we need to open them from scratch 

//called when sim.exe got no parameters
void init_files_def(sim_files_p files){
    
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

    
}

// opens files when start to run 
void init_files(sim_files_p files, char** argv[]){
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
}


// initiate the 4 cores - cores declaration in in sim.h
void init_cores(single_core** cores){
    
    single_core *c0 = (single_core*)calloc(1, sizeof(single_core));
    init_core(files->core0trace, files->imem0, c0, 0);
    single_core *c1 = (single_core*)calloc(1, sizeof(single_core));
    init_core(files->core1trace, files->imem1, c1, 1);
    single_core *c2 = (single_core*)calloc(1, sizeof(single_core));
    init_core(files->core2trace, files->imem2, c2, 2);
    single_core *c3 = (single_core*)calloc(1, sizeof(single_core));
    init_core(files->core3trace, files->imem0, c3, 3);
}
    

// initiate main memory function using memin.txt
void init_main_memory(FILE *memin){
  int j, temp;
  if(memin == NULL){
    printf("Failed open memin.txt\n");
  }
  j = 0;
  while(!feof(memin)){
    fscanf(memin, "%08x", &(temp));
    Memory->data[j] = temp;
    j++;
  }
  while (j < 1048576) {
		Memory->data[j] = 0;
		j++;
	}
  printf("Finished loading main memory\n");
}

void init_cores_done(int **cores_done){
    for (int i = 0; i < cores_count; i++)
    {
        int *halt = (int*)calloc(1, sizeof(int));
        *halt = 0 ;
        cores_done[i] = halt ;
    }
}

// function to write the main memory to memout file 
void write_memout(FILE *memout){
    int j, temp;
    if(memout == NULL){
        printf("Failed open memout.txt\n");
    }
    j = 0;
    while (j < mem_size) {
        fprintf(memout, Memory->data[j]);
	    j++;
	    }
    fclose(memout);
    printf("Finished writing memory to memout\n");
}



int main(int argc, char* argv[]) {
    int **cores_done[cores_count];
    int clock_cycle = 0 ;
    if(argc == 27){
        init_files(files, argv);
    } else { 
        init_files_def(files);
    }

    init_cores_done(cores_done);
    init_cores(cores);
    initiate_memory_system();
    init_main_memory(files->memin);
    
    while( *cores_done[0] == 0 || *cores_done[1] == 0 || *cores_done[2] == 0 || *cores_done[3] == 0){
        for (int i = 0; i < cores_count; i++)
        {
            if(*cores_done[i] == 0){
                simulate_clock_cycle(i, clock_cycle, cores_done[i]);
            }
        }
        mesi_state_machine(); //check with omri about cyc++
        clock_cycle ++ ;
    }
    write_memout(files->memout);
}

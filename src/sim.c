#include "sim.h"
// Check if the files exists any way and just needs the parametetrs because there wasnt on the argv 
// or I understand right and we need to open them from scratch 


// initiate the 4 cores - cores declaration in in sim.h
void init_cores(single_core** cores){
    
    single_core *c0 = (single_core*)calloc(1, sizeof(single_core));
    init_core(files->core0trace, files->imem0, c0, 0);
    single_core *c1 = (single_core*)calloc(1, sizeof(single_core));
    init_core(files->core1trace, files->imem1, c1, 1);
    single_core *c2 = (single_core*)calloc(1, sizeof(single_core));
    init_core(files->core2trace, files->imem2, c2, 2);
    single_core *c3 = (single_core*)calloc(1, sizeof(single_core));
    init_core(files->core3trace, files->imem3, c3, 3);
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

void init_cores_done(int** cores_done){
    for (int i = 0; i < cores_count; i++)
    {
        int *halt = (int*)calloc(1, sizeof(int));
        *halt = 0 ;
        cores_done[i] = halt ;
    }
}

void print_all_regs(){
    print_regs(cores[0], files->regout0);
    print_regs(cores[1], files->regout1);
    print_regs(cores[2], files->regout2);
    print_regs(cores[3], files->regout3);
}

void print_all_stats(){
    print_stats(cores[0], files->stats0);
    print_stats(cores[1], files->stats1);
    print_stats(cores[2], files->stats2);
    print_stats(cores[3], files->stats3);
}

int main(int argc, char* argv[]) {
    int *cores_done[cores_count];
    int clock_cycle = 0 ;
    
    if(argc == 27){
        files = init_files(files, argv);
    } else { 
        files = init_files_def(files);
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
       
        mesi_state_machine(files, clock_cycle); //check with omri about cyc++
        clock_cycle ++ ;
    }
    // write_memout(files->memout);

    print_all_regs();
    print_all_stats();
    dump_memory(files);
}

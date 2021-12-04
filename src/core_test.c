
#include "core.h"

//extern Memory;

// read memory function
void read_memory(){
  FILE *mem_f;
  int j, temp;
  mem_f = fopen("memin.txt", "r");
  if(mem_f == NULL){
    printf("Failed open memin.txt\n");
  }
  j = 0;
  while(!feof(mem_f)){
    fscanf(mem_f, "%08x", &(temp));
    printf( "DBG5\n");
    Memory->data[j] = temp;
    j++;
  }
  printf( "DBG4\n");
  while (j < 1048576) {
		Memory->data[j] = 0;
		j++;
	}
  printf("Finished leading memory\n");
}


int main(int argc, char* argv[]) {
    FILE *trace, *imem;
    int *halt;
    int clock_cycle;

    printf("\tRunning Simulator\n");

    single_core *core = (single_core*)calloc(1, sizeof(single_core));
    halt = (int*)calloc(1, sizeof(int));
    *halt = 0;

    trace = fopen("trace3.txt","w");
    imem = fopen("imem3.txt","r");

    if(trace == NULL || imem == NULL){
		printf( "Error opening files");
		exit(3);
    }
    init_core(trace, imem, core, 0);
    initiate_memory_system();
    //read_memory();
    load_mem_manually_for_core_debug();
    /*for(int clock_cycle = 0; clock_cycle < 30; clock_cycle++){
      simulate_clock_cycle(0, clock_cycle, halt);
    }*/
    clock_cycle = 0;
    while(!(*halt)){
      simulate_clock_cycle(0, clock_cycle, halt);
      clock_cycle++;
    }
    
    printf("\tFinished simulation\n");

}
#include "memory.h"

/* ***************** Main Memory Function - inner debug  ****************** */


int main(int argc, char* argv[]){
    FILE* cache_0 = fopen("data_c0.txt","w");
    FILE* cache_1 = fopen("data_c1.txt","w");
    FILE* cache_2 = fopen("data_c2.txt","w");
    FILE* cache_3 = fopen("data_c3.txt","w");
    FILE* memd = fopen("data_mem.txt","w");


    cache_p c0 = calloc(1,sizeof(cache)); init_cache(c0);
    cache_p c1 = calloc(1,sizeof(cache)); init_cache(c1);
    cache_p c2 = calloc(1,sizeof(cache)); init_cache(c2);
    cache_p c3 = calloc(1,sizeof(cache)); init_cache(c3);
    
    
    initiate_memory_system();

    printf("\n\tCache Debug Main Function: %s\n", argv[argc-1]);

    int run=1, counter =0 ,word=4, offset, alligned_address;

    int st0 = MISS,st1 = MISS,st2 = MISS,st3 = MISS;
    int rv0, wv1 = 461993, rv2, wv3=8057775;


    load_mem_manually();
    while(run){
        offset= word %4;
        alligned_address = word - offset;
        if (is_miss(st0)){ st0 = read_word (alligned_address   ,c0, &rv0); }
        if (is_miss(st1)){ st1 = write_word(alligned_address+1 ,c1, &wv1); }
        if (is_miss(st2)){ st2 = read_word (alligned_address+1 ,c2, &rv2); }
        if (is_miss(st3)){ st3 = write_word(alligned_address+3 ,c3, &wv3); }
        mesi_state_machine();
        run = (counter < 1024);
        counter ++;
    }
    printf("\t >> loop1 finished (@%i) \n",counter);
    word = 10; run =1; wv1 = 11111111; wv3 = 13131313; counter =0 ; 
    st0 = MISS;st1 = MISS; st2 = MISS; st3 = MISS;
    while(run){
        if (is_miss(st0)){ st0 = read_word (word   ,c0, &rv0); }
        if (is_miss(st1)){ st1 = write_word(word+1 ,c1, &wv1); }
        if (is_miss(st2)){ st2 = read_word (word+1 ,c2, &rv2); }
        if (is_miss(st3)){ st3 = write_word(word+3 ,c3, &wv3); }
        mesi_state_machine();
        run = (counter < 1024);
        counter ++;
    }
    printf("\t >> loop2 : finished (@%i) \n",counter);

    word = 260; run =1; wv1 = 02611620; wv3 = 02633620; counter =0 ; 
    st0 = MISS;st1 = MISS; st2 = MISS; st3 = MISS;
    while(run){
        if (is_miss(st0)){ st0 = read_word (4    ,c0,   &rv0); }
        if (is_miss(st1)){ st1 = read_word (word    ,c3,   &rv0); }
        // if (is_miss(st1)){ st1 = write_word(word+1  ,c3,  &wv1); }
        // if (is_miss(st2)){ st2 = write_word (word+1 ,c3,  &counter); }
        // if (is_miss(st3)){ st3 = write_word(word+3  ,c3,  &wv3); }
        mesi_state_machine();
        run = (counter < 1024);
        counter ++;
    }
    printf("\t >> loop3 : finished (@%i) \n",counter);

//    if (rv2 != wv1) {
//         printf("read @%.5x should have been %.8i but recived %.8i", alligned_address+1, wv1, rv2);
//     }
    printf("\t >> took %i cycles\n", cycle);
    print_cache(cache_0,c0); print_cache(cache_1,c1); 
    print_cache(cache_2,c2); print_cache(cache_3,c3);
    print_mem(memd);
    fclose(cache_0); fclose(cache_1); fclose(cache_2); fclose(cache_3);
    FILE* ds = fopen("dsram.txt","w");
    FILE* ts = fopen("tsram.txt","w");
    dump_cache(c0,ds,ts);
    fclose(ds); fclose(ts);
    close_memory_system();




    printf("\t [V]: Finished");
}

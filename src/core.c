
#include "core.h"


void init_core_for_debug(FILE* trace_file, single_core* core){
    core->pc = 0;
    core->is_halt = 0;
    int j = 0;
        while (j < MEM_SIZE) {
		core->Op_Mem[j] = 0;
		j++;
	}
    core->trace_file = trace_file;
    core->Cache = (cache*)calloc(1, sizeof(cache));
    init_cache(core->Cache);
    core->IF_op = (operation*)calloc(1, sizeof(operation));
    core->ID_op = (operation*)calloc(1, sizeof(operation));
    core->EX_op = (operation*)calloc(1, sizeof(operation));
    core->MEM_op = (operation*)calloc(1, sizeof(operation));
    core->WB_op = (operation*)calloc(1, sizeof(operation));
    for(int i = 0; i < 16; i++){
        core->Reg_File[i] = 0;
    }
}

void init_operation(operation* op){
    op->addr = 0;
    op->code = -1;
    op->empty = 1;
    op->inst = 0;
    op->op_pc = 0;
    op->rd = -1;
    op->rs = -1;
    op->rt = -1;
}

void init_core_stats(core_stats* stats){
    stats->cycles = 0;
    stats->decode_stall = 0;
    stats->mem_stall = 0;
}

void init_core(FILE* trace_file, FILE* imem, single_core* core, int core_num){
    core->pc = 0;
    core->next_pc = 0;
    core->is_halt = 0;
    core->data_hazzard = 0;
    core->prev_cache_miss = 0;
    core->prev_mem_inst_pc = -1;
    read_imem(imem, core);
    core->trace_file = trace_file;
    core->Cache = (cache*)calloc(1, sizeof(cache));
    core->core_stats = (core_stats*)calloc(1,sizeof(core_stats));
    init_core_stats(core->core_stats);
    init_cache(core->Cache);
    core->IF_op = (operation*)calloc(1, sizeof(operation));
    init_operation(core->IF_op);
    core->ID_op = (operation*)calloc(1, sizeof(operation));
    init_operation(core->ID_op);
    core->EX_op = (operation*)calloc(1, sizeof(operation));
    init_operation(core->EX_op);
    core->MEM_op = (operation*)calloc(1, sizeof(operation));
    init_operation(core->MEM_op);
    core->WB_op = (operation*)calloc(1, sizeof(operation));
    init_operation(core->WB_op);
    for(int i = 0; i < 16; i++){
        core->Reg_File[i] = 0;
    }
    cores[core_num] = core;
}

void read_imem(FILE* imem, single_core* core){
    int j = 0;
    while(!feof(imem)){
        fscanf(imem, "%08x", &(core->Op_Mem[j]));
        j++;
    }
    while (j < MEM_SIZE) {
		core->Op_Mem[j] = 0;
		j++;
	}

}

/*checks if its a jump or sw operatoin -> in that case rd isn't relevant for data hazard detection*/
int is_relevant_rd(operation* op){
    if((op->code != SW) && (op->code != BEQ) && (op->code != BNE) && (op->code != BLT) && (op->code != BJT) && (op->code !=BLE) && (op->code != BGE) && (op->code != JAL)){
        return 1;
    }
    else{
        return 0;
    }

}

int detect_data_hazzard(single_core* core){
    if(core->ID_op->code == SW){
        if(((core->ID_op->rd == core->EX_op->rd) && (core->EX_op->empty != 1) && (core->EX_op->rd != 0)  && (core->EX_op->rd != 1) && is_relevant_rd(core->EX_op)) ||
           ((core->ID_op->rd == core->MEM_op->rd) && (core->MEM_op->empty != 1)  && (core->MEM_op->rd != 0) && (core->MEM_op->rd != 1) && is_relevant_rd(core->MEM_op))|| 
           ((core->ID_op->rd == core->WB_op->rd) && (core->WB_op->empty != 1) && (core->WB_op->rd != 0) && (core->WB_op->rd != 1) && is_relevant_rd(core->WB_op)) ||
           ((core->ID_op->rs == core->EX_op->rd) && (core->EX_op->empty != 1) && (core->EX_op->rd != 0)  && (core->EX_op->rd != 1) && is_relevant_rd(core->EX_op)) ||
           ((core->ID_op->rt == core->EX_op->rd) && (core->EX_op->empty != 1) && (core->EX_op->rd != 0)  && (core->EX_op->rd != 1) && is_relevant_rd(core->EX_op)) ||
           ((core->ID_op->rs == core->MEM_op->rd) && (core->MEM_op->empty != 1)  && (core->MEM_op->rd != 0) && (core->MEM_op->rd != 1) && is_relevant_rd(core->MEM_op))||
           ((core->ID_op->rt == core->MEM_op->rd) && (core->MEM_op->empty != 1)  && (core->MEM_op->rd != 0) && (core->MEM_op->rd != 1) && is_relevant_rd(core->MEM_op))||
           ((core->ID_op->rs == core->WB_op->rd) && (core->WB_op->empty != 1) && (core->WB_op->rd != 0) && (core->WB_op->rd != 1) && is_relevant_rd(core->WB_op)) ||
           ((core->ID_op->rt == core->WB_op->rd) && (core->WB_op->empty != 1) && (core->WB_op->rd != 0) && (core->WB_op->rd != 1) && is_relevant_rd(core->WB_op)) 
           && (core->ID_op->rd != 0) && (core->ID_op->rd != 1)) {
            return 1;
        }
        else{
            return 0;
        }
    }
    else if((((core->ID_op->rs == core->EX_op->rd) || (core->ID_op->rt == core->EX_op->rd)) && is_relevant_rd(core->EX_op) && (core->EX_op->rd != 0)  && (core->EX_op->rd != 1)&& (core->EX_op->empty != 1)) ||
        (((core->ID_op->rs == core->MEM_op->rd) || (core->ID_op->rt == core->MEM_op->rd))  && is_relevant_rd(core->MEM_op) && (core->MEM_op->rd != 0) && (core->MEM_op->rd != 1) && (core->MEM_op->empty != 1)) ||
        (((core->ID_op->rs == core->WB_op->rd) || (core->ID_op->rt == core->WB_op->rd)) && is_relevant_rd(core->WB_op) && (core->WB_op->rd != 0) && (core->WB_op->rd != 1) && (core->WB_op->empty != 1)) ){
            return 1;
    }

    else{
        return 0;
    }
}


void simulate_clock_cycle( int core_num, int clock_cycle, int* halt){

    int branch_taken, cache_hit, data_hazzard;

    single_core *core = cores[core_num];

    data_hazzard = 0;

    core->core_stats->cycles = clock_cycle;
    
    if(core->is_halt == 1){ // will be true only when the halt instruction in EX stage
        core->ID_op->empty = 1;
        core->IF_op->empty = 1;
    }

    cache_hit = MEM_ex(core);


    if(cache_hit != MISS || (core->prev_cache_miss != MISS && cache_hit == MISS)){

            if (!(core->is_halt)){ // Halt command wasn't received yet 
                IF_ex(core);
            }
            else{ // Halt command was received
                core->IF_op->empty = 1;
            }

            branch_taken = ID_ex(core);


            data_hazzard = detect_data_hazzard(core);

     
            EX_ex(core_num);

            print_trace(core, clock_cycle);

            *(halt) = WB_ex(core);

            if (cache_hit != MISS){
                end_clock_sycle(core, data_hazzard);
            }


            if(!(data_hazzard)){
                if(!branch_taken){
                    if (!(core->prev_cache_miss != MISS && cache_hit == MISS)){
                        core->pc++;
                    }
                }
                else{
                    core->pc = core->next_pc;
                }
            }
            if(core->prev_cache_miss != MISS && cache_hit == MISS){
                core->core_stats->mem_stall++;
            }
    }
    
    else{
        core->core_stats->mem_stall++;
        core->WB_op->empty = 1;

        print_trace(core, clock_cycle);
    }
    core->data_hazzard = data_hazzard;
    core->prev_cache_miss = cache_hit;
}

void IF_ex(single_core* core) {

    int line;

    line = core->Op_Mem[core->pc];

    core->IF_op->inst = line;
    core->IF_op->op_pc = core->pc;
    core->IF_op->empty = 0;

}

int ID_ex(single_core* core){

    int next_pc; //used only in JAL op
    int imm;
    int branch_taken = 0;

    // Decoding the registers value
    int inst = core->ID_op->inst;
    imm = (int)(inst & IMM_MASK);
    core->ID_op->code =  (inst & OPP_MASK) >> OPP_SHFT;
    core->ID_op->rd = (inst & RD_MASK) >> RD_SHFT;
    //core->ID_op->rd_val = core->Reg_File[core->ID_op->rd];
    core->ID_op->rs = (inst & RS_MASK) >> RS_SHFT;
    if(core->ID_op->rs == 1){
        core->ID_op->rs_val = imm;
    }
    else{
        core->ID_op->rs_val =  core->Reg_File[core->ID_op->rs];
    }
    core->ID_op->rt =  (inst & RT_MASK) >> RT_SHFT;
    if(core->ID_op->rt == 1){
        core->ID_op->rt_val = imm;
    }
    else{
        core->ID_op->rt_val =  core->Reg_File[core->ID_op->rt];
    }
    if(core->ID_op->rd == 1){
        core->ID_op->rd_val = imm;
    }
    else{
        core->ID_op->rd_val = core->Reg_File[core->ID_op->rd];
    }

    

    // Decoding the function
    switch (core->ID_op->code) {
	case(ADD):
		core->ID_op->op_code = &add;
        return branch_taken;
	case(SUB):
		core->ID_op->op_code =  &sub;
        return branch_taken;
	case(AND):
		core->ID_op->op_code = &and;
        return branch_taken;
	case(OR):
		core->ID_op->op_code = &or;
        return branch_taken;
	case(XOR):
		core->ID_op->op_code = &xor;
        return branch_taken;
	case(MUL):
		core->ID_op->op_code = &mul;
        return branch_taken;
	case(SLL):
		core->ID_op->op_code = &sll;
        return branch_taken;
	case(SRA):
		core->ID_op->op_code = &sra;
        return branch_taken;
	case(SRL):
		core->ID_op->op_code = &srl;
        return branch_taken;
	case(BEQ):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val == core->ID_op->rt_val){
            core->next_pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BNE):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val != core->ID_op->rt_val){
            core->next_pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BLT):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val < core->ID_op->rt_val){
            core->next_pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BJT):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val > core->ID_op->rt_val){
            core->next_pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BLE):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val <= core->ID_op->rt_val){
            core->next_pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BGE):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val >= core->ID_op->rt_val){
            core->next_pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(JAL):
		core->ID_op->op_code = &nop;
        next_pc = core->ID_op->rd_val & LSB_9BIT;
        core->ID_op->rd = 15;
        core->ID_op->rd_val = core->pc;
        core->next_pc = next_pc;
        branch_taken = 1;
        return branch_taken;
	case(LW):
		core->ID_op->op_code = &lw;
        branch_taken = 0;
        return branch_taken;
	case(SW):
		core->ID_op->op_code = &sw;
        branch_taken = 0;
        return branch_taken;
	case(HALT):
		core->ID_op->op_code = &nop;
        core->is_halt = 1;
        branch_taken = 0;
        return branch_taken;
	//if unknown opcode -> do nop
	default:
		core->ID_op->op_code = &nop;
        branch_taken = 0;
        return branch_taken;
	}


}

void EX_ex(int core_num){

    single_core *core = cores[core_num];
    if (! core->EX_op->empty){
        core->EX_op->op_code(core_num);
    }
}

int MEM_ex(single_core* core){
    int res = 0;
    if((core->MEM_op->code == LW) && (core->MEM_op->empty != 1)){
        res = read_word(core->MEM_op->addr, core->Cache, &(core->MEM_op->rd_val));
        core->prev_mem_inst_pc = core->MEM_op->op_pc;
        return res;
    }
    else if ((core->MEM_op->code == SW)  && (core->MEM_op->empty != 1)){
        res = write_word(core->MEM_op->addr, core->Cache, &(core->MEM_op->rd_val));
        core->prev_mem_inst_pc = core->MEM_op->op_pc;
        return res;
    }
    else{
        core->prev_mem_inst_pc = -1;
        return 0;
    }
}

int WB_ex(single_core* core){

    
    if(core->WB_op->code == HALT){
        return 1;
    }
    else{
        if((core->WB_op->rd != 0 && core->WB_op->rd != 1) && (core->WB_op->empty != 1)){ // Won't write to registers r0 and r1
            core->Reg_File[core->WB_op->rd] = core->WB_op->rd_val;
        }
        return 0;
    }
}

/*
void start_clock_cycle(){

    get_IF_ID_sprn(IF_ID->sprn);

    ID_EX->sprn->opcode = IF_ID->spro->opcode;
    ID_EX->sprn->rd = IF_ID->spro->rd;
    ID_EX->sprn->rs = IF_ID->spro->rs;
    ID_EX->sprn->rt = IF_ID->spro->rt;

    EX_MEM->sprn->opcode = ID_EX->spro->opcode;
    EX_MEM->sprn->rd = ID_EX->spro->rd;
    EX_MEM->sprn->rs = ID_EX->spro->rs;
    EX_MEM->sprn->rt = ID_EX->spro->rt;

    MEM_WB->sprn->opcode = EX_MEM->spro->opcode;
    MEM_WB->sprn->rd = EX_MEM->spro->rd;
    MEM_WB->sprn->rs = EX_MEM->spro->rs;
    MEM_WB->sprn->rt = EX_MEM->spro->rt;

}*/

void proceed_reg_data(operation *op1, operation *op2){

    op1->code = op2->code;
    op1->rd = op2->rd;
    op1->rs = op2->rs;
    op1->rt = op2->rt;
    op1->rd_val = op2->rd_val;
    op1->rs_val = op2->rs_val;
    op1->rt_val = op2->rt_val;
    op1->addr = op2->addr;
    op1->empty = op2->empty;
    op1->op_pc = op2->op_pc;
    op1->inst = op2->inst;
    op1->op_code = op2->op_code;

}
//reg D --> reg Q
void end_clock_sycle(single_core* core, int data_hazzard){

    if(data_hazzard && (!core->is_halt)){
        proceed_reg_data(core->WB_op, core->MEM_op);
        proceed_reg_data(core->MEM_op, core->EX_op);
        core->EX_op->empty = 1;
        core->core_stats->decode_stall++;
    }
    else{
        proceed_reg_data(core->WB_op, core->MEM_op);
        proceed_reg_data(core->MEM_op, core->EX_op);
        proceed_reg_data(core->EX_op, core->ID_op);
        proceed_reg_data(core->ID_op, core->IF_op);
    }
    //core->WB_op = core->MEM_op;
    //core->MEM_op = core->EX_op;
    //core->EX_op = core->ID_op;
    //core->ID_op = core->IF_op;

}

void print_trace(single_core* core, int clock_cycle){
    fprintf(core->trace_file,"%i ", clock_cycle);
    if(core->IF_op->empty == 1){
        fprintf(core->trace_file,"--- ");
    }
    else{
        fprintf(core->trace_file, "%03X ",core->IF_op->op_pc);
    }
    if(core->ID_op->empty == 1){
        fprintf(core->trace_file,"--- ");
    }
    else{
        fprintf(core->trace_file, "%03X ",core->ID_op->op_pc);
    }
    if(core->EX_op->empty == 1){
        fprintf(core->trace_file,"--- ");
    }
    else{
        fprintf(core->trace_file, "%03X ",core->EX_op->op_pc);
    }
    if(core->MEM_op->empty == 1){
        fprintf(core->trace_file,"--- ");
    }
    else{
        fprintf(core->trace_file, "%03X ",core->MEM_op->op_pc);
    }
    if(core->WB_op->empty == 1){
        fprintf(core->trace_file,"--- ");
    }
    else{
        fprintf(core->trace_file, "%03X ",core->WB_op->op_pc);
    }

    fprintf(core->trace_file,"%08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X \n", core->Reg_File[2], core->Reg_File[3], core->Reg_File[4], core->Reg_File[5], core->Reg_File[6], core->Reg_File[7], core->Reg_File[8], core->Reg_File[9], core->Reg_File[10], core->Reg_File[11], core->Reg_File[12], core->Reg_File[13], core->Reg_File[14], core->Reg_File[15]);
}

void print_regs(single_core* core, FILE* regs){
    for(int i = 2; i <= 15; i++){
        fprintf(regs, "%08X\n", core->Reg_File[i]);
    }
}

void print_stats(int core_num, single_core* core, FILE* stats_file){
    fprintf(stats_file, "cycles %i\n", (core->core_stats->cycles + 1)); // cycle count starts from 0
    fprintf(stats_file, "instructions %i\n", ((core->core_stats->cycles - 3)- core->core_stats->mem_stall - core->core_stats->decode_stall));
    fprintf(stats_file, "read_hit %i\n", (num_of_read_req[core_num] - num_of_read_miss[core_num]));
    fprintf(stats_file, "write_hit %i\n", (num_of_write_req[core_num] - num_of_write_miss[core_num]));
    fprintf(stats_file, "read_miss %i\n", num_of_read_miss[core_num]);
    fprintf(stats_file, "write_miss %i\n", num_of_write_miss[core_num]);
    fprintf(stats_file, "decode_stall %i\n", core->core_stats->decode_stall);
    fprintf(stats_file, "mem_stall %i\n", core->core_stats->mem_stall);
}
/* **********************************************************/
/*  ~~~~~~~~~~~~~    SIMP OP CODES OPERATIONS ~~~~~~~~~~~~  */
/* **********************************************************/
//1
void add(int core_num) {
    single_core *core = cores[core_num];
    core->EX_op->rd_val = core->EX_op->rs_val + core->EX_op->rt_val;

}

//2
void sub(int core_num) {
    single_core *core = cores[core_num];
    core->EX_op->rd_val = core->EX_op->rs_val - core->EX_op->rt_val;

}

//3
void and(int core_num) {
    single_core *core = cores[core_num];
    core->EX_op->rd_val = core->EX_op->rs_val & core->EX_op->rt_val;
}

//4
void or (int core_num) {
    single_core *core = cores[core_num];
    core->EX_op->rd_val = core->EX_op->rs_val | core->EX_op->rt_val;

}

//5
void xor (int core_num) {
    single_core *core = cores[core_num];
    core->EX_op->rd_val = core->EX_op->rs_val ^ core->EX_op->rt_val;

}

//6
void mul(int core_num) {
    single_core *core = cores[core_num];
    core->EX_op->rd_val = core->EX_op->rs_val * core->EX_op->rt_val;

}

//6
void sll(int core_num) {
    single_core *core = cores[core_num];
    core->EX_op->rd_val = core->EX_op->rs_val << core->EX_op->rt_val;

}

//7
void sra(int core_num) {
    single_core *core = cores[core_num];
    core->EX_op->rd_val = core->EX_op->rs_val >> core->EX_op->rt_val;

}

//8
void srl(int core_num) {
    single_core *core = cores[core_num];
    core->EX_op->rd_val = (int)core->EX_op->rs_val >> (int)core->EX_op->rt_val;

}

//16
void lw(int core_num) {
    single_core *core = cores[core_num];
    core->EX_op->addr = core->EX_op->rs_val + core->EX_op->rt_val;
}

//17
void sw(int core_num) {
    single_core *core = cores[core_num];
    core->EX_op->addr = core->EX_op->rs_val + core->EX_op->rt_val;
}


void nop(int core_num) {

}

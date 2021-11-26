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


void init_core(FILE* trace_file, FILE* imem, single_core* core, int core_num){
    core->pc = 0;
    core->is_halt = 0;
    read_imem(imem, core);
    core->trace_file = trace_file;
    core->Cache = (cache*)calloc(1, sizeof(cache));
    init_cache(core->Cache);
    core->IF_op = (operation*)calloc(1, sizeof(operation));
    core->IF_op->empty = 1;
    core->ID_op = (operation*)calloc(1, sizeof(operation));
    core->ID_op->empty = 1;
    core->EX_op = (operation*)calloc(1, sizeof(operation));
    core->EX_op->empty = 1;
    core->MEM_op = (operation*)calloc(1, sizeof(operation));
    core->MEM_op->empty = 1;
    core->WB_op = (operation*)calloc(1, sizeof(operation));
    core->WB_op->empty = 1;
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

void simulate_clock_cycle( int core_num, int clock_cycle, int* halt){

    int branch_taken;

    int cache_hit;

    single_core *core = cores[core_num];

    if(core->is_halt == 1){ // will be true only when the halt instruction in EX stage
        core->ID_op->empty = 1;
        core->IF_op->empty = 1;
    }

    //start_clock_cycle();
    cache_hit = MEM_ex(core);

    if(cache_hit != MISS){

            if (!(core->is_halt)){ // Halt command wasn't received yet 
                IF_ex(core);
            }
            else{ // Halt command was received
                core->IF_op->empty = 1;
            }

            branch_taken = ID_ex(core);
            
            EX_ex(core_num);

            print_trace(core, clock_cycle);

            *(halt) = WB_ex(core);

            end_clock_sycle(core);

            if (!branch_taken){
                core->pc++; 
            }
    }
    else{
        print_trace(core, clock_cycle);
    }

    
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
    core->ID_op->rd_val = core->Reg_File[core->ID_op->rd];
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
    /*
    printf("inst = %08x\n", inst);
    printf("code = %2x, rd = %i, rs = %i, rt = %i, imm = %i\n", core->ID_op->code, core->ID_op->rd, core->ID_op->rs, core->ID_op->rt, imm);
    printf("rd_val = %i, rs_val = %i, rt_val = %i, imm = %i\n", core->ID_op->rd_val, core->ID_op->rs_val, core->ID_op->rt_val, imm);
    */

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
            core->pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BNE):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val != core->ID_op->rt_val){
            core->pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BLT):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val < core->ID_op->rt_val){
            core->pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BJT):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val > core->ID_op->rt_val){
            core->pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BLE):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val <= core->ID_op->rt_val){
            core->pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BGE):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val >= core->ID_op->rt_val){
            core->pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(JAL):
		core->ID_op->op_code = &nop;
        next_pc = core->ID_op->rd_val & LSB_9BIT;
        core->ID_op->rd = 15;
        core->ID_op->rd_val = core->pc;
        core->pc = next_pc;
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
    // ID_op->op_code(operation,pc);

}

void EX_ex(int core_num){

    single_core *core = cores[core_num];

    core->ID_op->op_code(core_num);
}

int MEM_ex(single_core* core){
    if(core->MEM_op->code == LW){
        return read_word(core->MEM_op->addr, core->Cache, &(core->MEM_op->rd_val));
    }
    else if (core->MEM_op->code == SW){
        return write_word(core->MEM_op->addr, core->Cache, &(core->MEM_op->rd_val));
    }
    else{
        return 0;
    }
}

int WB_ex(single_core* core){
    
    if(core->WB_op->code == HALT){
        return 1;
    }
    else{
        if(core->WB_op->rd != 0 && core->WB_op->rd != 1){ // Won't write to registers r0 and r1
            core->Reg_File[core->WB_op->rd] = core->WB_op->rd_val;
            //printf("REG_FILE[%i] = %i\n", core->WB_op->rd, core->WB_op->rd_val);
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
void end_clock_sycle(single_core* core){
/*

    printf("IF pc = %03x\n", core->IF_op->op_pc);
    printf("ID pc = %03x\n", core->ID_op->op_pc);
    printf("EX pc = %03x\n", core->EX_op->op_pc);
    printf("MEM pc = %03x\n", core->MEM_op->op_pc);
    printf("WB pc = %03x\n", core->WB_op->op_pc);
    */

    proceed_reg_data(core->WB_op, core->MEM_op);
    proceed_reg_data(core->MEM_op, core->EX_op);
    proceed_reg_data(core->EX_op, core->ID_op);
    proceed_reg_data(core->ID_op, core->IF_op);

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
        fprintf(core->trace_file, "%03x ",core->IF_op->op_pc);
    }
    if(core->ID_op->empty == 1){
        fprintf(core->trace_file,"--- ");
    }
    else{
        fprintf(core->trace_file, "%03x ",core->ID_op->op_pc);
    }
    if(core->EX_op->empty == 1){
        fprintf(core->trace_file,"--- ");
    }
    else{
        fprintf(core->trace_file, "%03x ",core->EX_op->op_pc);
    }
    if(core->MEM_op->empty == 1){
        fprintf(core->trace_file,"--- ");
    }
    else{
        fprintf(core->trace_file, "%03x ",core->MEM_op->op_pc);
    }
    if(core->WB_op->empty == 1){
        fprintf(core->trace_file,"--- ");
    }
    else{
        fprintf(core->trace_file, "%03x ",core->WB_op->op_pc);
    }

    fprintf(core->trace_file,"%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x \n", core->Reg_File[2], core->Reg_File[3], core->Reg_File[4], core->Reg_File[5], core->Reg_File[6], core->Reg_File[7], core->Reg_File[8], core->Reg_File[9], core->Reg_File[10], core->Reg_File[11], core->Reg_File[12], core->Reg_File[13], core->Reg_File[14], core->Reg_File[15]);
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

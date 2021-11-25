#include "core.h"

void init_core(FILE* trace_file, FILE* imem, single_core* core){
    core->pc = 0;
    core->is_halt = 0;
    read_imem(imem, core);
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

void simulate_clock_cycle(single_core* core, int clock_cycle, int* halt){

    int branch_taken;

    int cache_hit;

    //start_clock_cycle();
    cache_hit = MEM_ex(core);

    if(cache_hit != MISS){
            branch_taken = ID_ex(core);

            if (!(core->is_halt)){ // Halt command wasn't received yet 
                IF_ex(core);
            }
            
            EX_ex(core);

            *(halt) = WB_ex(core);

            end_clock_sycle(core);

            if (!branch_taken){
                core->pc++; 
            }
    }

    print_trace(core, clock_cycle);
}

void IF_ex(single_core* core) {

    int line;

    line = core->Op_Mem[core->pc];

    core->IF_op->inst = line;
    core->IF_op->op_pc = core->pc;

}

int ID_ex(single_core* core){

    int imm;
    int branch_taken = 0;

    // Decoding the registers value
    int inst = core->ID_op->inst;
    imm = (inst & IMM_MASK);
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
        if(core->ID_op->rs_val <= core->ID_op->rt_val){
            core->pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BJT):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val >= core->ID_op->rt_val){
            core->pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BLE):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val < core->ID_op->rt_val){
            core->pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(BGE):
		core->ID_op->op_code = &nop;
        if(core->ID_op->rs_val > core->ID_op->rt_val){
            core->pc = core->ID_op->rd_val & LSB_9BIT;
            branch_taken = 1;
        }
        return branch_taken;
	case(JAL):
        int next_pc;
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

void EX_ex(single_core* core){

    core->ID_op->op_code(core);
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

//reg D --> reg Q
void end_clock_sycle(single_core* core){
/*
    IF_ID->spro->opcode = IF_ID->sprn->opcode;
    IF_ID->spro->rd = IF_ID->sprn->rd;
    IF_ID->spro->rs = IF_ID->sprn->rs;
    IF_ID->spro->rt = IF_ID->sprn->rt;

    ID_EX->spro->opcode = ID_EX->sprn->opcode;
    ID_EX->spro->rd = ID_EX->sprn->rd;
    ID_EX->spro->rs = ID_EX->sprn->rs;
    ID_EX->spro->rt = ID_EX->sprn->rt;

    EX_MEM->spro->opcode = EX_MEM->sprn->opcode;
    EX_MEM->spro->rd = EX_MEM->sprn->rd;
    EX_MEM->spro->rs = EX_MEM->sprn->rs;
    EX_MEM->spro->rt = EX_MEM->sprn->rt;

    MEM_WB->spro->opcode = MEM_WB->sprn->opcode;
    MEM_WB->spro->rd = MEM_WB->sprn->rd;
    MEM_WB->spro->rs = MEM_WB->sprn->rs;
    MEM_WB->spro->rt = MEM_WB->sprn->rt; */
      

    core->WB_op = core->MEM_op;
    core->MEM_op = core->EX_op;
    core->EX_op = core->ID_op;
    core->ID_op = core->IF_op;

}

void print_trace(single_core* core, int clock_cycle){
    fprintf(core->trace_file,"%i %03x %03x %03x %03x %03x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x \n", clock_cycle, core->IF_op->op_pc, core->ID_op->op_pc, core->EX_op->op_pc, core->MEM_op->op_pc, core->WB_op->op_pc, core->Reg_File[2], core->Reg_File[3], core->Reg_File[4], core->Reg_File[5], core->Reg_File[6], core->Reg_File[7], core->Reg_File[8], core->Reg_File[9], core->Reg_File[10], core->Reg_File[11], core->Reg_File[12], core->Reg_File[13], core->Reg_File[14], core->Reg_File[15]);
}
/* **********************************************************/
/*  ~~~~~~~~~~~~~    SIMP OP CODES OPERATIONS ~~~~~~~~~~~~  */
/* **********************************************************/
//1
void add(single_core* core) {

    core->EX_op->rd_val = core->EX_op->rs_val + core->EX_op->rt_val;

}

//2
void sub(single_core* core) {

    core->EX_op->rd_val = core->EX_op->rs_val - core->EX_op->rt_val;

}

//3
void and(single_core* core) {

    core->EX_op->rd_val = core->EX_op->rs_val & core->EX_op->rt_val;
}

//4
void or (single_core* core) {

    core->EX_op->rd_val = core->EX_op->rs_val | core->EX_op->rt_val;

}

//5
void xor (single_core* core) {

    core->EX_op->rd_val = core->EX_op->rs_val ^ core->EX_op->rt_val;

}

//6
void mul(single_core* core) {

    core->EX_op->rd_val = core->EX_op->rs_val * core->EX_op->rt_val;

}

//6
void sll(single_core* core) {

    core->EX_op->rd_val = core->EX_op->rs_val << core->EX_op->rt_val;

}

//7
void sra(single_core* core) {

    core->EX_op->rd_val = core->EX_op->rs_val >> core->EX_op->rt_val;

}

//8
void srl(single_core* core) {

    core->EX_op->rd_val = (int)core->EX_op->rs_val >> (int)core->EX_op->rt_val;

}

//16
void lw(single_core* core) {
    core->EX_op->addr = core->EX_op->rs_val + core->EX_op->rt_val;
}

//17
void sw(single_core* core) {
    core->EX_op->addr = core->EX_op->rs_val + core->EX_op->rt_val;
}


void nop(single_core* core) {

}

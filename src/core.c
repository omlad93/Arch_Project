#include "core.h"


void simulate_clock_cycle(int pc){

    char* line = NULL;

    start_clock_cycle();

    line = Op_Mem[pc];

    IF_ex(line);

    ID_ex();

    EX_ex();

    MEM_ex();

    WB_ex();

    end_clock_sycle();

}

void IF_ex(char* line) {
	int rd, rs, rt, code;
	char* temp = (char*)calloc(7, sizeof(char));

    IF_op->inst = line;

	//parse op type
	sprintf(temp, "%s", line);
	temp[2] = '\0';
	IF_op->code = strtol(&temp[0], NULL, 16);

	//parse rd
	sprintf(temp, "%s", line);
	temp[3] = '\0';
	IF_op->rd = strtol(&temp[2], NULL, 16);

	//parse rs
	sprintf(temp, "%s", line);
	temp[4] = '\0';
	IF_op->rs = strtol(&temp[3], NULL, 16);

	//parse rt
	sprintf(temp, "%s", line);
	IF_op->rt = strtol(&temp[4], NULL, 16);


}

void ID_ex(single_core* core){
    switch (ID_op->code) {
	case(ADD):
		core -> ID_op -> op_code = &add;
        return;
	case(SUB):
		core -> ID_op -> op_code =  &sub;
        return;
	case(AND):
		core -> ID_op -> op_code = &and;
        return;
	case(OR):
		core -> ID_op -> op_code = &or;
        return;
	case(XOR):
		core -> ID_op -> op_code = &xor;
        return;
	case(MUL):
		core -> ID_op -> op_code = &mul;
        return;
	case(SLL):
		core -> ID_op -> op_code = &sll;
        return;
	case(SRA):
		core -> ID_op -> op_code = &sra;
        return;
	case(SRL):
		core -> ID_op -> op_code = &srl;
        return;
	case(BEQ):
		core -> ID_op -> op_code = &beq;
        return;
	case(BNE):
		core -> ID_op -> op_code = &bne;
        return;
	case(BLT):
		core -> ID_op -> op_code = &blt;
        return;
	case(BJT):
		core -> ID_op -> op_code = &bgt;
        return;
	case(BLE):
		core -> ID_op -> op_code = &ble;
        return;
	case(BGE):
		core -> ID_op -> op_code = &bge;
        return;
	case(JAL):
		core -> ID_op -> op_code = &jal;
        return;
	case(LW):
		core -> ID_op -> op_code = &lw;
        return;
	case(SW):
		core -> ID_op -> op_code = &sw;
        return;
	case(HALT):
		core -> ID_op -> op_code = &halt;
        return;
	//if unknown opcode -> do nop
	default:
		core -> ID_op -> op_code = &nop;
        return;
	}

}

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

}

//reg D --> reg Q
void end_clock_sycle(){

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
    MEM_WB->spro->rt = MEM_WB->sprn->rt;   

    WB_op = MEM_op;
    MEM_op = EX_op;
    EX_op = ID_op;
    ID_op = IF_op;
    fetch_op(IF_op);

}

/* **********************************************************/
/*  ~~~~~~~~~~~~~    SIMP OP CODES OPERATIONS ~~~~~~~~~~~~  */
/* **********************************************************/
//1
int add(operation* op, int pc) {

}

//2
int sub(operation* op, int pc) {

}

//3
int and(operation* op, int pc) {

}

//4
int or (operation* op, int pc) {

}

//5
int xor (operation* op, int pc) {

}

//6
int mul(operation* op, int pc) {

}

//6
int sll(operation* op, int pc) {

}

//7
int sra(operation* op, int pc) {

}

//8
int srl(operation* op, int pc) {

}

//9
int beq(operation* op, int pc) {

}

//10
int bne(operation* op, int pc) {

}

//11
int blt(operation* op, int pc) {

}

//12
int bgt(operation* op, int pc) {

}

//13
int ble(operation* op, int pc) {

}

//14
int bge(operation* op, int pc) {

}

//15
int jal(operation* op, int pc) {

}

//16
int lw(operation* op, int pc) {

}

//17
int sw(operation* op, int pc) {


}


//21
int halt(operation* op, int pc) {

}

//assign to $0
int nop(operation*op, int pc) {

}

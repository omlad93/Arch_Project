#include "core.h"


void simulate_clock_cycle(){

    start_clock_cycle();

    IF_ex();

    ID_ex();

    EX_ex();

    MEM_ex();

    WB_ex();

    end_clock_sycle();

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
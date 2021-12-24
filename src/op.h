#ifndef OP_H
    #define OP_H
#endif
#ifndef CORE_H
	#include "core.h"
#endif

typedef struct operation {
	int code; //the code of the operation
	int rd; //rd register
	int rs; //rs register
	int rt; //rt register

	int rd_val; // value of rd @Decode
	int rs_val; // value of rs @Decode
	int rt_val; // value of rt @Decode

	int addr; // used in memory instructions

	int op_pc;
	int inst; // the opcode line from Imem
	int(*op_code)(single_core* core); // a function pointer according to opcode
	} operation;

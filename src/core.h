#include "cache.h"


/*
 * opcodes
 */
#define ADD  0
#define SUB  1
#define AND  2
#define OR   3
#define XOR  4
#define MUL  5
#define SLL  6
#define SRA  7
#define SRL  8
#define BEQ  9
#define BNE  10
#define BLT  11
#define BJT  12
#define BLE  13
#define BGE  14
#define JAL  15
#define LW   16
#define SW   17
#define HALT 20


typedef struct registers_s {
	// 8 bit opcode
	int opcode;

	// 4 bit destination register index
	int rd;

	// 4 bit source #0 register index
	int rs;

	// 4 bit source #1 register index
	int rt;

} registers_s;

typedef struct pipeline_registers {
		registers_s *spro, *sprn;
}pipeline_registers;

typedef struct operation {
	int rd; //rd register
	int rs; //rs register
	int rt; //rt register

	int imm_used; // and indicator of using immediate

	char* inst; // the opcode line from Imem
	//int(*op_code)(struct operation* op, int pc); // a function pointer according to opcode
} operation;


/*   SIMP REGISTERS    */
int REG_FILE[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
pipeline_registers *IF_ID;
pipeline_registers *ID_EX;
pipeline_registers *EX_MEM;
pipeline_registers *MEM_WB;


int pc;
int* ack;
//int  Data_Mem[4096];		//Data Memory aka DMEM
int Op_Mem[1024];		    //Operation Memory aka IME
cache *Cache;               //Cache 
operation *IF_op;           //Operation in Fetch 
operation *ID_op;           //Operation in Decode 
operation *EX_op;           //Operation in Execute 
operation *MEM_op;          //Operation in Memory
operation *WB_op;           //Operation in Write Back


void IF_ex();

void ID_ex();

void EX_ex();

void MEM_ex();

void WB_ex();

void start_clock_cycle();

void end_clock_sycle();

void fetch_op(operation* IF_op);

void get_IF_ID_sprn(pipeline_registers* sprn);


void simulate_clock_cycle();



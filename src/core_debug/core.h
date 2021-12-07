
#include "memory.h"
#ifndef CORE_H
#define CORE_H


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

/*
 * Parsing instraction macros - #FIXME
 */
#define OPP_SHFT 0x18 //24 bits
#define RD_SHFT 0x14 //20 bits
#define RS_SHFT 0x10 //16 bits
#define RT_SHFT 0xc //12 bits
#define OPP_MASK 0xFF000000 //bits 31:24
#define RD_MASK 0x00F00000 //bits 20-23
#define RS_MASK 0x000F0000 //bits 16-19
#define RT_MASK 0x0000F000 //bits 15-12
#define IMM_MASK 0x00000FFF //bits 11-0

#define SRL_MASK 0x7FFFFFFF
#define LSB_9BIT 0x000001FF
#define MEM_SIZE 1024

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
	int code; //the code of the operation
	int rd; //rd register
	int rs; //rs register
	int rt; //rt register

	int rd_val; // value of rd @Decode
	int rs_val; // value of rs @Decode
	int rt_val; // value of rt @Decode

	int addr; // used in memory instructions

	int empty; //indicates a bubble;

	int op_pc;
	int inst; // the opcode line from Imem
	void(*op_code)(int core_num); // a function pointer according to opcode
	} operation;

typedef struct core {
	FILE* trace_file;
	int is_halt;
	int pc;
	int next_pc;
	int data_hazzard;
	int prev_cache_miss;						
	int Op_Mem[MEM_SIZE];		//Operation Memory aka IMEM
	int Reg_File[16];
	
	operation *IF_op;           //Operation in Fetch 
	operation *ID_op;           //Operation in Decode 
	operation *EX_op;           //Operation in Execute 
	operation *MEM_op;          //Operation in Memory
	operation *WB_op;           //Operation in Write Back
	
	cache *Cache;               //Cache 

} single_core;

/*   SIMP REGISTERS    */
// int REG_FILE[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// pipeline_registers *IF_ID;
// pipeline_registers *ID_EX;
// pipeline_registers *EX_MEM;
// pipeline_registers *MEM_WB;

static single_core* cores[4]; // array of cores in the CPU
// int fuck_this(){
// 	return cores[0]->pc;
// } 

/* **********************************************************/
/*  ~~~~~~~~~~~~~    SIMP OP CODES OPERATIONS ~~~~~~~~~~~~  */
/* **********************************************************/
//1
void add(int core_num);

//2
void sub(int core_num);

//3
void and(int core_num);

//4
void or (int core_num);

//5
void xor (int core_num);

//6
void mul(int core_nume);

//6
void sll(int core_num);

//7
void sra(int core_num);

//8
void srl(int core_num);

//16
void lw(int core_num);

//17
void sw(int core_num);

//assign to $0 or unreconized behivour 
void nop(int core_num);

/* **********************************************************/
/*  ~~~~~~~~~~   Single core execution functions ~~~~~~~~~  */
/* **********************************************************/

void IF_ex(single_core* core);

//void fetch_op(single_core* core);

int ID_ex(single_core* core);

void EX_ex(int core_num);

int MEM_ex(single_core* core);

int WB_ex(single_core* core);

/* Simulates Single Core clock cycle.
receives the core, the number of the click cycle and a pointer to int that indicates wheter to end the execution of this core*/
void simulate_clock_cycle(int core_num, int clock_cycle, int* halt);

//void start_clock_cycle(single_core* core);

void end_clock_sycle(single_core* core, int data_hazzard);

void init_core(FILE* trace_file, FILE* imem, single_core* core, int core_num);

void read_imem(FILE* imem, single_core* core);

void print_trace(single_core* core, int clock_cycle);

int detect_data_hazzard(single_core* core);

void init_operation(operation* op);

//void get_IF_ID_sprn(pipeline_registers* sprn);

//void core_exec(single_core* core);

#endif



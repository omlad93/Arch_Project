#include "Memory/memory.h"git add


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

	int op_pc;

	//int imm_used; // and indicator of using immediate

	int inst; // the opcode line from Imem
	int(*op_code)(single_core* core); // a function pointer according to opcode
} operation;


typedef struct core {
	FILE* trace_file;
	int is_halt;
	int pc;						
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




/* **********************************************************/
/*  ~~~~~~~~~~~~~    SIMP OP CODES OPERATIONS ~~~~~~~~~~~~  */
/* **********************************************************/
//1
void add(single_core* core);

//2
void sub(single_core* core);

//3
void and(single_core* core);

//4
void or (single_core* core);

//5
void xor (single_core* core);

//6
void mul(single_core* core);

//6
void sll(single_core* core);

//7
void sra(single_core* core);

//8
void srl(single_core* core);

//16
void lw(single_core* core);

//17
void sw(single_core* core);

//assign to $0 or unreconized behivour 
void nop(single_core* core);

/* **********************************************************/
/*  ~~~~~~~~~~   Single core execution functions ~~~~~~~~~  */
/* **********************************************************/

void IF_ex(single_core* core);

//void fetch_op(single_core* core);

int ID_ex(single_core* core);

void EX_ex(single_core* core);

int MEM_ex(single_core* core);

int WB_ex(single_core* core);

/* Simulates Single Core clock cycle.
receives the core, the number of the click cycle and a pointer to int that indicates wheter to end the execution of this core*/
void simulate_clock_cycle(single_core* core, int clock_cycle, int* halt);

//void start_clock_cycle(single_core* core);

void end_clock_sycle(single_core* core);

void init_core(FILE* imem, single_core* core);

void read_imem(FILE* imem, single_core* core);

void print_trace(single_core* core, int clock_cycle);

//void get_IF_ID_sprn(pipeline_registers* sprn);

//void core_exec(single_core* core);





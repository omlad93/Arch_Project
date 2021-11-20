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

/*
 * Parsing instraction macros - #FIXME
 */
#define OPP_SHFT 0x19
#define RD_SHFT 0x16
#define RS_SHFT 0x13
#define RT_SHFT 0x10
#define OPP_MASK 0x3E000000 //bits 29-25
#define RD_MASK 0x01c00000 //bits 24-22
#define RS_MASK 0x00380000 //bits 21-19
#define RT_MASK 0x00070000 //bits 18-16

#define SRL_MASK 0x7FFFFFFF
#define LSB_9BIT 0x000001FF

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

	//int imm_used; // and indicator of using immediate

	int inst; // the opcode line from Imem
	int(*op_code)(single_core* core); // a function pointer according to opcode
} operation;


typedef struct core {
	int pc;						
	int Op_Mem[1024];		    //Operation Memory aka IME
	int Reg_File[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
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
int add(single_core* core);

//2
int sub(single_core* core);

//3
int and(single_core* core);

//4
int or (single_core* core);

//5
int xor (single_core* core);

//6
int mul(single_core* core);

//6
int sll(single_core* core);

//7
int sra(single_core* core);

//8
int srl(single_core* core);

//9
int beq(single_core* core);

//10
int bne(single_core* core);

//11
int blt(single_core* core);

//12
int bgt(single_core* core);

//13
int ble(single_core* core);

//14
int bge(single_core* core);

//15
int jal(single_core* core);

//16
int lw(single_core* core);

//17
int sw(single_core* core);

//21
int halt(single_core* core);

//assign to $0 or unreconized behivour 
int nop(single_core* core);

/* **********************************************************/
/*  ~~~~~~~~~~   Single core execution functions ~~~~~~~~~  */
/* **********************************************************/

void IF_ex(single_core* core);

//void fetch_op(single_core* core);

void ID_ex(single_core* core);

void EX_ex(single_core* core);

void MEM_ex(single_core* core);

void WB_ex(single_core* core);

void simulate_clock_cycle(single_core* core);

void start_clock_cycle(single_core* core);

void end_clock_sycle(single_core* core);

void init_core(FILE* imem, single_core* core);

void read_imem(FILE* imem, single_core* core);

//void get_IF_ID_sprn(pipeline_registers* sprn);

void core_exec(single_core* core);




